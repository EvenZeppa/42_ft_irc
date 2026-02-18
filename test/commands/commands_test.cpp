#include "../../include/Server.hpp"
#include "../../include/Client.hpp"
#include "../../include/Channel.hpp"
#include "../../include/CommandManager.hpp"
#include <cassert>
#include <iostream>
#include <cstring>

// Helper function to remove client from server without deleting
void removeClientWithoutDelete(Server& server, int fd) {
	std::map<int, Client*>& clients = server.clients();
	clients.erase(fd);
}

// ============================================================================
// AUTHENTICATION COMMAND TESTS
// ============================================================================

void test_pass_command() {
	std::cout << "\n--- Testing PASS Command ---" << std::endl;
	
	Server server;
	server.pass("secret123").name("TestServer");
	
	Client client1(100);
	Client client2(101);
	
	std::vector<std::string> correctPass;
	correctPass.push_back("secret123");
	
	std::vector<std::string> wrongPass;
	wrongPass.push_back("wrong");
	
	CommandManager cm;
	
	// Test correct password
	try {
		cm.cmdPass(server, client1, correctPass);
		assert(client1.isPassReceived());
		std::cout << "✓ Correct password accepted" << std::endl;
	} catch (...) {
		std::cout << "✗ Correct password rejected" << std::endl;
		assert(false);
	}
	
	// Test wrong password
	try {
		cm.cmdPass(server, client2, wrongPass);
		std::cout << "✗ Wrong password accepted" << std::endl;
		assert(false);
	} catch (...) {
		std::cout << "✓ Wrong password rejected" << std::endl;
	}
	
	// Test already registered
	client1.registered(true);
	try {
		cm.cmdPass(server, client1, correctPass);
		std::cout << "✗ PASS allowed after registration" << std::endl;
		assert(false);
	} catch (...) {
		std::cout << "✓ PASS rejected after registration" << std::endl;
	}
}

void test_nick_command() {
	std::cout << "\n--- Testing NICK Command ---" << std::endl;
	
	Server server;
	server.name("TestServer");
	
	Client* client1 = new Client(100);
	Client* client2 = new Client(101);
	server.addClient(client1);
	server.addClient(client2);
	
	std::vector<std::string> validNick;
	validNick.push_back("Alice");
	
	std::vector<std::string> invalidNick;
	invalidNick.push_back("123invalid");
	
	std::vector<std::string> emptyNick;
	emptyNick.push_back("");
	
	CommandManager cm;
	
	// Test valid nickname
	try {
		cm.cmdNick(server, *client1, validNick);
		assert(client1->nickname() == "Alice");
		assert(client1->isNickReceived());
		std::cout << "✓ Valid nickname accepted" << std::endl;
	} catch (...) {
		std::cout << "✗ Valid nickname rejected" << std::endl;
		assert(false);
	}
	
	// Test invalid nickname (starts with digit)
	try {
		cm.cmdNick(server, *client2, invalidNick);
		std::cout << "✗ Invalid nickname accepted" << std::endl;
		assert(false);
	} catch (...) {
		std::cout << "✓ Invalid nickname rejected" << std::endl;
	}
	
	// Test empty nickname
	try {
		cm.cmdNick(server, *client2, emptyNick);
		std::cout << "✗ Empty nickname accepted" << std::endl;
		assert(false);
	} catch (...) {
		std::cout << "✓ Empty nickname rejected" << std::endl;
	}
	
	// Test nickname in use
	std::vector<std::string> usedNick;
	usedNick.push_back("Alice");
	try {
		cm.cmdNick(server, *client2, usedNick);
		std::cout << "✗ Duplicate nickname accepted" << std::endl;
		assert(false);
	} catch (...) {
		std::cout << "✓ Duplicate nickname rejected" << std::endl;
	}
	
	// Server will delete clients in destructor
}

