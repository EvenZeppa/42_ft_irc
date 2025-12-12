#include "../include/Client.hpp"
#include <unistd.h>

Client::Client() :
	_realname(""),
	_username(""),
	_nickname(""),
	_hostname(""),
	_fd(-1),
	_epollServerFd(-1),
	_isPassReceived(false),
	_isNickReceived(false),
	_isUserReceived(false),
	_isRegistered(false),
	_lastPing(0),
	_lastPong(0),
	_modes(),
	_readBuffer(""),
	_writeBuffer(""),
	_channels()
{}

Client::Client(int fd) :
	_realname(""),
	_username(""),
	_nickname(""),
	_hostname(""),
	_fd(fd),
	_epollServerFd(-1),
	_isPassReceived(false),
	_isNickReceived(false),
	_isUserReceived(false),
	_isRegistered(false),
	_lastPing(0),
	_lastPong(0),
	_modes(),
	_readBuffer(""),
	_writeBuffer(""),
	_channels()
{}

Client::~Client() {
	if (_fd != -1)
		::close(_fd);
}

// Getters
std::string Client::realname() const { return _realname; }
std::string Client::username() const { return _username; }
std::string Client::nickname() const { return _nickname; }
std::string Client::hostname() const { return _hostname; }
std::string Client::fullmask() const { return _nickname + "!" + _username + "@" + _hostname; }
int Client::fd() const { return _fd; }
int Client::epollServerFd() const { return _epollServerFd; }
bool Client::isPassReceived() const { return _isPassReceived; }
bool Client::isNickReceived() const { return _isNickReceived; }
bool Client::isUserReceived() const { return _isUserReceived; }
bool Client::isRegistered() const { return _isRegistered; }
time_t Client::lastPing() const { return _lastPing; }
time_t Client::lastPong() const { return _lastPong; }

// Setters
Client& Client::realname(const std::string& realname) { _realname = realname; return *this; }
Client& Client::username(const std::string& username) { _username = username; return *this; }
Client& Client::nickname(const std::string& nickname) { _nickname = nickname; return *this; }
Client& Client::hostname(const std::string& hostname) { _hostname = hostname; return *this; }
Client& Client::fd(int fd) { _fd = fd; return *this; }
Client& Client::epollServerFd(int epsfd) { _epollServerFd = epsfd; return *this; }
Client& Client::passReceived(bool status) { _isPassReceived = status; return *this; }
Client& Client::nickReceived(bool status) { _isNickReceived = status; return *this; }
Client& Client::userReceived(bool status) { _isUserReceived = status; return *this; }
Client& Client::registered(bool status) { _isRegistered = status; return *this; }
Client& Client::lastPing(time_t timestamp) { _lastPing = timestamp; return *this; }
Client& Client::lastPong(time_t timestamp) { _lastPong = timestamp; return *this; }

// Mode operations
void Client::addMode(char mode) { _modes.insert(mode); }
void Client::removeMode(char mode) { _modes.erase(mode); }
bool Client::hasMode(char mode) const {
	return _modes.find(mode) != _modes.end();
}

// Buffer operations
std::string& Client::readBuffer() { return _readBuffer; }
const std::string& Client::readBuffer() const { return _readBuffer; }
void Client::appendToReadBuffer(const std::string& data) { _readBuffer += data; }
void Client::clearReadBuffer() { _readBuffer.clear(); }

std::string& Client::writeBuffer() { return _writeBuffer; }
const std::string& Client::writeBuffer() const { return _writeBuffer; }
void Client::appendToWriteBuffer(const std::string& data) { _writeBuffer += data; }
void Client::clearWriteBuffer() { _writeBuffer.clear(); }

// Channels operations
void Client::joinChannel(const std::string& channelName) { _channels.insert(channelName); }
void Client::leaveChannel(const std::string& channelName) { _channels.erase(channelName); }
bool Client::isInChannel(const std::string& channelName) const {
	return _channels.find(channelName) != _channels.end();
}

std::ostream& operator<<(std::ostream& os, const Client& client) {
	os  << "Client: " << client._nickname
		<< ", Username: " << client._username
		<< ", Realname: " << client._realname
		<< ", Hostname: " << client._hostname
		<< ", FD: " << client._fd
		<< ", Registered: " << (client._isRegistered ? "Yes" : "No")
		<< ", Modes: ";
	for (std::set<char>::const_iterator it = client._modes.begin(); it != client._modes.end(); ++it) {
		os << *it;
	}
	return os;
}
