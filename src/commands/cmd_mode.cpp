#include "CommandManager.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "network/IrcReplies.hpp"
#include <sstream>

namespace {
    /** @brief Convert decimal string to integer. @param str Input string. @return Parsed integer value. */
    int stringToInt(const std::string& str) {
        std::stringstream ss(str);
        int val;
        ss >> val;
        return val;
    }
}

void CommandManager::cmdMode(Server& server, Client& client, const std::vector<std::string>& args) {
    std::string target = args[0];

    if (target[0] != '#') {
        if (target != client.nickname()) {
            throw IrcReplies(ERR_USERSDONTMATCH);
        }

        if (args.size() == 1) {
            std::string modeStr = "+";
            if (client.hasMode('i')) modeStr += "i";
            std::string msg = ":" + server.name() + " 221 " + client.nickname() + " " + modeStr + "\r\n";
            client.appendToWriteBuffer(msg);
            server.handleClientWrite(client);
            return;
        }

        std::string modeString = args[1];
        bool add = true;
        for (size_t i = 0; i < modeString.size(); ++i) {
            if (modeString[i] == '+') {
                add = true;
            } else if (modeString[i] == '-') {
                add = false;
            } else if (modeString[i] == 'i') {
                if (add) {
                    client.addMode('i');
                } else {
                    client.removeMode('i');
                }
            }
        }
        return;
    }

    Channel* channel = server.getChannel(target);
    if (!channel) {
        throw IrcReplies(ERR_NOSUCHCHANNEL, target);
    }

    if (!channel->hasMember(client.nickname())) {
        throw IrcReplies(ERR_NOTONCHANNEL, target);
    }

    if (args.size() == 1) {
        std::string modeStr = "+";
        if (channel->hasMode('i')) modeStr += "i";
        if (channel->hasMode('t')) modeStr += "t";
        if (channel->hasMode('k')) modeStr += "k";
        if (channel->hasMode('l')) modeStr += "l";
        
        std::string msg = ":" + server.name() + " 324 " + client.nickname() + 
                          " " + target + " " + modeStr + "\r\n";
        client.appendToWriteBuffer(msg);
        server.handleClientWrite(client);
        return;
    }

    if (!channel->hasOperator(client.nickname())) {
        throw IrcReplies(ERR_CHANOPRIVSNEEDED, target);
    }

    std::string modeString = args[1];
    size_t paramIndex = 2;
    bool add = true;
    std::string appliedModes;
    std::string appliedParams;

    for (size_t i = 0; i < modeString.size(); ++i) {
        char mode = modeString[i];
        
        if (mode == '+') {
            add = true;
            continue;
        } else if (mode == '-') {
            add = false;
            continue;
        }

        switch (mode) {
            case 'i':
                if (add) {
                    channel->addMode('i');
                    appliedModes += "+i";
                } else {
                    channel->removeMode('i');
                    appliedModes += "-i";
                }
                break;

            case 't':
                if (add) {
                    channel->addMode('t');
                    appliedModes += "+t";
                } else {
                    channel->removeMode('t');
                    appliedModes += "-t";
                }
                break;

            case 'k':
                if (add) {
                    if (paramIndex < args.size()) {
                        channel->key(args[paramIndex]);
                        channel->addMode('k');
                        appliedModes += "+k";
                        appliedParams += " " + args[paramIndex];
                        paramIndex++;
                    }
                } else {
                    channel->key("");
                    channel->removeMode('k');
                    appliedModes += "-k";
                }
                break;

            case 'l':
                if (add) {
                    if (paramIndex < args.size()) {
                        int limit = stringToInt(args[paramIndex]);
                        channel->limit(limit);
                        channel->addMode('l');
                        appliedModes += "+l";
                        appliedParams += " " + args[paramIndex];
                        paramIndex++;
                    }
                } else {
                    channel->limit(0);
                    channel->removeMode('l');
                    appliedModes += "-l";
                }
                break;

            case 'o':
                if (paramIndex < args.size()) {
                    std::string targetNick = args[paramIndex];
                    if (channel->hasMember(targetNick)) {
                        if (add) {
                            channel->addOperator(targetNick);
                            appliedModes += "+o";
                        } else {
                            channel->removeOperator(targetNick);
                            appliedModes += "-o";
                        }
                        appliedParams += " " + targetNick;
                    }
                    paramIndex++;
                }
                break;

            default:
                throw IrcReplies(ERR_UNKNOWNMODE, std::string(1, mode));
        }
    }

    if (!appliedModes.empty()) {
        std::string modeMsg = ":" + client.fullmask() + " MODE " + target + 
                              " " + appliedModes + appliedParams + "\r\n";
        std::map<int, Client*>& clients = server.clients();
        for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second && it->second->isInChannel(target)) {
                it->second->appendToWriteBuffer(modeMsg);
                server.handleClientWrite(*it->second);
            }
        }
    }
}