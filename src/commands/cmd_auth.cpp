/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_auth.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ezeppa <ezeppa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 14:30:00 by ezeppa            #+#    #+#             */
/*   Updated: 2026/03/03 15:43:19 by vbonnard           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandManager.hpp"
#include "Server.hpp"
#include "network/IrcReplies.hpp"
#include "Logger.hpp"

#include <cctype>
#include <ctime>
#include <set>
#include <sstream>
#include <vector>

namespace {
/** @brief Validate one nickname character. @param c Character to test. @return True when allowed by IRC nickname rules. */
bool isValidNickChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '[' || c == ']' ||
           c == '\\' || c == '`' || c == '^' || c == '{' || c == '}';
}

/** @brief Validate a nickname syntax. @param nick Candidate nickname. @return True when nickname is valid. */
bool isValidNickname(const std::string& nick) {
    if (nick.empty()) {
        return false;
    }
    if (!std::isalpha(static_cast<unsigned char>(nick[0]))) {
        return false;
    }
    for (size_t i = 1; i < nick.size(); ++i) {
        if (!isValidNickChar(nick[i])) {
            return false;
        }
    }
    return true;
}

/** @brief Check whether a nickname is already used by another client. @param server Server context. @param client Current client. @param nick Target nickname. @return True when nickname is taken. */
bool nicknameInUse(const Server& server, const Client& client, const std::string& nick) {
    std::map<int, Client*>& clients = server.clients();
    for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second && it->second->fd() != client.fd() && it->second->nickname() == nick) {
            return true;
        }
    }
    return false;
}

/** @brief Send numeric welcome reply to a newly registered client. @param server Server context. @param client Target client. */
void sendWelcome(Server& server, Client& client) {
    std::string msg = ":" + server.name() + " 001 " + client.nickname() +
                      " :Welcome to the Internet Relay Network " + client.fullmask() + "\r\n";
    Logger::log(Logger::INFO, client.fd(), "WELCOME sending: " + msg, client.nickname());
    client.appendToWriteBuffer(msg);
    server.handleClientWrite(client);
}

/** @brief Attempt final registration once PASS/NICK/USER requirements are met. @param server Server context. @param client Target client. */
void tryRegister(Server& server, Client& client) {
    std::ostringstream status;
    status << "tryRegister fd=" << client.fd()
           << " registered=" << client.isRegistered()
           << " nick=" << client.isNickReceived()
           << " user=" << client.isUserReceived()
           << " pass=" << client.isPassReceived()
           << " server_pass_set=" << (!server.pass().empty());
    Logger::log(Logger::INFO, client.fd(), status.str(), client.nickname());
    
    if (client.isRegistered()) {
        Logger::log(Logger::INFO, client.fd(), "tryRegister already registered, skipping", client.nickname());
        return;
    }
    if (!client.isNickReceived() || !client.isUserReceived()) {
        Logger::log(Logger::INFO, client.fd(), "tryRegister missing NICK or USER", client.nickname());
        return;
    }
    if (!server.pass().empty() && !client.isPassReceived()) {
        Logger::log(Logger::INFO, client.fd(), "tryRegister PASS required but not received", client.nickname());
        return;
    }
    Logger::log(Logger::INFO, client.fd(), "tryRegister complete, sending welcome", client.nickname());
    client.registered(true);
    sendWelcome(server, client);
}
}

void CommandManager::cmdPass(Server& server, Client& client, const std::vector<std::string>& args) {
    Logger::log(Logger::INFO, client.fd(), "PASS received", client.nickname());
    if (client.isRegistered()) {
        Logger::log(Logger::ERROR, client.fd(), "PASS rejected (already registered)", client.nickname());
        throw IrcReplies(ERR_ALREADYREGISTRED);
    }
    if (!server.pass().empty() && args[0] != server.pass()) {
        Logger::log(Logger::ERROR, client.fd(), "PASS mismatch", client.nickname());
        throw IrcReplies(ERR_PASSWDMISMATCH);
    }
    Logger::log(Logger::INFO, client.fd(), "PASS accepted", client.nickname());
    client.passReceived(true);
    tryRegister(server, client);
}

void CommandManager::cmdNick(Server& server, Client& client, const std::vector<std::string>& args) {
    Logger::log(Logger::INFO, client.fd(), "NICK received", client.nickname());
    if (args[0].empty()) {
        Logger::log(Logger::ERROR, client.fd(), "NICK missing", client.nickname());
        throw IrcReplies(ERR_NONICKNAMEGIVEN);
    }
    if (!isValidNickname(args[0])) {
        Logger::log(Logger::ERROR, client.fd(), "NICK invalid: " + args[0], client.nickname());
        throw IrcReplies(ERR_ERRONEUSNICKNAME, args[0]);
    }
    if (nicknameInUse(server, client, args[0])) {
        Logger::log(Logger::ERROR, client.fd(), "NICK in use: " + args[0], client.nickname());
        throw IrcReplies(ERR_NICKNAMEINUSE, args[0]);
    }
    Logger::log(Logger::INFO, client.fd(), "NICK accepted: " + args[0], client.nickname());
    std::string oldNick = client.nickname();
	client.nickname(args[0]);
    client.nickReceived(true);
	if (client.isRegistered()) {
		std::string msg = ":" + oldNick + "!" + client.username() + "@" + server.host() +
							" NICK :" + client.nickname() + "\r\n";
        client.appendToWriteBuffer(msg);
    	server.handleClientWrite(client);
	}
	else {
	    tryRegister(server, client);
	}
}

