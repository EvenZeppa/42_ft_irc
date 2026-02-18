// JOIN, PART, TOPIC, NAMES

#include "CommandManager.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "network/IrcReplies.hpp"
#include <ctime>
#include <sstream>

void CommandManager::cmdJoin(Server& server, Client& client, const std::vector<std::string>& args) {
    std::string channelName = args[0];
    std::string key;
    if (args.size() > 1) {
        key = args[1];
    }

    // Validate channel name
    if (channelName.empty() || channelName[0] != '#') {
        throw IrcReplies(ERR_NOSUCHCHANNEL, channelName);
    }

    Channel* channel = server.getChannel(channelName);
    
    // Create channel if it doesn't exist
    if (!channel) {
        channel = new Channel(channelName);
        server.addChannel(channel);
        // First user becomes operator
        channel->addMember(client.nickname());
        channel->addOperator(client.nickname());
        client.joinChannel(channelName);
    } else {
        // Check invite-only
        if (channel->isInviteOnly() && !channel->hasInvited(client.nickname())) {
            throw IrcReplies(ERR_INVITEONLYCHAN, channelName);
        }
        
        // Check key
        if (channel->isKeyMode() && channel->key() != key) {
            throw IrcReplies(ERR_BADCHANNELKEY, channelName);
        }
        
        // Check limit
        if (channel->isLimitMode() && channel->limit() > 0) {
            size_t memberCount = 0;
            std::map<int, Client*>& clients = server.clients();
            for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
                if (it->second && it->second->isInChannel(channelName)) {
                    memberCount++;
                }
            }
            if (memberCount >= channel->limit()) {
                throw IrcReplies(ERR_CHANNELISFULL, channelName);
            }
        }
        
        // Add member
        channel->addMember(client.nickname());
        channel->removeInvited(client.nickname());
        client.joinChannel(channelName);
    }

    // Send JOIN message to all channel members
    std::string joinMsg = ":" + client.fullmask() + " JOIN :" + channelName + "\r\n";
    std::map<int, Client*>& clients = server.clients();
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second && it->second->isInChannel(channelName)) {
            it->second->appendToWriteBuffer(joinMsg);
            server.handleClientWrite(*it->second);
        }
    }

    // Send topic if exists
    if (!channel->topic().empty()) {
        std::string topicMsg = ":" + server.name() + " 332 " + client.nickname() + 
                               " " + channelName + " :" + channel->topic() + "\r\n";
        client.appendToWriteBuffer(topicMsg);
        server.handleClientWrite(client);
    } else {
        std::string noTopicMsg = ":" + server.name() + " 331 " + client.nickname() + 
                                 " " + channelName + " :No topic is set\r\n";
        client.appendToWriteBuffer(noTopicMsg);
        server.handleClientWrite(client);
    }

    // Send NAMES list
    cmdNames(server, client, args);
}

void CommandManager::cmdPart(Server& server, Client& client, const std::vector<std::string>& args) {
    std::string channelName = args[0];
    std::string reason = "Leaving";
    if (args.size() > 1) {
        reason = args[1];
    }

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        throw IrcReplies(ERR_NOSUCHCHANNEL, channelName);
    }

    if (!channel->hasMember(client.nickname())) {
        throw IrcReplies(ERR_NOTONCHANNEL, channelName);
    }

    // Send PART message to all channel members
    std::string partMsg = ":" + client.fullmask() + " PART " + channelName + " :" + reason + "\r\n";
    std::map<int, Client*>& clients = server.clients();
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second && it->second->isInChannel(channelName)) {
            it->second->appendToWriteBuffer(partMsg);
            server.handleClientWrite(*it->second);
        }
    }

    // Remove member
    channel->removeMember(client.nickname());
    channel->removeOperator(client.nickname());
    client.leaveChannel(channelName);

    // Remove empty channel
    bool isEmpty = true;
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second && it->second->isInChannel(channelName)) {
            isEmpty = false;
            break;
        }
    }
    if (isEmpty) {
        server.removeChannel(channelName);
    }
}

void CommandManager::cmdTopic(Server& server, Client& client, const std::vector<std::string>& args) {
    std::string channelName = args[0];
    
    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        throw IrcReplies(ERR_NOSUCHCHANNEL, channelName);
    }

    if (!channel->hasMember(client.nickname())) {
        throw IrcReplies(ERR_NOTONCHANNEL, channelName);
    }

    // View topic
    if (args.size() == 1) {
        if (channel->topic().empty()) {
            throw IrcReplies(RPL_NOTOPIC, channelName);
        } else {
            std::string topicMsg = ":" + server.name() + " 332 " + client.nickname() + 
                                   " " + channelName + " :" + channel->topic() + "\r\n";
            client.appendToWriteBuffer(topicMsg);
            server.handleClientWrite(client);
        }
        return;
    }

    // Set topic
    if (channel->isTopicRestricted() && !channel->hasOperator(client.nickname())) {
        throw IrcReplies(ERR_CHANOPRIVSNEEDED, channelName);
    }

    std::string newTopic = args[1];
    channel->topic(newTopic);
    channel->topicSetter(client.nickname());
    channel->topicSetTime(std::time(NULL));

    // Broadcast topic change
    std::string topicMsg = ":" + client.fullmask() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
    std::map<int, Client*>& clients = server.clients();
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second && it->second->isInChannel(channelName)) {
            it->second->appendToWriteBuffer(topicMsg);
            server.handleClientWrite(*it->second);
        }
    }
}

void CommandManager::cmdNames(Server& server, Client& client, const std::vector<std::string>& args) {
    std::string channelName;
    if (!args.empty()) {
        channelName = args[0];
    }

    if (channelName.empty()) {
        // List all channels (simplified)
        std::string endMsg = ":" + server.name() + " 366 " + client.nickname() + " * :End of NAMES list\r\n";
        client.appendToWriteBuffer(endMsg);
        server.handleClientWrite(client);
        return;
    }

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        throw IrcReplies(ERR_NOSUCHCHANNEL, channelName);
    }

    // Build names list
    std::string namesList;
    std::map<int, Client*>& clients = server.clients();
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second && it->second->isInChannel(channelName)) {
            if (!namesList.empty()) {
                namesList += " ";
            }
            if (channel->hasOperator(it->second->nickname())) {
                namesList += "@";
            }
            namesList += it->second->nickname();
        }
    }

    // Send names reply
    std::string namesMsg = ":" + server.name() + " 353 " + client.nickname() + 
                           " = " + channelName + " :" + namesList + "\r\n";
    client.appendToWriteBuffer(namesMsg);
    server.handleClientWrite(client);

    // Send end of names
    std::string endMsg = ":" + server.name() + " 366 " + client.nickname() + 
                         " " + channelName + " :End of NAMES list\r\n";
    client.appendToWriteBuffer(endMsg);
    server.handleClientWrite(client);
}