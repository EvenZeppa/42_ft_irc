// PASS, NICK, USER, CAP, QUIT, WHO, PING, PONG

#include "CommandManager.hpp"
#include "Server.hpp"
#include "network/IrcReplies.hpp"

#include <cctype>
#include <ctime>
#include <set>
#include <vector>

namespace {
bool isValidNickChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '[' || c == ']' ||
           c == '\\' || c == '`' || c == '^' || c == '{' || c == '}';
}

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

bool nicknameInUse(const Server& server, const Client& client, const std::string& nick) {
    std::map<int, Client*>& clients = server.clients();
    for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second && it->second->fd() != client.fd() && it->second->nickname() == nick) {
            return true;
        }
    }
    return false;
}

void sendWelcome(Server& server, Client& client) {
    std::string msg = ":" + server.name() + " 001 " + client.nickname() +
                      " :Welcome to the Internet Relay Network " + client.fullmask() + "\r\n";
    std::cout << "[WELCOME] Sending to fd=" << client.fd() << ": " << msg;
    client.appendToWriteBuffer(msg);
    server.handleClientWrite(client);
}

void tryRegister(Server& server, Client& client) {
    std::cout << "[tryRegister] fd=" << client.fd() 
              << " registered=" << client.isRegistered()
              << " nick=" << client.isNickReceived()
              << " user=" << client.isUserReceived()
              << " pass=" << client.isPassReceived()
              << " server_pass_set=" << (!server.pass().empty()) << std::endl;
    
    if (client.isRegistered()) {
        std::cout << "[tryRegister] Already registered, skipping" << std::endl;
        return;
    }
    if (!client.isNickReceived() || !client.isUserReceived()) {
        std::cout << "[tryRegister] Missing NICK or USER" << std::endl;
        return;
    }
    if (!server.pass().empty() && !client.isPassReceived()) {
        std::cout << "[tryRegister] Server requires password but PASS not received" << std::endl;
        return;
    }
    std::cout << "[tryRegister] Registration complete! Sending welcome" << std::endl;
    client.registered(true);
    sendWelcome(server, client);
}
} // namespace

void CommandManager::cmdPass(Server& server, Client& client, const std::vector<std::string>& args) {
    std::cout << "[PASS] Client fd=" << client.fd() << " pass=" << args[0] << std::endl;
    if (client.isRegistered()) {
        std::cout << "[PASS] Already registered" << std::endl;
        throw IrcReplies(ERR_ALREADYREGISTRED);
    }
    if (!server.pass().empty() && args[0] != server.pass()) {
        std::cout << "[PASS] Password mismatch: expected=" << server.pass() << " got=" << args[0] << std::endl;
        throw IrcReplies(ERR_PASSWDMISMATCH);
    }
    std::cout << "[PASS] Password accepted" << std::endl;
    client.passReceived(true);
    tryRegister(server, client);
}

void CommandManager::cmdNick(Server& server, Client& client, const std::vector<std::string>& args) {
    std::cout << "[NICK] Client fd=" << client.fd() << " nick=" << args[0] << std::endl;
    if (args[0].empty()) {
        std::cout << "[NICK] No nickname given" << std::endl;
        throw IrcReplies(ERR_NONICKNAMEGIVEN);
    }
    if (!isValidNickname(args[0])) {
        std::cout << "[NICK] Invalid nickname: " << args[0] << std::endl;
        throw IrcReplies(ERR_ERRONEUSNICKNAME, args[0]);
    }
    if (nicknameInUse(server, client, args[0])) {
        std::cout << "[NICK] Nickname in use: " << args[0] << std::endl;
        throw IrcReplies(ERR_NICKNAMEINUSE, args[0]);
    }
    std::cout << "[NICK] Nickname accepted: " << args[0] << std::endl;
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
    std::cout << "[USER] Client fd=" << client.fd() << " user=" << args[0] << " realname=" << args[3] << std::endl;
    if (client.isRegistered()) {
        std::cout << "[USER] Already registered" << std::endl;
        throw IrcReplies(ERR_ALREADYREGISTRED);
    }
    client.username(args[0]);
    client.realname(args[3]);
    client.userReceived(true);
    std::cout << "[USER] User info set" << std::endl;
    tryRegister(server, client);
}

void CommandManager::cmdCap(Server& server, Client& client, const std::vector<std::string>& args) {
    std::cout << "[CAP] Client fd=" << client.fd();
    if (!args.empty()) {
        std::cout << " sub=" << args[0];
    }
    std::cout << std::endl;
    
    if (args.empty()) {
        return;
    }

    const std::string& sub = args[0];
    if (sub == "LS") {
        std::string msg = ":" + server.name() + " CAP * LS :\r\n";
        std::cout << "[CAP LS] Sending: " << msg;
        client.appendToWriteBuffer(msg);
    } else if (sub == "REQ") {
        std::string msg = ":" + server.name() + " CAP * NAK :";
        if (args.size() > 1) {
            msg += args[1];
        }
        msg += "\r\n";
        std::cout << "[CAP REQ] Sending: " << msg;
        client.appendToWriteBuffer(msg);
    } else if (sub == "END") {
        std::cout << "[CAP END] Client finished capability negotiation" << std::endl;
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