/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ezeppa <ezeppa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+            */
/*   Created: 2026/03/17 00:00:00 by ezeppa            #+#    #+#              */
/*   Updated: 2026/03/17 00:00:00 by vbonnard           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <poll.h>

static const char* MESSAGES[] = {
	"*beep boop* Je suis un bot. La vie est belle.",
	"42 est la reponse. Mais a quelle question ? Mystere.",
	"Le serveur tourne. Moi aussi. Depuis 10 secondes.",
	"Ping ! Non attends, c'est moi qui parle. Pong ?",
	"Quelqu'un a du cafe ? Les bots ont soif de bytes.",
	"*bruit de robot* Bip. Bop. Message inutile envoye.",
	"Si tu lis ca, le bot fonctionne. Felicitations.",
	"Je pourrais calculer Pi. Mais j'envoie des messages a la place.",
	"01001000 01101001 = Hi en binaire. Tu es le bienvenu.",
	"Le chat IRC, c'est comme le courrier, mais plus rapide.",
	"*ronronnement mecanique* Tout va bien par ici.",
	"Question existentielle : les bots revent-ils de moutons electriques ?",
	"10 secondes se sont ecoulees. Le temps passe. Le bot aussi.",
	"Si ce message t'ennuie, imagine l'avoir ecrit toi-meme.",
	"Bot power: ON. Creativity: OFF. Messages: RANDOM.",
};

static const size_t NUM_MESSAGES = sizeof(MESSAGES) / sizeof(MESSAGES[0]);

static int connect_to_server(const std::string& host, const std::string& port) {
	struct addrinfo hints;
	struct addrinfo* res;
	int sock = -1;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0) {
		std::cerr << "bot: getaddrinfo failed" << std::endl;
		return -1;
	}

	for (struct addrinfo* p = res; p != NULL; p = p->ai_next) {
		sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sock < 0) continue;
		if (connect(sock, p->ai_addr, p->ai_addrlen) == 0) break;
		close(sock);
		sock = -1;
	}

	freeaddrinfo(res);
	return sock;
}

static void send_line(int sock, const std::string& line) {
	std::string msg = line + "\r\n";
	send(sock, msg.c_str(), msg.size(), 0);
}

static void handle_input(int sock, std::string& buffer) {
	char buf[512];
	ssize_t n = recv(sock, buf, sizeof(buf) - 1, 0);
	if (n <= 0) return;

	buf[n] = '\0';
	buffer += buf;

	size_t pos;
	while ((pos = buffer.find("\r\n")) != std::string::npos) {
		std::string line = buffer.substr(0, pos);
		buffer.erase(0, pos + 2);

		if (line.find("PING ") == 0) {
			std::string token = line.substr(5);
			send_line(sock, "PONG " + token);
		}
	}
}

int main(int argc, char** argv) {
	if (argc < 4) {
		std::cerr << "Usage: " << argv[0] << " <host> <port> <password> [channel]" << std::endl;
		std::cerr << "  channel defaults to #bot" << std::endl;
		return 1;
	}

	std::string host = argv[1];
	std::string port = argv[2];
	std::string password = argv[3];
	std::string channel = (argc >= 5) ? argv[4] : "#bot";

	if (channel[0] != '#') channel = "#" + channel;

	int sock = connect_to_server(host, port);
	if (sock < 0) {
		std::cerr << "bot: could not connect to " << host << ":" << port << std::endl;
		return 1;
	}

	send_line(sock, "PASS " + password);
	send_line(sock, "NICK bot");
	send_line(sock, "USER bot 0 * :ft_irc bonus bot");
	sleep(1);
	send_line(sock, "JOIN " + channel);

	std::cout << "Bot connected to " << host << ":" << port << ", joined " << channel << std::endl;
	std::cout << "Sending a message every 10 seconds. Ctrl+C to quit." << std::endl;

	std::srand(static_cast<unsigned int>(std::time(NULL)));
	std::string read_buffer;
	time_t last_msg = std::time(NULL);

	while (true) {
		struct pollfd pfd;
		pfd.fd = sock;
		pfd.events = POLLIN;

		int timeout = 1000;
		time_t now = std::time(NULL);
		if (now - last_msg >= 10) {
			timeout = 0;
		}

		int ret = poll(&pfd, 1, timeout);

		if (ret > 0 && (pfd.revents & POLLIN)) {
			handle_input(sock, read_buffer);
		}

		now = std::time(NULL);
		if (now - last_msg >= 10) {
			const char* msg = MESSAGES[std::rand() % NUM_MESSAGES];
			send_line(sock, "PRIVMSG " + channel + " :" + msg);
			std::cout << "[bot] " << msg << std::endl;
			last_msg = now;
		}
	}

	close(sock);
	return 0;
}