void CommandManager::cmdUser(Server& server, Client& client, const std::vector<std::string>& args) {
    Logger::log(Logger::INFO, client.fd(), "USER received", client.nickname());
    if (client.isRegistered()) {
        Logger::log(Logger::ERROR, client.fd(), "USER rejected (already registered)", client.nickname());
        throw IrcReplies(ERR_ALREADYREGISTRED);
    }
    client.username(args[0]);
    client.realname(args[3]);
    client.userReceived(true);
    Logger::log(Logger::INFO, client.fd(), "USER info set", client.nickname());
    tryRegister(server, client);
}

void CommandManager::cmdCap(Server& server, Client& client, const std::vector<std::string>& args) {
    if (!args.empty()) {
        Logger::log(Logger::INFO, client.fd(), "CAP " + args[0], client.nickname());
    } else {
        Logger::log(Logger::INFO, client.fd(), "CAP", client.nickname());
    }
    
    if (args.empty()) {
        return;
    }

    const std::string& sub = args[0];
    if (sub == "LS") {
        std::string msg = ":" + server.name() + " CAP * LS :\r\n";
        Logger::log(Logger::INFO, client.fd(), "CAP LS", client.nickname());
        client.appendToWriteBuffer(msg);
    } else if (sub == "REQ") {
        std::string msg = ":" + server.name() + " CAP * NAK :";
        if (args.size() > 1) {
            msg += args[1];
        }
        msg += "\r\n";
        Logger::log(Logger::INFO, client.fd(), "CAP REQ", client.nickname());
        client.appendToWriteBuffer(msg);
    } else if (sub == "END") {
        Logger::log(Logger::INFO, client.fd(), "CAP END", client.nickname());
    }
    server.handleClientWrite(client);
}

void CommandManager::cmdQuit(Server& server, Client& client, const std::vector<std::string>& args) {
    std::string reason = "Client Quit";
    if (!args.empty() && !args[0].empty()) {
        reason = args[0];
    }
    std::string msg = ":" + client.fullmask() + " QUIT :" + reason + "\r\n";

    std::vector<std::string> joinedChannels;
    std::map<std::string, Channel*>& channels = server.channels();
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (it->second && it->second->hasMember(client.nickname())) {
            joinedChannels.push_back(it->first);
        }
    }

    std::set<int> notifiedClients;
    std::map<int, Client*>& clients = server.clients();
    for (std::vector<std::string>::iterator chanIt = joinedChannels.begin(); chanIt != joinedChannels.end(); ++chanIt) {
        Channel* channel = server.getChannel(*chanIt);
        if (!channel) {
            continue;
        }

        for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if (!it->second || it->second->fd() == client.fd()) {
                continue;
            }
            if (!it->second->isInChannel(*chanIt)) {
                continue;
            }
            if (notifiedClients.insert(it->second->fd()).second) {
                it->second->appendToWriteBuffer(msg);
                server.handleClientWrite(*it->second);
            }
        }

        channel->removeMember(client.nickname());
        channel->removeOperator(client.nickname());
        std::string promoted = channel->promoteFirstMemberIfNoOps();
        client.leaveChannel(*chanIt);

        bool isEmpty = true;
        for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second && it->second->isInChannel(*chanIt)) {
                isEmpty = false;
                break;
            }
        }
        if (isEmpty) {
            delete channel;
            server.removeChannel(*chanIt);
        } else if (!promoted.empty()) {
            std::string modeMsg = ":" + server.name() + " MODE " + *chanIt + " +o " + promoted + "\r\n";
            for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
                if (it->second && it->second->isInChannel(*chanIt)) {
                    it->second->appendToWriteBuffer(modeMsg);
                    server.handleClientWrite(*it->second);
                }
            }
        }
    }

    client.appendToWriteBuffer(msg);
    server.handleClientWrite(client);
    client.fd(-1);
}

void CommandManager::cmdWho(Server& server, Client& client, const std::vector<std::string>& args) {
    std::string target = "*";
    if (!args.empty()) {
        target = args[0];
    }
    std::string msg = ":" + server.name() + " 315 " + client.nickname() + " " + target +
                      " :End of WHO list\r\n";
    client.appendToWriteBuffer(msg);
    server.handleClientWrite(client);
}

void CommandManager::cmdPing(Server& server, Client& client, const std::vector<std::string>& args) {
    client.lastPing(std::time(NULL));
    std::string msg = ":" + server.name() + " PONG " + server.name() + " :" + args[0] + "\r\n";
    client.appendToWriteBuffer(msg);
    server.handleClientWrite(client);
}

void CommandManager::cmdPong(Server& server, Client& client, const std::vector<std::string>& args) {
    (void)server;
    (void)args;
    client.lastPong(std::time(NULL));
}