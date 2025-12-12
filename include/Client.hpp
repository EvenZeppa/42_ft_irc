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

	Client(const Client& other);
	Client& operator=(const Client& other);
public:
	Client();
	Client(int fd);
	~Client();

	// Getters
	std::string realname() const;
	std::string username() const;
	std::string nickname() const;
	std::string hostname() const;
	std::string fullmask() const;
	int fd() const;
	int epollServerFd() const;
	bool isPassReceived() const;
	bool isNickReceived() const;
	bool isUserReceived() const;
	bool isRegistered() const;
	time_t lastPing() const;
	time_t lastPong() const;

	// Setters
	Client& realname(const std::string& realname);
	Client& username(const std::string& username);
	Client& nickname(const std::string& nickname);
	Client& hostname(const std::string& hostname);
	Client& fd(int fd);
	Client& epollServerFd(int epsfd);
	Client& passReceived(bool status);
	Client& nickReceived(bool status);
	Client& userReceived(bool status);
	Client& registered(bool status);
	Client& lastPing(time_t timestamp);
	Client& lastPong(time_t timestamp);

	// Mode operations
	void addMode(char mode);
	void removeMode(char mode);
	bool hasMode(char mode) const;

	// Buffer operations
	std::string& readBuffer();
	const std::string& readBuffer() const;
	void appendToReadBuffer(const std::string& data);
	void clearReadBuffer();

	std::string& writeBuffer();
	const std::string& writeBuffer() const;
	void appendToWriteBuffer(const std::string& data);
	void clearWriteBuffer();

	// Channels operations
	void joinChannel(const std::string& channelName);
	void leaveChannel(const std::string& channelName);
	bool isInChannel(const std::string& channelName) const;

	friend std::ostream& operator<<(std::ostream& os, const Client& client);
};