void test_user_command() {
	std::cout << "\n--- Testing USER Command ---" << std::endl;
	
	Server server;
	server.name("TestServer");
	
	Client client(100);
	
	std::vector<std::string> userArgs;
	userArgs.push_back("alice");
	userArgs.push_back("hostname");
	userArgs.push_back("servername");
	userArgs.push_back("Alice Smith");
	
	CommandManager cm;
	
	// Test valid USER
	try {
		cm.cmdUser(server, client, userArgs);
		assert(client.username() == "alice");
		assert(client.realname() == "Alice Smith");
		assert(client.isUserReceived());
		std::cout << "✓ USER command accepted" << std::endl;
	} catch (...) {
		std::cout << "✗ USER command rejected" << std::endl;
		assert(false);
	}
	
	// Test already registered
	client.registered(true);
	try {
		cm.cmdUser(server, client, userArgs);
		std::cout << "✗ USER allowed after registration" << std::endl;
		assert(false);
	} catch (...) {
		std::cout << "✓ USER rejected after registration" << std::endl;
	}
}

void test_registration_flow() {
	std::cout << "\n--- Testing Complete Registration Flow ---" << std::endl;
	
	Server server;
	server.pass("pass123").name("TestServer");
	
	Client* client = new Client(100);
	server.addClient(client);
	
	CommandManager cm;
	
	// Step 1: PASS
	std::vector<std::string> passArgs;
	passArgs.push_back("pass123");
	cm.cmdPass(server, *client, passArgs);
	assert(!client->isRegistered());
	std::cout << "✓ PASS received, not registered yet" << std::endl;
	
	// Step 2: NICK
	std::vector<std::string> nickArgs;
	nickArgs.push_back("TestUser");
	cm.cmdNick(server, *client, nickArgs);
	assert(!client->isRegistered());
	std::cout << "✓ NICK received, not registered yet" << std::endl;
	
	// Step 3: USER (should trigger registration)
	std::vector<std::string> userArgs;
	userArgs.push_back("testuser");
	userArgs.push_back("host");
	userArgs.push_back("server");
	userArgs.push_back("Test User");
	cm.cmdUser(server, *client, userArgs);
	assert(client->isRegistered());
	std::cout << "✓ USER received, registration complete" << std::endl;
}

// ============================================================================
// CHANNEL COMMAND TESTS
// ============================================================================

void test_join_command() {
	std::cout << "\n--- Testing JOIN Command ---" << std::endl;
	
	Server server;
	server.name("TestServer");
	
	Client client1(100);
	client1.nickname("Alice").registered(true);
	server.addClient(&client1);
	
	Client client2(101);
	client2.nickname("Bob").registered(true);
	server.addClient(&client2);
	
	CommandManager cm;
	
	// Test creating a new channel
	std::vector<std::string> joinArgs;
	joinArgs.push_back("#test");
	
	cm.cmdJoin(server, client1, joinArgs);
	assert(server.hasChannel("#test"));
	assert(client1.isInChannel("#test"));
	
	Channel* channel = server.getChannel("#test");
	assert(channel->hasMember("Alice"));
	assert(channel->hasOperator("Alice")); // First user becomes op
	std::cout << "✓ Channel created and user joined as operator" << std::endl;
	
	// Test joining existing channel
	cm.cmdJoin(server, client2, joinArgs);
	assert(channel->hasMember("Bob"));
	assert(!channel->hasOperator("Bob")); // Second user is not op
	std::cout << "✓ Second user joined channel" << std::endl;
	
	// Test invalid channel name
	std::vector<std::string> invalidArgs;
	invalidArgs.push_back("invalid");
	try {
		cm.cmdJoin(server, client1, invalidArgs);
		std::cout << "✗ Invalid channel name accepted" << std::endl;
		assert(false);
	} catch (...) {
		std::cout << "✓ Invalid channel name rejected" << std::endl;
	}
}

