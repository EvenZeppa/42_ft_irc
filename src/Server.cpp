#include "../include/Server.hpp"

#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstdlib>
#include <cerrno>

Server::Server() :
	_host("127.0.0.1"),
	_port("6667"),
	_pass(""),
	_name("emptyName") {}

Server::Server(const Server& other) :
	_host(other._host),
	_port(other._port),
	_pass(other._pass),
	_name(other._name) {}

Server& Server::operator=(const Server& other) {
	_host = other._host;
	_port = other._port;
	_pass = other._pass;
	_name = other._name;
	return *this;
}

Server::~Server() {
	std::map<int, Client*>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it) {
		delete it->second;
	}
	_clients.clear();

	std::map<std::string, Channel*>::iterator chanIt;
	for (chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt) {
		delete chanIt->second;
	}
	_channels.clear();

	if (_socketfd != -1) {
		::close(_socketfd);
	}
	if (_epfd != -1) {
		::close(_epfd);
	}
}

// Getters
std::string Server::host() const { return _host; }
std::string Server::port() const { return _port; }
std::string Server::pass() const { return _pass; }
std::string Server::name() const { return _name; }

// Setters
Server& Server::host(const std::string& host) { _host = host; return *this; }
Server& Server::port(const std::string& port) { _port = port; return *this; }
Server& Server::pass(const std::string& pass) { _pass = pass; return *this; }
Server& Server::name(const std::string& name) { _name = name; return *this; }

// Clients operations
bool Server::addClient(Client* client) {
	int fd = client->fd();
	if (hasClient(fd))
		return false;
	_clients[fd] = client;
	return true;
}

bool Server::removeClient(int fd) {
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return false;

	delete it->second;
	_clients.erase(it);

	return true;
}

Client* Server::getClient(int fd) {
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it != _clients.end())
		return it->second;
	return NULL;
}

const Client* Server::getClient(int fd) const {
	std::map<int, Client*>::const_iterator it = _clients.find(fd);
	if (it != _clients.end())
		return it->second;
	return NULL;
}

bool Server::hasClient(int fd) const {
	return _clients.find(fd) != _clients.end();
}

size_t Server::clientCount() const {
	return _clients.size();
}

std::map<int, Client*>& Server::clients() {
	return _clients;
}

std::map<int, Client*>& Server::clients() const {
	return const_cast<std::map<int, Client*>&>(_clients);
}

// Channels operations
bool Server::addChannel(Channel* channel) {
	std::string name = channel->name();
	if (hasChannel(name))
		return false;
	_channels[name] = channel;
	return true;
}

bool Server::removeChannel(const std::string& name) {
	return _channels.erase(name) > 0;
}

Channel* Server::getChannel(const std::string& name) {
	std::map<std::string, Channel*>::iterator it = _channels.find(name);
	if (it != _channels.end())
		return it->second;
	return NULL;
}

const Channel* Server::getChannel(const std::string& name) const {
	std::map<std::string, Channel*>::const_iterator it = _channels.find(name);
	if (it != _channels.end())
		return it->second;
	return NULL;
}

bool Server::hasChannel(const std::string& name) const {
	return _channels.find(name) != _channels.end();
}

size_t Server::channelCount() const {
	return _channels.size();
}

std::map<std::string, Channel*>& Server::channels() {
	return _channels;
}

std::map<std::string, Channel*>& Server::channels() const {
	return const_cast<std::map<std::string, Channel*>&>(_channels);
}

int Server::init() {
	_socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketfd == -1) {
		std::cerr << "Error: Socket init" << std::endl;
		return -1;
	}

	struct addrinfo sin, *res, *p;

	memset(&sin, 0, sizeof(sin));
	sin.ai_family = AF_INET;
	sin.ai_socktype = SOCK_STREAM;
	sin.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, _port.c_str(), &sin, &res)) {
		std::cerr << "getaddrinfo error" << std::endl;
		return -1;
	}

	fcntl(_socketfd, F_SETFL, O_NONBLOCK);

	for (p = res; p != NULL; p = p->ai_next) {
		_socketfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (_socketfd < 0) continue;

		int opt = 1;
		setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		if (bind(_socketfd, p->ai_addr, p->ai_addrlen) == 0) {
			break;
		}

		::close(_socketfd);
		_socketfd = -1;
	}

	freeaddrinfo(res);

	if (listen(_socketfd, 5) == -1) {
		std::cerr << "Error: Socket listen" << std::endl;
		return -1;
	}

	_epfd = epoll_create1(0);
	if (_epfd == -1) {
		std::cerr << "Error: Epoll create" << std::endl;
		return -1;
	}

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = STDIN_FILENO;

	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {
		std::cerr << "Error: Epoll ctl for quit" << std::endl;
		return -1;
	}

	ev.data.fd = _socketfd;
	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, _socketfd, &ev)) {
		std::cerr << "Error: Epoll ctl for listen server" << std::endl;
		return -1;
	}
	return 0;
}

