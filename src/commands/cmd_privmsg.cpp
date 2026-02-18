// PRIVMSG

#include "CommandManager.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "network/IrcReplies.hpp"

void CommandManager::cmdPrivmsg(Server& server, Client& client, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        throw IrcReplies(ERR_NEEDMOREPARAMS, "PRIVMSG");
    }

    std::string target = args[0];
    std::string message = args[1];

    if (target.empty()) {
        throw IrcReplies(ERR_NORECIPIENT, "PRIVMSG");
    }

    if (message.empty()) {
        throw IrcReplies(ERR_NOTEXTTOSEND);
    }

    // Channel message
    if (target[0] == '#') {
        Channel* channel = server.getChannel(target);
        if (!channel) {
            throw IrcReplies(ERR_NOSUCHCHANNEL, target);
        }

        if (!channel->hasMember(client.nickname())) {
            throw IrcReplies(ERR_CANNOTSENDTOCHAN, target);
        }

        // Send to all channel members except sender
        std::string msg = ":" + client.fullmask() + " PRIVMSG " + target + " :" + message + "\r\n";
        std::map<int, Client*>& clients = server.clients();
        for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second && it->second->isInChannel(target) && 
                it->second->nickname() != client.nickname()) {
                it->second->appendToWriteBuffer(msg);
                server.handleClientWrite(*it->second);
            }
        }
    } else {
        // Private message to user
        Client* targetClient = NULL;
        std::map<int, Client*>& clients = server.clients();
        for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second && it->second->nickname() == target) {
                targetClient = it->second;
                break;
            }
        }

        if (!targetClient) {
            throw IrcReplies(ERR_NOSUCHNICK, target);
        }

        std::string msg = ":" + client.fullmask() + " PRIVMSG " + target + " :" + message + "\r\n";
        targetClient->appendToWriteBuffer(msg);
        server.handleClientWrite(*targetClient);
    }
}