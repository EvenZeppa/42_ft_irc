#pragma once

#include "Client.hpp"
#include <vector>
#include <map>
#include <string>

class Server;

typedef void (*CommandFunc)(Server&,Client&, const std::vector<std::string>&);

struct CommandEntry{
    CommandFunc func;
    bool authRequired;
    int minArgs;

    /** @brief Build a default invalid command entry. */
    CommandEntry () : func(NULL), authRequired(false), minArgs(0) {}
    /**
     * @brief Build a command entry.
     * @param f Command handler function.
     * @param auth Whether registration is required.
     * @param minA Minimum number of arguments.
     */
    CommandEntry(CommandFunc f, bool auth, int minA)
        : func(f), authRequired(auth), minArgs(minA) {}

};

class CommandManager {
private:
    std::map<std::string, CommandEntry> _commands;
    /**
     * @brief Build an IRC numeric reply line.
     * @param server Server context used for server name.
     * @param code Numeric reply code.
     * @param arg1 First formatting argument.
     * @param arg2 Second formatting argument.
     * @return Fully formatted IRC line ending with CRLF.
     */
    std::string _buildReply(const Server& server, int code, std::string arg1, std::string arg2);


public:
    /** @brief Construct command registry and bind handlers. */
    CommandManager();
    /**
     * @brief Dispatch a command and handle IRC exceptions.
     * @param server Server instance.
     * @param client Client issuing the command.
     * @param cmdName Command name.
     * @param args Parsed command arguments.
     */
    void executeCommand(Server& server, Client& client, std::string cmdName, std::vector<std::string> args);

    /** @brief Handle PASS command. @param server Server context. @param client Requesting client. @param args Command arguments. */
    static void cmdPass(Server& server, Client& client, const std::vector<std::string>& args);
    /** @brief Handle NICK command. @param server Server context. @param client Requesting client. @param args Command arguments. */
    static void cmdNick(Server& server, Client& client, const std::vector<std::string>& args);
    /** @brief Handle USER command. @param server Server context. @param client Requesting client. @param args Command arguments. */
    static void cmdUser(Server& server, Client& client, const std::vector<std::string>& args);
    /** @brief Handle CAP command. @param server Server context. @param client Requesting client. @param args Command arguments. */
    static void cmdCap(Server& server, Client& client, const std::vector<std::string>& args);
    /** @brief Handle QUIT command. @param server Server context. @param client Requesting client. @param args Command arguments. */
    static void cmdQuit(Server& server, Client& client, const std::vector<std::string>& args); 
    /** @brief Handle WHO command. @param server Server context. @param client Requesting client. @param args Command arguments. */
    static void cmdWho(Server& server, Client& client, const std::vector<std::string>& args);
    /** @brief Handle PING command. @param server Server context. @param client Requesting client. @param args Command arguments. */
    static void cmdPing(Server& server, Client& client, const std::vector<std::string>& args);
    /** @brief Handle PONG command. @param server Server context. @param client Requesting client. @param args Command arguments. */
    static void cmdPong(Server& server, Client& client, const std::vector<std::string>& args);

    /** @brief Handle JOIN command. @param server Server context. @param client Requesting client. @param args Command arguments. */
    static void cmdJoin(Server& server, Client& client, const std::vector<std::string>& args);
    /** @brief Handle PART command. @param server Server context. @param client Requesting client. @param args Command arguments. */
    static void cmdPart(Server& server, Client& client, const std::vector<std::string>& args);
    /** @brief Handle TOPIC command. @param server Server context. @param client Requesting client. @param args Command arguments. */
    static void cmdTopic(Server& server, Client& client, const std::vector<std::string>& args);
    /** @brief Handle NAMES command. @param server Server context. @param client Requesting client. @param args Command arguments. */
    static void cmdNames(Server& server, Client& client, const std::vector<std::string>& args);

    /** @brief Handle MODE command. @param server Server context. @param client Requesting client. @param args Command arguments. */
    static void cmdMode(Server& server, Client& client, const std::vector<std::string>& args);

    /** @brief Handle KICK command. @param server Server context. @param client Requesting client. @param args Command arguments. */
    static void cmdKick(Server& server, Client& client, const std::vector<std::string>& args);
    /** @brief Handle INVITE command. @param server Server context. @param client Requesting client. @param args Command arguments. */
    static void cmdInvite(Server& server, Client& client, const std::vector<std::string>& args);
    /** @brief Handle PRIVMSG command. @param server Server context. @param client Requesting client. @param args Command arguments. */
    static void cmdPrivmsg(Server& server, Client& client, const std::vector<std::string>& args);

};