void test_part_command() {
	std::cout << "\n--- Testing PART Command ---" << std::endl;
	
	Server server;
	server.name("TestServer");
	
	Client client(100);
	client.nickname("Alice").registered(true);
	server.addClient(&client);
	
	Channel* channel = new Channel("#test");
	channel->addMember("Alice");
	server.addChannel(channel);
	client.joinChannel("#test");
	
	CommandManager cm;
	
	// Test parting channel
	std::vector<std::string> partArgs;
	partArgs.push_back("#test");
	partArgs.push_back("Goodbye");
	
	cm.cmdPart(server, client, partArgs);
	assert(!client.isInChannel("#test"));
	assert(!channel->hasMember("Alice"));
	std::cout << "✓ User parted channel" << std::endl;
	
	// Test parting non-existent channel
	std::vector<std::string> invalidArgs;
	invalidArgs.push_back("#nonexistent");
	try {
		cm.cmdPart(server, client, invalidArgs);
		std::cout << "✗ PART on non-existent channel succeeded" << std::endl;
		assert(false);
	} catch (...) {
		std::cout << "✓ PART on non-existent channel rejected" << std::endl;
	}
}

void test_topic_command() {
	std::cout << "\n--- Testing TOPIC Command ---" << std::endl;
	
	Server server;
	server.name("TestServer");
	
	Client client1(100);
	client1.nickname("Alice").registered(true);
	server.addClient(&client1);
	
	Client client2(101);
	client2.nickname("Bob").registered(true);
	server.addClient(&client2);
	
	Channel* channel = new Channel("#test");
	channel->addMember("Alice");
	channel->addMember("Bob");
	channel->addOperator("Alice");
	channel->addMode('t'); // Topic restricted
	server.addChannel(channel);
	
	client1.joinChannel("#test");
	client2.joinChannel("#test");
	
	CommandManager cm;
	
	// Test setting topic as operator
	std::vector<std::string> topicArgs;
	topicArgs.push_back("#test");
	topicArgs.push_back("Welcome to the test channel!");
	
	cm.cmdTopic(server, client1, topicArgs);
	assert(channel->topic() == "Welcome to the test channel!");
	std::cout << "✓ Operator set topic" << std::endl;
	
	// Test setting topic as non-operator with +t
	try {
		cm.cmdTopic(server, client2, topicArgs);
		std::cout << "✗ Non-operator set topic with +t" << std::endl;
		assert(false);
	} catch (...) {
		std::cout << "✓ Non-operator blocked from setting topic with +t" << std::endl;
	}
	
	// Test viewing topic
	std::vector<std::string> viewArgs;
	viewArgs.push_back("#test");
	// Should not throw
	try {
		cm.cmdTopic(server, client2, viewArgs);
		std::cout << "✓ User can view topic" << std::endl;
	} catch (...) {
		std::cout << "✗ User cannot view topic" << std::endl;
		assert(false);
	}
}

// ============================================================================
// PRIVMSG COMMAND TESTS
// ============================================================================

void test_privmsg_command() {
	std::cout << "\n--- Testing PRIVMSG Command ---" << std::endl;
	
	Server server;
	server.name("TestServer");
	
	Client client1(100);
	client1.nickname("Alice").username("alice").hostname("localhost").registered(true);
	server.addClient(&client1);
	
	Client client2(101);
	client2.nickname("Bob").username("bob").hostname("localhost").registered(true);
	server.addClient(&client2);
	
	CommandManager cm;
	
	// Test private message to user
	std::vector<std::string> msgArgs;
	msgArgs.push_back("Bob");
	msgArgs.push_back("Hello Bob!");
	
	client2.clearWriteBuffer();
	cm.cmdPrivmsg(server, client1, msgArgs);
	assert(!client2.writeBuffer().empty());
	std::cout << "✓ Private message sent to user" << std::endl;
	
	// Test message to non-existent user
	std::vector<std::string> invalidArgs;
	invalidArgs.push_back("NonExistent");
	invalidArgs.push_back("Hello");
	try {
		cm.cmdPrivmsg(server, client1, invalidArgs);
		std::cout << "✗ Message to non-existent user succeeded" << std::endl;
		assert(false);
	} catch (...) {
		std::cout << "✓ Message to non-existent user rejected" << std::endl;
	}
	
	// Test channel message
	Channel* channel = new Channel("#test");
	channel->addMember("Alice");
	channel->addMember("Bob");
	server.addChannel(channel);
	client1.joinChannel("#test");
	client2.joinChannel("#test");
	
	std::vector<std::string> chanArgs;
	chanArgs.push_back("#test");
	chanArgs.push_back("Hello channel!");
	
	client2.clearWriteBuffer();
	cm.cmdPrivmsg(server, client1, chanArgs);
	assert(!client2.writeBuffer().empty());
	std::cout << "✓ Channel message sent" << std::endl;
}

