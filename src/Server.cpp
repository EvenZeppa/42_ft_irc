#include "../include/Server.hpp"

#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstdlib>
#include <cerrno>
#include <sstream>

#include "../include/Logger.hpp"

Server::Server() :
	_host("127.0.0.1"),
	_port("6667"),
	_pass(""),
	_name("emptyName"),
	_socketfd(-1),
	_epfd(-1),
	_running(true),
	_isClosed(false) {}

Server::Server(const Server& other) :
	_host(other._host),
	_port(other._port),
	_pass(other._pass),
	_name(other._name),
	_socketfd(-1),
	_epfd(-1),
	_running(true),
	_isClosed(false) {}

Server& Server::operator=(const Server& other) {
	_host = other._host;
	_port = other._port;
	_pass = other._pass;
	_name = other._name;
	return *this;
}

Server::~Server() {
	cleanupResources();
}

void Server::cleanupResources() {
	if (_isClosed) {
		return;
	}
	_isClosed = true;
	_running = false;

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
		_socketfd = -1;
	}
	if (_epfd != -1) {
		::close(_epfd);
		_epfd = -1;
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

// void initGrammar(Grammar& grammar) {
// 	grammar.addRule("<letter> ::= ( 'a' ... 'z' 'A' ... 'Z' )");
// 	grammar.addRule("<number> ::= ( '0' ... '9' )");
// 	grammar.addRule("<special> ::= '-' | '[' | ']' | '\\\\' | '`' | '^' | '{' | '}'");

// 	grammar.addRule("<nospace> ::= '!' | '\"' | '#' | '$' | '%' | '&' | ''' | '(' | ')' | '*' | '+' | ',' | '-' | '.' | '/' | '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | ':' | ';' | '<' | '=' | '>' | '?' | '@' | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' | 'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z' | '[' | '\' | ']' | '^' | '_' | '`' | 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' | 's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z' | '{' | '|' | '} | '~'");
// 	grammar.addRule("<safechar> ::= ' ' | '!' | '\"' | '#' | '$' | '%' | '&' | ''' | '(' | ')' | '*' | '+' | ',' | '-' | '.' | '/' | '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | ':' | ';' | '<' | '=' | '>' | '?' | '@' | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' | 'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z' | '[' | '\' | ']' | '^' | '_' | '`' | 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' | 's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z' | '{' | '|' | '}' | '~'");
// 	grammar.addRule("<nospecial> ::= '!' | '\"' | '#' | '$' | '%' | '&' | ''' | '(' | ')' | '*' | '+' | ',' | '-' | '.' | '/' | '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | ';' | '<' | '=' | '>' | '?' | '@' | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' | 'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z' | '[' | '\' | ']' | '^' | '_' | '`' | 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' | 's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z' | '{' | '|' | '}' | '~'");
// 	grammar.addRule("<nonwhite> ::= ( ^ 0x20 0x0 0xD 0xA )");

// 	grammar.addRule("<SPACE> ::= ' ' { ' ' }");
// 	grammar.addRule("<crlf> ::= '\r' '\n'");

// 	grammar.addRule("<middle> ::= <nospecial> { <nospace> }");
// 	grammar.addRule("<trailing> ::= { <safechar> }");

// 	grammar.addRule("<params> ::= <SPACE> [ ':' <trailing> | <middle> [ <params> ] ]");
// 	grammar.addRule("<command> ::= <letter> { <letter> } | <number> <number> <number>");

// 	grammar.addRule("<nick> ::= <letter> { <letter> | <number> | <special> }");
// 	grammar.addRule("<user> ::= <nonwhite> { <nonwhite> }");

// 	grammar.addRule("<hostname-char> ::= <letter> | <number> | '-'");
// 	grammar.addRule("<hostname-end> ::= <letter> | <number>");
// 	grammar.addRule("<servername> ::= <letter> { <hostname-char> } <hostname-end>");

// 	grammar.addRule("<prefix> ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]");
// 	grammar.addRule("<message> ::= [ ':' <prefix> <SPACE> ] <command> <params> <crlf>");
// }

void initGrammar(Grammar& grammar) {
    // Définitions de base
    grammar.addRule("<letter> ::= 'a'...'z' | 'A'...'Z'");
    grammar.addRule("<number> ::= '0'...'9'");
    grammar.addRule("<special> ::= '-' | '[' | ']' | '\\\\' | '`' | '^' | '{' | '}'");
    
    // Ajout des accents (Extended ASCII / ISO-8859-1)
    // 0xA0 à 0xFF couvre la majorité des caractères accentués et spéciaux latins
    grammar.addRule("<accented> ::= 0xA0...0xFF");

    // Nettoyage des règles complexes avec les plages
    // <safechar> : Tous les caractères imprimables de l'espace (32) au tilde (126) + accents
    grammar.addRule("<safechar> ::= ' '...'~' | <accented>");
    
    // <nospace> : Comme safechar mais sans l'espace ('!' est le caractère 33)
    grammar.addRule("<nospace> ::= '!'...'~' | <accented>");
    
    // <nospecial> : Comme nospace mais exclut souvent ':' pour le parsing des paramètres IRC
    // On définit donc les plages autour de ':' (qui est le code 58)
    grammar.addRule("<nospecial> ::= '!'...'9' | ';'...'~' | <accented>");

    // Caractères de contrôle et espaces
    grammar.addRule("<nonwhite> ::= 0x21...0xFF"); // Tout sauf espace et contrôles C0
    grammar.addRule("<SPACE> ::= ' ' { ' ' }");
    grammar.addRule("<crlf> ::= '\r' '\n'");

    // Structure des messages
    grammar.addRule("<middle> ::= <nospecial> { <nospace> }");
    grammar.addRule("<trailing> ::= { <safechar> }");

    // Paramètres et Commandes
    grammar.addRule("<params> ::= <SPACE> [ ':' <trailing> | <middle> [ <params> ] ]");
    grammar.addRule("<command> ::= <letter> { <letter> } | <number> <number> <number>");

    // Identités
    grammar.addRule("<nick> ::= <letter> { <letter> | <number> | <special> }");
    grammar.addRule("<user> ::= <nonwhite> { <nonwhite> }");

    // Réseau
    grammar.addRule("<hostname-char> ::= <letter> | <number> | '-'");
    grammar.addRule("<hostname-end> ::= <letter> | <number>");
    grammar.addRule("<servername> ::= <letter> { <hostname-char> } [ <hostname-end> ]");

    // Message complet
    grammar.addRule("<prefix> ::= <servername> | <nick> [ '!' <user> ] [ '@' <servername> ]");
    grammar.addRule("<message> ::= [ ':' <prefix> <SPACE> ] <command> <params> <crlf>");
}

int Server::init() {
	_socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketfd == -1) {
		Logger::log(Logger::ERROR, -1, "Socket init failed");
		return -1;
	}

	struct addrinfo sin, *res, *p;

	memset(&sin, 0, sizeof(sin));
	sin.ai_family = AF_INET;
	sin.ai_socktype = SOCK_STREAM;
	sin.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, _port.c_str(), &sin, &res)) {
		Logger::log(Logger::ERROR, -1, "getaddrinfo failed");
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
		Logger::log(Logger::ERROR, -1, "Socket listen failed");
		return -1;
	}

	_epfd = epoll_create1(0);
	if (_epfd == -1) {
		Logger::log(Logger::ERROR, -1, "epoll_create1 failed");
		return -1;
	}

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = STDIN_FILENO;

	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {
		Logger::log(Logger::ERROR, -1, "epoll_ctl add STDIN failed");
		return -1;
	}

	ev.data.fd = _socketfd;
	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, _socketfd, &ev)) {
		Logger::log(Logger::ERROR, -1, "epoll_ctl add server socket failed");
		return -1;
	}

	initGrammar(_grammar);

	return 0;
}

