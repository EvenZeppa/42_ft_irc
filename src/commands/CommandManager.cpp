#include "CommandManager.hpp"
#include "Server.hpp"
#include "network/IrcReplies.hpp"
#include <sstream>

CommandManager::CommandManager() {
    _commands["PASS"] = CommandEntry(CommandManager::cmdPass, false, 1);
    _commands["NICK"] = CommandEntry(CommandManager::cmdNick, false, 1);
    _commands["USER"] = CommandEntry(CommandManager::cmdUser, false, 4);
    _commands["CAP"]  = CommandEntry(CommandManager::cmdCap, false, 1);
    _commands["QUIT"] = CommandEntry(CommandManager::cmdQuit, false, 0);

    _commands["JOIN"] = CommandEntry(CommandManager::cmdJoin, true, 1);
    _commands["PART"] = CommandEntry(CommandManager::cmdPart, true, 1);
    _commands["TOPIC"] = CommandEntry(CommandManager::cmdTopic, true, 1);
    _commands["NAMES"] = CommandEntry(CommandManager::cmdNames, true, 0);

    _commands["MODE"] = CommandEntry(CommandManager::cmdMode, true, 1);

    _commands["KICK"] = CommandEntry(CommandManager::cmdKick, true, 2);
    _commands["INVITE"] = CommandEntry(CommandManager::cmdInvite, true, 2);

    _commands["PRIVMSG"] = CommandEntry(CommandManager::cmdPrivmsg, true, 2);
}

void CommandManager::executeCommand(Server& server, Client& client, std::string cmdName, std::vector<std::string> args) {
    try {
        if (_commands.find(cmdName) == _commands.end()) {
            throw IrcReplies(ERR_UNKNOWNCOMMAND);
        }

        CommandEntry entry = _commands[cmdName];

        if (entry.authRequired && !client.isRegistered()) {
            throw IrcReplies(ERR_NOTREGISTERED);
        }

        if (static_cast<int>(args.size()) < entry.minArgs) {
            throw IrcReplies(ERR_NEEDMOREPARAMS);
        }

        entry.func(server, client, args);
    } catch (const IrcReplies& reply) {
        std::string replyMsg = _buildReply(server, reply.Code(), reply.Arg(), "");
        client.appendToWriteBuffer(replyMsg);
    }
}   

std::string CommandManager::_buildReply(const Server& server, int code, std::string arg1, std::string arg2) {
    std::string msg = ":" + server.name() + " ";
    std::stringstream ss;
    ss << code;
    msg += ss.str() + " ";

    switch (code) {
        case RPL_WELCOME:
            msg += ":Welcome to the Internet Relay Network " + arg1;
            break;
        case RPL_UMODEIS:
            msg += ":Your user mode is " + arg1; // TODO: correction a faire ici
            break;
        case RPL_AWAY:
            msg += ":You are marked as away";
            break;
        case RPL_CHANNELMODEIS:
            msg += arg1 + " :Channel mode is set to " + arg2;
            break;
        case RPL_NOTOPIC:
            msg += arg1 + " :No topic is set";
            break;
        case RPL_TOPIC:
            msg += arg1 + " : " + arg2;
            break;
        case RPL_WHOISUSER:
            msg += arg1 + " " + arg2 + " :is a user"; // TODO: correction a faire ici
            break;
        case RPL_ENDOFWHO:
            msg += arg1 + " :End of WHO list";
            break;
        case RPL_NAMREPLY:
            msg += "= " + arg1 + " :User list"; // TODO: correction a faire ici
            break;
        case RPL_INVITING:
            msg += arg1 + arg2;
            break;
        case RPL_ENDOFNAMES:
            msg += arg1 + " :End of NAMES list";
            break;
        case RPL_UMODEUNKNOWNFLAG:
            msg += ":Unknown mode flag";
            break;
        case ERR_NOSUCHNICK:
            msg += arg1 + " :No such nick/channel";
            break;
        case ERR_NOSUCHCHANNEL:
            msg += arg1 + " :No such channel";
            break;
        case ERR_CANNOTSENDTOCHAN:
            msg += arg1 + " :Cannot send to channel";
            break;
        case ERR_TOOMANYCHANNELS:
            msg += arg1 + " :You have joined too many channels";
            break;
        case ERR_TOOMANYTARGETS:
            msg += arg1 + " :Too many targets";
            break;
        case ERR_NORECIPIENT:
            msg += ":No recipient given (" + arg1 + ")";
            break;
        case ERR_NOTEXTTOSEND:
            msg += ":No text to send";
            break;
        case ERR_NOTOPLEVEL:
            msg += arg1 + " :No toplevel domain specified";
            break;
        case ERR_WILDTOPLEVEL:
            msg += arg1 + " :Wildcard in toplevel domain";
            break;
        case ERR_UNKNOWNCOMMAND:
            msg += arg1 + " :Unknown command";
            break;
        case ERR_NOTREGISTERED:
            msg += ":You have not registered";
            break;
        case ERR_NEEDMOREPARAMS:
            msg += arg1 + " :Not enough parameters";
            break;
        case ERR_ALREADYREGISTRED:
            msg += ":You are already registered";
            break;
        case ERR_USERONCHANNEL  :
            msg += arg1 + " " + arg2 + " :is already on channel";
            break;
        case ERR_USERNOTINCHANNEL:
            msg += arg1 + " " + arg2 + " :They aren't on that channel";
            break;
        case ERR_KEYSET:
            msg += arg1 + " :Channel key already set";
            break;
        case ERR_CHANNELISFULL:
            msg += arg1 + " :Channel is full";
            break;
        case ERR_UNKNOWNMODE:
            msg += arg1 + " :Unknown mode";
            break;
        case ERR_INVITEONLYCHAN:
            msg += arg1 + " :Invite only channel";
            break;
        case ERR_BADCHANNELKEY:
            msg += arg1 + " :Bad channel key";
            break;
        case ERR_CHANOPRIVSNEEDED:
            msg += arg1 + " :Channel operator privileges needed";
            break;
        case ERR_USERSDONTMATCH:
            msg += ":Users don't match";
            break;
        case ERR_ERRONEUSNICKNAME:
            msg += arg1 + " :Erroneous nickname";
            break;
        case ERR_NICKNAMEINUSE:
            msg += arg1 + " :Nickname is already in use";
            break;
        case ERR_NICKCOLLISION:
            msg += arg1 + " :Nickname collision";
            break;
        case ERR_NONICKNAMEGIVEN:
            msg += ":No nickname given";
            break;
        case ERR_NOTONCHANNEL:
            msg += arg1 + " :You're not on that channel";
            break;
        case ERR_PASSWDMISMATCH:
            msg += ":Password incorrect";
            break;
        default:
            msg += ":Unknown error";
            break;
    }
    return msg + "\r\n";
}



