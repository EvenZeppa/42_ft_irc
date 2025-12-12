#pragma once

#include <string>
#include <iostream>
#include <map>

#include "Client.hpp"
#include "Channel.hpp"

class Server {
private:
	// Connection
	std::string _host;
	std::string _port;
	std::string _pass;

	// Identify
	std::string _name;

	// Clients and Channels
	std::map<int, Client*> _clients;
	std::map<std::string, Channel*> _channels;

	int _socketfd;
	int _epfd;
public:
	Server();
	Server(const Server& other);
	Server& operator=(const Server& other);
	~Server();

	// Getters
	std::string host() const;
	std::string port() const;
	std::string pass() const;
	std::string name() const;

	// Setters
	Server& host(const std::string& host);
	Server& port(const std::string& port);
	Server& pass(const std::string& pass);
	Server& name(const std::string& name);

	// Clients CRUD
	bool addClient(Client* client);
	bool removeClient(int fd);
	Client* getClient(int fd);
	const Client* getClient(int fd) const;
	bool hasClient(int fd) const;
	size_t clientCount() const;
	std::map<int, Client*>& clients();
	std::map<int, Client*>& clients() const;

	// Channels CRUD
	bool addChannel(Channel* channel);
	bool removeChannel(const std::string& name);
	Channel* getChannel(const std::string& name);
	const Channel* getChannel(const std::string& name) const;
	bool hasChannel(const std::string& name) const;
	size_t channelCount() const;
	std::map<std::string, Channel*>& channels();
	std::map<std::string, Channel*>& channels() const;

	int init();
	void run();
	void close();
	int handle_event(struct epoll_event ev);

	void handleClientWrite(Client& client);
	void handleClientRead(Client& client);

	friend std::ostream& operator<<(std::ostream& os, const Server& server);
};