void Server::run() {
	while (_running) {
		struct epoll_event rev[1028];
		int num_events = epoll_wait(_epfd, rev, 1028, -1);
		if (num_events < 0) {
			if (_running) {
				Logger::log(Logger::ERROR, -1, "epoll_wait failed");
			}
			break;
		}
		if (num_events > 0)
			for (int i = 0; i < num_events; ++i) {
				handle_event(rev[i]);
			}
	}
}

void Server::close()
{
	cleanupResources();
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
			newClient->hostname(_host);
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
		Logger::log(Logger::OUT, client.fd(), writeBuffer, client.nickname());
		ssize_t bytesSent = send(client.fd(), writeBuffer.c_str(), writeBuffer.size(), 0);
		if (bytesSent > 0) {
			std::ostringstream oss;
			oss << "Sent " << bytesSent << " bytes";
			Logger::log(Logger::INFO, client.fd(), oss.str(), client.nickname());
			writeBuffer.erase(0, bytesSent);
		} else if (bytesSent == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
			Logger::log(Logger::ERROR, client.fd(), "send() failed", client.nickname());
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
		Logger::log(Logger::IN, client.fd(), std::string(buffer), client.nickname());

		// Normalize line endings: convert LF to CRLF if not already present
		std::string& readBuf = client.readBuffer();
		for (size_t i = 0; i < readBuf.length(); i++) {
			if (readBuf[i] == '\n' && (i == 0 || readBuf[i-1] != '\r')) {
				readBuf.insert(i, 1, '\r');
				i++; // Skip the \n we just processed
			}
		}

		// Parse all complete commands in buffer
		while (!client.readBuffer().empty()) {
			BNFParser parser(_grammar);

			size_t consumed = 0;
			ASTNode* ast = parser.parse("<message>", client.readBuffer(), consumed);
			if (ast) {
				DataExtractor extractor;
				ExtractedData data = extractor.extract(ast);

				if (data.has("<command>")) {
					std::string command;
					std::vector<std::string> middles;
					std::string trailing;

					command = data.first("<command>");
					if (data.has("<middle>"))
						middles = data.all("<middle>");
					if (data.has("<trailing>")) {
						trailing = data.first("<trailing>");
						middles.push_back(trailing);
					}

					std::string parseMsg = "Command: " + command;
					if (!middles.empty()) {
						parseMsg += " Args:";
						for (std::vector<std::string>::iterator it = middles.begin(); it != middles.end(); ++it) {
							parseMsg += " [" + *it + "]";
						}
					}
					Logger::log(Logger::INFO, client.fd(), parseMsg, client.nickname());

					CommandManager cm;
					cm.executeCommand(*this, client, command, middles);

					client.subToReadBuffer(consumed);
					
					delete ast;
				} else {
					Logger::log(Logger::ERROR, client.fd(), "Parse failed (no command)", client.nickname());
					delete ast;
					break;
				}
			} else {
				// No complete message yet, wait for more data
				if (!client.readBuffer().empty()) {
					Logger::log(Logger::INFO, client.fd(), "Incomplete message in buffer, waiting for more data", client.nickname());
				}
				break;
			}
		}

		if (!client.readBuffer().empty()) {
			Logger::log(Logger::INFO, client.fd(), "Buffer remaining: " + client.readBuffer(), client.nickname());
		}
	} else if (bytesRead == 0) {
		Logger::log(Logger::INFO, client.fd(), "Client disconnected", client.nickname());
		client.fd(-1);
	} else if (bytesRead == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
		Logger::log(Logger::ERROR, client.fd(), "recv() failed", client.nickname());
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
