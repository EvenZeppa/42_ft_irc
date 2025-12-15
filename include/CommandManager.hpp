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

    CommandEntry () : func(NULL), authRequired(false), minArgs(0) {}
    CommandEntry(CommandFunc f, bool auth, int minA)
        : func(f), authRequired(auth), minArgs(minA) {}

};

class CommandManager {
private:
    std::map<std::string, CommandEntry> _commands;
    std::string _buildReply(const Server& server, int code, std::string arg1, std::string arg2);


public:
    CommandManager();
    void executeCommand(Server& server, Client& client, std::string cmdName, std::vector<std::string> args);
    
    // cmd Auth
    static void cmdPass(Server& server, Client& client, const std::vector<std::string>& args);
    static void cmdNick(Server& server, Client& client, const std::vector<std::string>& args);
    static void cmdUser(Server& server, Client& client, const std::vector<std::string>& args);
    static void cmdCap(Server& server, Client& client, const std::vector<std::string>& args);
    static void cmdQuit(Server& server, Client& client, const std::vector<std::string>& args); 
    static void cmdWho(Server& server, Client& client, const std::vector<std::string>& args);
    static void cmdPing(Server& server, Client& client, const std::vector<std::string>& args);
    static void cmdPong(Server& server, Client& client, const std::vector<std::string>& args);

    // cmd Channel
    static void cmdJoin(Server& server, Client& client, const std::vector<std::string>& args);
    static void cmdPart(Server& server, Client& client, const std::vector<std::string>& args);
    static void cmdTopic(Server& server, Client& client, const std::vector<std::string>& args);
    static void cmdNames(Server& server, Client& client, const std::vector<std::string>& args);

    // cmd Mode
    static void cmdMode(Server& server, Client& client, const std::vector<std::string>& args);

    // cmd Ops
    static void cmdKick(Server& server, Client& client, const std::vector<std::string>& args);
    static void cmdInvite(Server& server, Client& client, const std::vector<std::string>& args);
    // cmd Privmsg
    static void cmdPrivmsg(Server& server, Client& client, const std::vector<std::string>& args);

};