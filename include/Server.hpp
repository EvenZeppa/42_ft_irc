/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ezeppa <ezeppa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 00:00:00 by ezeppa            #+#    #+#             */
/*   Updated: 2026/03/03 00:00:00 by vbonnard           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>
#include <map>

#include "Client.hpp"
#include "Channel.hpp"

#include "CommandManager.hpp"

#include "Grammar.hpp"
#include "BNFParser.hpp"
#include "DataExtractor.hpp"

class Server {
private:
	std::string _host;
	std::string _port;
	std::string _pass;

	std::string _name;

	std::map<int, Client*> _clients;
	std::map<std::string, Channel*> _channels;

	int _socketfd;
	int _epfd;
	bool _running;
	bool _isClosed;

	Grammar _grammar;

	/**
	 * @brief Release all allocated resources and close descriptors.
	 */
	void cleanupResources();
public:
	/** @brief Construct a server with default values. */
	Server();
	/** @brief Construct a server by copying settings from another instance. @param other Source server. */
	Server(const Server& other);
	/** @brief Assign server settings. @param other Source server. @return Reference to this server. */
	Server& operator=(const Server& other);
	/** @brief Destroy the server and clean resources. */
	~Server();

	/** @brief Get bind host. @return Host string. */
	std::string host() const;
	/** @brief Get bind port. @return Port string. */
	std::string port() const;
	/** @brief Get server password. @return Password string. */
	std::string pass() const;
	/** @brief Get server name. @return Server name. */
	std::string name() const;

	/** @brief Set bind host. @param host Host string. @return Reference to this server. */
	Server& host(const std::string& host);
	/** @brief Set bind port. @param port Port string. @return Reference to this server. */
	Server& port(const std::string& port);
	/** @brief Set server password. @param pass Password string. @return Reference to this server. */
	Server& pass(const std::string& pass);
	/** @brief Set server name. @param name Server name. @return Reference to this server. */
	Server& name(const std::string& name);

	/** @brief Register a new client. @param client Client pointer to store. @return True when inserted. */
	bool addClient(Client* client);
	/** @brief Remove and delete a client by file descriptor. @param fd Client descriptor. @return True when removed. */
	bool removeClient(int fd);
	/** @brief Get mutable client by descriptor. @param fd Client descriptor. @return Client pointer or NULL. */
	Client* getClient(int fd);
	/** @brief Get const client by descriptor. @param fd Client descriptor. @return Client pointer or NULL. */
	const Client* getClient(int fd) const;
	/** @brief Check client existence. @param fd Client descriptor. @return True if present. */
	bool hasClient(int fd) const;
	/** @brief Get number of connected clients. @return Client count. */
	size_t clientCount() const;
	/** @brief Access mutable client map. @return Reference to client map. */
	std::map<int, Client*>& clients();
	/** @brief Access const client map. @return Reference to client map. */
	std::map<int, Client*>& clients() const;

	/** @brief Register a new channel. @param channel Channel pointer to store. @return True when inserted. */
	bool addChannel(Channel* channel);
	/** @brief Remove a channel by name. @param name Channel name. @return True when removed. */
	bool removeChannel(const std::string& name);
	/** @brief Get mutable channel by name. @param name Channel name. @return Channel pointer or NULL. */
	Channel* getChannel(const std::string& name);
	/** @brief Get const channel by name. @param name Channel name. @return Channel pointer or NULL. */
	const Channel* getChannel(const std::string& name) const;
	/** @brief Check channel existence. @param name Channel name. @return True if present. */
	bool hasChannel(const std::string& name) const;
	/** @brief Get number of channels. @return Channel count. */
	size_t channelCount() const;
	/** @brief Access mutable channel map. @return Reference to channel map. */
	std::map<std::string, Channel*>& channels();
	/** @brief Access const channel map. @return Reference to channel map. */
	std::map<std::string, Channel*>& channels() const;

	/** @brief Initialize sockets, epoll, and parser grammar. @return 0 on success, -1 on failure. */
	int init();
	/** @brief Start the main event loop until stopped. */
	void run();
	/** @brief Stop the server and release resources. */
	void close();
	/** @brief Handle a single epoll event. @param ev Event payload. @return 0 on success, negative on failure. */
	int handle_event(struct epoll_event ev);

	/** @brief Flush pending output to a client socket. @param client Target client. */
	void handleClientWrite(Client& client);
	/** @brief Read and parse incoming data from a client socket. @param client Target client. */
	void handleClientRead(Client& client);

	/** @brief Stream server debug information. @param os Output stream. @param server Server to serialize. @return Output stream reference. */
	friend std::ostream& operator<<(std::ostream& os, const Server& server);
};