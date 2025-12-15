// PASS, NICK, USER, CAP, QUIT, WHO, PING, PONG

#include "CommandManager.hpp"
#include "Server.hpp"
#include "network/IrcReplies.hpp"

void CommandManager::cmdPass(Server& server, Client& client, const std::vector<std::string>& args) {
    // Implementation of PASS command
}

void CommandManager::cmdNick(Server& server, Client& client, const std::vector<std::string>& args) {
    // Implementation of NICK command
}

void CommandManager::cmdUser(Server& server, Client& client, const std::vector<std::string>& args) {
    // Implementation of USER command
}

void CommandManager::cmdCap(Server& server, Client& client, const std::vector<std::string>& args) {
    // Implementation of CAP command
}

void CommandManager::cmdQuit(Server& server, Client& client, const std::vector<std::string>& args) {
    // Implementation of QUIT command
}

void CommandManager::cmdWho(Server& server, Client& client, const std::vector<std::string>& args) {
    // Implementation of WHO command
}

void CommandManager::cmdPing(Server& server, Client& client, const std::vector<std::string>& args) {
    // Implementation of PING command
}

void CommandManager::cmdPong(Server& server, Client& client, const std::vector<std::string>& args) {
    // Implementation of PONG command
}