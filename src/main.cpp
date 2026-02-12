#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"

#include "BNFParser.hpp"
#include "Grammar.hpp"
#include "DataExtractor.hpp"

#include <iostream>


void initGrammar(Grammar& grammar) {
    grammar.addRule("<letter> ::= ( 'a' ... 'z' 'A' ... 'Z' )");
    grammar.addRule("<number> ::= ( '0' ... '9' )");
    grammar.addRule("<special> ::= '-' | '[' | ']' | '\\\\' | '`' | '^' | '{' | '}'");

    grammar.addRule("<nospace> ::= '!' | '\"' | '#' | '$' | '%' | '&' | ''' | '(' | ')' | '*' | '+' | ',' | '-' | '.' | '/' | '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | ':' | ';' | '<' | '=' | '>' | '?' | '@' | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' | 'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z' | '[' | '\' | ']' | '^' | '_' | '`' | 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' | 's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z' | '{' | '|' | '} | '~'");
    grammar.addRule("<safechar> ::= ' ' | '!' | '\"' | '#' | '$' | '%' | '&' | ''' | '(' | ')' | '*' | '+' | ',' | '-' | '.' | '/' | '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | ':' | ';' | '<' | '=' | '>' | '?' | '@' | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' | 'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z' | '[' | '\' | ']' | '^' | '_' | '`' | 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' | 's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z' | '{' | '|' | '}' | '~'");
    grammar.addRule("<nospecial> ::= '!' | '\"' | '#' | '$' | '%' | '&' | ''' | '(' | ')' | '*' | '+' | ',' | '-' | '.' | '/' | '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | ';' | '<' | '=' | '>' | '?' | '@' | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' | 'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z' | '[' | '\' | ']' | '^' | '_' | '`' | 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' | 's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z' | '{' | '|' | '}' | '~'");
    grammar.addRule("<nonwhite> ::= ( ^ 0x20 0x0 0xD 0xA )");

    grammar.addRule("<SPACE> ::= ' ' { ' ' }");
    grammar.addRule("<crlf> ::= '\r' '\n'");

    grammar.addRule("<middle> ::= <nospecial> { <nospace> }");
    grammar.addRule("<trailing> ::= { <safechar> }");

    grammar.addRule("<params> ::= <SPACE> [ ':' <trailing> | <middle> [ <params> ] ]");
    grammar.addRule("<command> ::= <letter> { <letter> } | <number> <number> <number>");

    grammar.addRule("<nick> ::= <letter> { <letter> | <number> | <special> }");
    grammar.addRule("<user> ::= <nonwhite> { <nonwhite> }");

    grammar.addRule("<hostname-char> ::= <letter> | <number> | '-'");
    grammar.addRule("<hostname-end> ::= <letter> | <number>");
    grammar.addRule("<servername> ::= <letter> { <hostname-char> } <hostname-end>");

    grammar.addRule("<prefix> ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]");
    grammar.addRule("<message> ::= [ ':' <prefix> <SPACE> ] <command> <params> <crlf>");
}

// int main () {
// 	Grammar grammar;
// 	initGrammar(grammar);

// 	BNFParser parser(grammar);

// 	std::string input = ":nick!user@host COMMAND param1 param2 :This is a trailing message\r\n";
// 	size_t consumed = 0;
// 	ASTNode* ast = parser.parse("<message>", input, consumed);
// 	if (ast) {
// 		std::cout << "Parsed message successfully, consumed " << consumed << " characters." << std::endl;
		
// 		DataExtractor extractor;
// 		ExtractedData data = extractor.extract(ast);

// 		std::cout << "Extracted Data:" << std::endl;
// 		if (data.has("<nick>")) {
// 			std::cout << "  Nick: " << data.first("<nick>") << std::endl;
// 		}
// 		if (data.has("<user>")) {
// 			std::cout << "  User: " << data.first("<user>") << std::endl;
// 		}
// 		if (data.has("<host>")) {
// 			std::cout << "  Host: " << data.first("<host>") << std::endl;
// 		}
// 		if (data.has("<command>")) {
// 			std::cout << "  Command: " << data.first("<command>") << std::endl;
// 		}
// 		if (data.has("<middle>")) {
// 			std::vector<std::string> middles = data.all("<middle>");
// 			for (size_t i = 0; i < middles.size(); ++i) {
// 				std::cout << "  Middle Param " << (i+1) << ": " << middles[i] << std::endl;
// 			}
// 		}
// 		if (data.has("<trailing>")) {
// 			std::cout << "  Trailing: " << data.first("<trailing>") << std::endl;
// 		}

// 		delete ast;
// 	} else {
// 		std::cout << "Failed to parse message." << std::endl;
// 	}

// 	return 0;
// }

int main() {
	std::cout << "IRC Project Initialized" << std::endl;
	
	Server server;
	server
		.host("192.168.1.26")
		.port("6667")
		.name("MyServer")
		.pass("1234");

	server.init();
	server.run();
	server.close();
	
	return 0;
}