/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_notice.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ezeppa <ezeppa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+            */
/*   Created: 2026/03/17 00:00:00 by ezeppa            #+#    #+#              */
/*   Updated: 2026/03/17 00:00:00 by vbonnard           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandManager.hpp"
#include "Server.hpp"
#include "Channel.hpp"

/**
 * NOTICE - Like PRIVMSG but no automatic replies (required for DCC/CTCP file transfer).
 * Forwards messages as-is, including CTCP (\x01...\x01) for DCC SEND.
 */
void CommandManager::cmdNotice(Server& server, Client& client, const std::vector<std::string>& args) {
    if (args.size() < 2 || args[0].empty() || args[1].empty()) {
        return;
    }

    std::string target = args[0];
    std::string message = args[1];

    if (target[0] == '#') {
        Channel* channel = server.getChannel(target);
        if (!channel || !channel->hasMember(client.nickname())) {
            return;
        }

        std::string msg = ":" + client.fullmask() + " NOTICE " + target + " :" + message + "\r\n";
        std::map<int, Client*>& clients = server.clients();
        for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second && it->second->isInChannel(target) &&
                it->second->nickname() != client.nickname()) {
                it->second->appendToWriteBuffer(msg);
                server.handleClientWrite(*it->second);
            }
        }
    } else {
        Client* targetClient = NULL;
        std::map<int, Client*>& clients = server.clients();
        for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second && it->second->nickname() == target) {
                targetClient = it->second;
                break;
            }
        }

        if (targetClient) {
            std::string msg = ":" + client.fullmask() + " NOTICE " + target + " :" + message + "\r\n";
            targetClient->appendToWriteBuffer(msg);
            server.handleClientWrite(*targetClient);
        }
    }
}
