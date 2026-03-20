/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ezeppa <ezeppa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 14:30:00 by ezeppa            #+#    #+#             */
/*   Updated: 2026/03/03 15:43:19 by vbonnard           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>

#include <set>

class Client {
private:
	std::string _realname;
	std::string _username;
	std::string _nickname;
	std::string _hostname;
	int _fd;
	int _epollServerFd;

	bool _isPassReceived;
	bool _isNickReceived;
	bool _isUserReceived;
	bool _isRegistered;

	time_t _lastPing;
	time_t _lastPong;

	std::set<char> _modes;

	std::string _readBuffer;
	std::string _writeBuffer;

	std::set<std::string> _channels;

	/**
	 * @brief Disallow copy construction.
	 */
	Client(const Client& other);
	/**
	 * @brief Disallow copy assignment.
	 * @return Reference to this instance.
	 */
	Client& operator=(const Client& other);
public:
	/**
	 * @brief Construct a disconnected client.
	 */
	Client();
	/**
	 * @brief Construct a client bound to a file descriptor.
	 * @param fd Client socket file descriptor.
	 */
	Client(int fd);
	/**
	 * @brief Destroy the client and close its file descriptor if valid.
	 */
	~Client();

	/** @brief Get the real name. @return The current real name. */
	std::string realname() const;
	/** @brief Get the username. @return The current username. */
	std::string username() const;
	/** @brief Get the nickname. @return The current nickname. */
	std::string nickname() const;
	/** @brief Get the hostname. @return The current hostname. */
	std::string hostname() const;
	/** @brief Build IRC full mask. @return Nick!user@host mask. */
	std::string fullmask() const;
	/** @brief Get client socket descriptor. @return File descriptor. */
	int fd() const;
	/** @brief Get server epoll descriptor. @return Epoll file descriptor. */
	int epollServerFd() const;
	/** @brief Check PASS receipt. @return True if PASS was received. */
	bool isPassReceived() const;
	/** @brief Check NICK receipt. @return True if NICK was received. */
	bool isNickReceived() const;
	/** @brief Check USER receipt. @return True if USER was received. */
	bool isUserReceived() const;
	/** @brief Check registration status. @return True if registered. */
	bool isRegistered() const;
	/** @brief Get last PING timestamp. @return Unix time value. */
	time_t lastPing() const;
	/** @brief Get last PONG timestamp. @return Unix time value. */
	time_t lastPong() const;

	/** @brief Set real name. @param realname New real name. @return Reference to this client. */
	Client& realname(const std::string& realname);
	/** @brief Set username. @param username New username. @return Reference to this client. */
	Client& username(const std::string& username);
	/** @brief Set nickname. @param nickname New nickname. @return Reference to this client. */
	Client& nickname(const std::string& nickname);
	/** @brief Set hostname. @param hostname New hostname. @return Reference to this client. */
	Client& hostname(const std::string& hostname);
	/** @brief Set socket descriptor. @param fd New file descriptor. @return Reference to this client. */
	Client& fd(int fd);
	/** @brief Set epoll descriptor. @param epsfd New epoll descriptor. @return Reference to this client. */
	Client& epollServerFd(int epsfd);
	/** @brief Mark PASS receipt state. @param status New state. @return Reference to this client. */
	Client& passReceived(bool status);
	/** @brief Mark NICK receipt state. @param status New state. @return Reference to this client. */
	Client& nickReceived(bool status);
	/** @brief Mark USER receipt state. @param status New state. @return Reference to this client. */
	Client& userReceived(bool status);
	/** @brief Mark registration state. @param status New state. @return Reference to this client. */
	Client& registered(bool status);
	/** @brief Set last PING timestamp. @param timestamp Unix time. @return Reference to this client. */
	Client& lastPing(time_t timestamp);
	/** @brief Set last PONG timestamp. @param timestamp Unix time. @return Reference to this client. */
	Client& lastPong(time_t timestamp);

	/** @brief Add a user mode. @param mode Mode flag to insert. */
	void addMode(char mode);
	/** @brief Remove a user mode. @param mode Mode flag to erase. */
	void removeMode(char mode);
	/** @brief Check if a mode is enabled. @param mode Mode flag. @return True if present. */
	bool hasMode(char mode) const;

	/** @brief Access incoming buffer. @return Mutable read buffer reference. */
	std::string& readBuffer();
	/** @brief Access incoming buffer. @return Const read buffer reference. */
	const std::string& readBuffer() const;
	/** @brief Append data to incoming buffer. @param data Raw input bytes. */
	void appendToReadBuffer(const std::string& data);
	/** @brief Remove consumed bytes from incoming buffer. @param n Number of bytes to remove from start. */
	void subToReadBuffer(size_t n);
	/** @brief Clear incoming buffer. */
	void clearReadBuffer();

	/** @brief Access outgoing buffer. @return Mutable write buffer reference. */
	std::string& writeBuffer();
	/** @brief Access outgoing buffer. @return Const write buffer reference. */
	const std::string& writeBuffer() const;
	/** @brief Append data to outgoing buffer. @param data Serialized IRC data. */
	void appendToWriteBuffer(const std::string& data);
	/** @brief Clear outgoing buffer. */
	void clearWriteBuffer();

	/** @brief Record channel membership. @param channelName Target channel name. */
	void joinChannel(const std::string& channelName);
	/** @brief Remove channel membership. @param channelName Target channel name. */
	void leaveChannel(const std::string& channelName);
	/** @brief Check channel membership. @param channelName Target channel name. @return True if the client is in channel. */
	bool isInChannel(const std::string& channelName) const;

	/** @brief Stream client debug information. @param os Output stream. @param client Client to serialize. @return Output stream reference. */
	friend std::ostream& operator<<(std::ostream& os, const Client& client);
};