// ============================================================================
// OPERATOR COMMAND TESTS
// ============================================================================

void test_kick_command() {
	std::cout << "\n--- Testing KICK Command ---" << std::endl;
	
	Server server;
	server.name("TestServer");
	
	Client client1(100);
	client1.nickname("Alice").username("alice").hostname("localhost").registered(true);
	server.addClient(&client1);
	
	Client client2(101);
	client2.nickname("Bob").username("bob").hostname("localhost").registered(true);
	server.addClient(&client2);
	
	Channel* channel = new Channel("#test");
	channel->addMember("Alice");
	channel->addMember("Bob");
	channel->addOperator("Alice");
	server.addChannel(channel);
	
	client1.joinChannel("#test");
	client2.joinChannel("#test");
	
	CommandManager cm;
	
	// Test kick by operator
	std::vector<std::string> kickArgs;
	kickArgs.push_back("#test");
	kickArgs.push_back("Bob");
	kickArgs.push_back("Misbehaving");
	
	cm.cmdKick(server, client1, kickArgs);
	assert(!channel->hasMember("Bob"));
	assert(!client2.isInChannel("#test"));
	std::cout << "✓ Operator kicked user" << std::endl;
	
	// Test kick by non-operator
	channel->addMember("Bob");
	client2.joinChannel("#test");
	
	try {
		cm.cmdKick(server, client2, kickArgs);
		std::cout << "✗ Non-operator kicked user" << std::endl;
		assert(false);
	} catch (...) {
		std::cout << "✓ Non-operator cannot kick" << std::endl;
	}
}

void test_invite_command() {
	std::cout << "\n--- Testing INVITE Command ---" << std::endl;
	
	Server server;
	server.name("TestServer");
	
	Client client1(100);
	client1.nickname("Alice").username("alice").hostname("localhost").registered(true);
	server.addClient(&client1);
	
	Client client2(101);
	client2.nickname("Bob").username("bob").hostname("localhost").registered(true);
	server.addClient(&client2);
	
	Channel* channel = new Channel("#test");
	channel->addMember("Alice");
	channel->addOperator("Alice");
	channel->addMode('i'); // Invite only
	server.addChannel(channel);
	
	client1.joinChannel("#test");
	
	CommandManager cm;
	
	// Test invite
	std::vector<std::string> inviteArgs;
	inviteArgs.push_back("Bob");
	inviteArgs.push_back("#test");
	
	cm.cmdInvite(server, client1, inviteArgs);
	assert(channel->hasInvited("Bob"));
	std::cout << "✓ User invited to channel" << std::endl;
	
	// Test invite already in channel
	channel->addMember("Bob");
	try {
		cm.cmdInvite(server, client1, inviteArgs);
		std::cout << "✗ Invited user already in channel" << std::endl;
		assert(false);
	} catch (...) {
		std::cout << "✓ Cannot invite user already in channel" << std::endl;
	}
}

// ============================================================================
// MODE COMMAND TESTS
// ============================================================================

