/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ezeppa <ezeppa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 00:00:00 by ezeppa            #+#    #+#             */
/*   Updated: 2026/03/03 00:00:00 by vbonnard           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

#include "BNFParser.hpp"
#include "Grammar.hpp"
#include "DataExtractor.hpp"

#include <iostream>

/**
 * @brief Program entry point.
 * @param argc Number of command-line arguments.
 * @param argv Command-line argument values.
 * @return 0 on successful shutdown, 1 when arguments are invalid.
 */
int main(int argc, char** argv) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return 1;
	}
	
	Server server;
	server
	.host("127.0.0.1")
	.port(argv[1])
	.name("MyServer")
	.pass(argv[2]);

	
	std::cout << "IRC Project Initialisation" << std::endl;
	server.init();
    std::cout << "IRC Project Running" << std::endl << "Write 'help' for more information" << std::endl;
	server.run();
    std::cout << "IRC Project Stopped" << std::endl;
	server.close();
	
	return 0;
}