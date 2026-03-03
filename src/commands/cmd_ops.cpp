/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_ops.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ezeppa <ezeppa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 00:00:00 by ezeppa            #+#    #+#             */
/*   Updated: 2026/03/03 00:00:00 by vbonnard           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandManager.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "network/IrcReplies.hpp"

void CommandManager::cmdKick(Server& server, Client& client, const std::vector<std::string>& args) {
    std::string channelName = args[0];
    std::string targetNick = args[1];
    std::string reason = "Kicked";
    if (args.size() > 2) {
        reason = args[2];
    }

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        throw IrcReplies(ERR_NOSUCHCHANNEL, channelName);
    }

    if (!channel->hasMember(client.nickname())) {
        throw IrcReplies(ERR_NOTONCHANNEL, channelName);
    }

    if (!channel->hasOperator(client.nickname())) {
        throw IrcReplies(ERR_CHANOPRIVSNEEDED, channelName);
    }

    if (!channel->hasMember(targetNick)) {
        throw IrcReplies(ERR_USERNOTINCHANNEL, targetNick);
    }

    std::string kickMsg = ":" + client.fullmask() + " KICK " + channelName + 
                          " " + targetNick + " :" + reason + "\r\n";
    std::map<int, Client*>& clients = server.clients();
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second && it->second->isInChannel(channelName)) {
            it->second->appendToWriteBuffer(kickMsg);
            server.handleClientWrite(*it->second);
        }
    }

    channel->removeMember(targetNick);
    channel->removeOperator(targetNick);
    
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second && it->second->nickname() == targetNick) {
            it->second->leaveChannel(channelName);
            break;
        }
    }
}

void CommandManager::cmdInvite(Server& server, Client& client, const std::vector<std::string>& args) {
    std::string targetNick = args[0];
    std::string channelName = args[1];

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        throw IrcReplies(ERR_NOSUCHCHANNEL, channelName);
    }

    if (!channel->hasMember(client.nickname())) {
        throw IrcReplies(ERR_NOTONCHANNEL, channelName);
    }

    if (channel->isInviteOnly() && !channel->hasOperator(client.nickname())) {
        throw IrcReplies(ERR_CHANOPRIVSNEEDED, channelName);
    }

    Client* targetClient = NULL;
    std::map<int, Client*>& clients = server.clients();
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second && it->second->nickname() == targetNick) {
            targetClient = it->second;
            break;
        }
    }

    if (!targetClient) {
        throw IrcReplies(ERR_NOSUCHNICK, targetNick);
    }

    if (channel->hasMember(targetNick)) {
        throw IrcReplies(ERR_USERONCHANNEL, targetNick);
    }

    channel->addInvited(targetNick);

    std::string inviteMsg = ":" + client.fullmask() + " INVITE " + targetNick + 
                            " :" + channelName + "\r\n";
    targetClient->appendToWriteBuffer(inviteMsg);
    server.handleClientWrite(*targetClient);

    std::string confirmMsg = ":" + server.name() + " 341 " + client.nickname() + 
                             " " + targetNick + " " + channelName + "\r\n";
    client.appendToWriteBuffer(confirmMsg);
    server.handleClientWrite(client);
}