void test_mode_command() {
	std::cout << "\n--- Testing MODE Command ---" << std::endl;
	
	Server server;
	server.name("TestServer");
	
	Client client1(100);
	client1.nickname("Alice").registered(true);
	server.addClient(&client1);
	
	Client client2(101);
	client2.nickname("Bob").registered(true);
	server.addClient(&client2);
	
	Channel* channel = new Channel("#test");
	channel->addMember("Alice");
	channel->addMember("Bob");
	channel->addOperator("Alice");
	server.addChannel(channel);
	
	client1.joinChannel("#test");
	client2.joinChannel("#test");
	
	CommandManager cm;
	
	// Test +i (invite-only)
	std::vector<std::string> modeArgs;
	modeArgs.push_back("#test");
	modeArgs.push_back("+i");
	
	cm.cmdMode(server, client1, modeArgs);
	assert(channel->hasMode('i'));
	std::cout << "✓ Invite-only mode set" << std::endl;
	
	// Test +t (topic restricted)
	modeArgs[1] = "+t";
	cm.cmdMode(server, client1, modeArgs);
	assert(channel->hasMode('t'));
	std::cout << "✓ Topic restricted mode set" << std::endl;
	
	// Test +k (key)
	modeArgs[1] = "+k";
	modeArgs.push_back("secret");
	cm.cmdMode(server, client1, modeArgs);
	assert(channel->hasMode('k'));
	assert(channel->key() == "secret");
	std::cout << "✓ Channel key set" << std::endl;
	
	// Test +l (limit)
	modeArgs.clear();
	modeArgs.push_back("#test");
	modeArgs.push_back("+l");
	modeArgs.push_back("10");
	cm.cmdMode(server, client1, modeArgs);
	assert(channel->hasMode('l'));
	assert(channel->limit() == 10);
	std::cout << "✓ Channel limit set" << std::endl;
	
	// Test +o (operator)
	modeArgs.clear();
	modeArgs.push_back("#test");
	modeArgs.push_back("+o");
	modeArgs.push_back("Bob");
	cm.cmdMode(server, client1, modeArgs);
	assert(channel->hasOperator("Bob"));
	std::cout << "✓ Operator status granted" << std::endl;
	
	// Test -o (remove operator)
	modeArgs[1] = "-o";
	cm.cmdMode(server, client1, modeArgs);
	assert(!channel->hasOperator("Bob"));
	std::cout << "✓ Operator status removed" << std::endl;
	
	// Test non-operator cannot set modes
	try {
		cm.cmdMode(server, client2, modeArgs);
		std::cout << "✗ Non-operator set mode" << std::endl;
		assert(false);
	} catch (...) {
		std::cout << "✓ Non-operator cannot set modes" << std::endl;
	}
}

// ============================================================================
// INTEGRATION TESTS
// ============================================================================