void Server::run() {
	while(true) {
		struct epoll_event rev[1028];
		int num_events = epoll_wait(_epfd, rev, 1028, -1);
		if (num_events > 0)
			for (int i = 0; i < num_events; ++i) {
				handle_event(rev[i]);
			}
	}
}

void Server::close()
{
	std::map<int, Client*>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it) {
		delete it->second;
	}
	_clients.clear();

	if (_socketfd != -1) ::close(_socketfd);
	if (_epfd != -1) ::close(_epfd);

	::exit(0);
}

int Server::handle_event(struct epoll_event ev) {
	int fd = ev.data.fd;

	if (fd == _socketfd) {
		if (ev.events & EPOLLIN) {
			struct sockaddr_in peer_addr;
			socklen_t peer_addr_size = sizeof(peer_addr);

			int cfd = accept(fd, (struct sockaddr *)&peer_addr, &peer_addr_size);

			// @TODO : Verifier si on peut l'utiliser pour le client
			fcntl(cfd, F_SETFL, O_NONBLOCK);

			struct epoll_event nev;
			nev.events = EPOLLIN;
			nev.data.fd = cfd;
			epoll_ctl(_epfd, EPOLL_CTL_ADD, cfd, &nev);

			Client* newClient = new Client(cfd);
			newClient->epollServerFd(_epfd);
			if (!addClient(newClient)) {
				delete newClient;
				return -1;
			}
		}
	} else if (fd == STDIN_FILENO) {
		std::string line;
		std::getline(std::cin, line);
		if (line == "quit") close();
	} else {
		std::map<int, Client*>::iterator it = _clients.find(fd);
		if (it != _clients.end()) {
			Client& c = *(it->second);
			if (ev.events & EPOLLOUT) {
				handleClientWrite(c);
				if (c.fd() == -1) {
					removeClient(fd);
					return 0;
				}
			}
			if (ev.events & EPOLLIN) {
				handleClientRead(c);
				if (c.fd() == -1) {
					removeClient(fd);
					return 0;
				}
			}
		}
	}
	return 0;
}

void Server::handleClientWrite(Client& client) {
	std::string& writeBuffer = client.writeBuffer();
	if (!writeBuffer.empty()) {
		ssize_t bytesSent = send(client.fd(), writeBuffer.c_str(), writeBuffer.size(), 0);
		if (bytesSent > 0) {
			writeBuffer.erase(0, bytesSent);
		} else if (bytesSent == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
			std::cerr << "Error: Sending data to client fd " << client.fd() << std::endl;
			client.fd(-1);
		}
	}
}

void Server::handleClientRead(Client& client) {
	char buffer[4096];
	ssize_t bytesRead = recv(client.fd(), buffer, sizeof(buffer) - 1, 0);
	if (bytesRead > 0) {
		buffer[bytesRead] = '\0';
		client.appendToReadBuffer(std::string(buffer));
		// Parse commands
		std::cout << client.readBuffer() << std::endl;
	} else if (bytesRead == 0) {
		std::cout << "Client fd " << client.fd() << " disconnected." << std::endl;
		client.fd(-1);
	} else if (bytesRead == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
		std::cerr << "Error: Receiving data from client fd " << client.fd() << std::endl;
		client.fd(-1);
	}
}

std::ostream& operator<<(std::ostream& os, const Server& server) {
	os  << "Server: " << server._name
		<< ", Host: " << server._host
		<< ", Port: " << server._port
		<< ", Pass: " << server._pass;
	return os;
}