void test_full_workflow() {
	std::cout << "\n--- Testing Full IRC Workflow ---" << std::endl;
	
	Server server;
	server.pass("testpass").name("TestServer").host("localhost");
	
	Client client1(100);
	client1.hostname("localhost");
	server.addClient(&client1);
	
	Client client2(101);
	client2.hostname("localhost");
	server.addClient(&client2);
	
	CommandManager cm;
	
	// Client 1 registration
	std::vector<std::string> args;
	args.push_back("testpass");
	cm.cmdPass(server, client1, args);
	
	args.clear();
	args.push_back("Alice");
	cm.cmdNick(server, client1, args);
	
	args.clear();
	args.push_back("alice");
	args.push_back("host");
	args.push_back("server");
	args.push_back("Alice User");
	cm.cmdUser(server, client1, args);
	
	assert(client1.isRegistered());
	std::cout << "✓ Client 1 registered" << std::endl;
	
	// Client 2 registration
	args.clear();
	args.push_back("testpass");
	cm.cmdPass(server, client2, args);
	
	args.clear();
	args.push_back("Bob");
	cm.cmdNick(server, client2, args);
	
	args.clear();
	args.push_back("bob");
	args.push_back("host");
	args.push_back("server");
	args.push_back("Bob User");
	cm.cmdUser(server, client2, args);
	
	assert(client2.isRegistered());
	std::cout << "✓ Client 2 registered" << std::endl;
	
	// Alice creates channel
	args.clear();
	args.push_back("#general");
	cm.cmdJoin(server, client1, args);
	
	Channel* channel = server.getChannel("#general");
	assert(channel != NULL);
	assert(channel->hasOperator("Alice"));
	std::cout << "✓ Channel created" << std::endl;
	
	// Set channel modes
	args.clear();
	args.push_back("#general");
	args.push_back("+itk");
	args.push_back("secret");
	cm.cmdMode(server, client1, args);
	
	assert(channel->isInviteOnly());
	assert(channel->isTopicRestricted());
	assert(channel->key() == "secret");
	std::cout << "✓ Channel modes set" << std::endl;
	
	// Invite Bob
	args.clear();
	args.push_back("Bob");
	args.push_back("#general");
	cm.cmdInvite(server, client1, args);
	
	assert(channel->hasInvited("Bob"));
	std::cout << "✓ Bob invited" << std::endl;
	
	// Bob joins with key
	args.clear();
	args.push_back("#general");
	args.push_back("secret");
	cm.cmdJoin(server, client2, args);
	
	assert(channel->hasMember("Bob"));
	std::cout << "✓ Bob joined channel" << std::endl;
	
	// Set topic
	args.clear();
	args.push_back("#general");
	args.push_back("Welcome to the channel!");
	cm.cmdTopic(server, client1, args);
	
	assert(channel->topic() == "Welcome to the channel!");
	std::cout << "✓ Topic set" << std::endl;
	
	// Send messages
	args.clear();
	args.push_back("#general");
	args.push_back("Hello everyone!");
	client2.clearWriteBuffer();
	cm.cmdPrivmsg(server, client1, args);
	// Bob should have received the message (not testing buffer content here)
	std::cout << "✓ Channel message sent" << std::endl;
	
	// Private message
	args.clear();
	args.push_back("Bob");
	args.push_back("Private message");
	client2.clearWriteBuffer();
	cm.cmdPrivmsg(server, client1, args);
	std::cout << "✓ Private message sent" << std::endl;
	
	// Grant operator
	args.clear();
	args.push_back("#general");
	args.push_back("+o");
	args.push_back("Bob");
	cm.cmdMode(server, client1, args);
	
	assert(channel->hasOperator("Bob"));
	std::cout << "✓ Operator status granted" << std::endl;
	
	std::cout << "✓ Full workflow completed successfully" << std::endl;
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int run_commands_tests() {
	std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
	std::cout << "║     IRC COMMANDS TEST SUITE            ║" << std::endl;
	std::cout << "╚════════════════════════════════════════╝" << std::endl;
	
	try {
		// Authentication tests
		std::cout << "\n═══════════════════════════════════════" << std::endl;
		std::cout << "    AUTHENTICATION COMMANDS" << std::endl;
		std::cout << "═══════════════════════════════════════" << std::endl;
		test_pass_command();
		test_nick_command();
		test_user_command();
		test_registration_flow();
		
		// Channel tests
		std::cout << "\n═══════════════════════════════════════" << std::endl;
		std::cout << "    CHANNEL COMMANDS" << std::endl;
		std::cout << "═══════════════════════════════════════" << std::endl;
		test_join_command();
		test_part_command();
		test_topic_command();
		
		// Message tests
		std::cout << "\n═══════════════════════════════════════" << std::endl;
		std::cout << "    MESSAGE COMMANDS" << std::endl;
		std::cout << "═══════════════════════════════════════" << std::endl;
		test_privmsg_command();
		
		// Operator tests
		std::cout << "\n═══════════════════════════════════════" << std::endl;
		std::cout << "    OPERATOR COMMANDS" << std::endl;
		std::cout << "═══════════════════════════════════════" << std::endl;
		test_kick_command();
		test_invite_command();
		
		// Mode tests
		std::cout << "\n═══════════════════════════════════════" << std::endl;
		std::cout << "    MODE COMMANDS" << std::endl;
		std::cout << "═══════════════════════════════════════" << std::endl;
		test_mode_command();
		
		// Integration tests
		std::cout << "\n═══════════════════════════════════════" << std::endl;
		std::cout << "    INTEGRATION TESTS" << std::endl;
		std::cout << "═══════════════════════════════════════" << std::endl;
		test_full_workflow();
		
		std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
		std::cout << "║  ✓ ALL COMMAND TESTS PASSED!          ║" << std::endl;
		std::cout << "╚════════════════════════════════════════╝\n" << std::endl;
		
		return 0;
	} catch (const std::exception& e) {
		std::cerr << "\n✗ COMMAND TEST FAILED WITH EXCEPTION: " << e.what() << std::endl;
		return 1;
	} catch (...) {
		std::cerr << "\n✗ COMMAND TEST FAILED WITH UNKNOWN EXCEPTION" << std::endl;
		return 1;
	}
}
