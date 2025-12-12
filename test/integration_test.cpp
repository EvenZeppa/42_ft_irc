#include <iostream>
#include <cassert>
#include <string>
#include <ctime>
#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"

// ============== SERVER <-> CLIENT INTEGRATION ==============

void test_server_client_add_retrieve() {
	Server server;
	server.name("TestServer");
	
	Client* client = new Client(42);
	client->nickname("testuser").username("test").realname("Test User");
	
	server.addClient(client);
	
	Client* retrieved = server.getClient(42);
	assert(retrieved != NULL);
	assert(retrieved->nickname() == "testuser");
	assert(retrieved->username() == "test");
	assert(retrieved->fd() == 42);
	std::cout << "✓ test_server_client_add_retrieve passed" << std::endl;
}

void test_server_multiple_clients_independence() {
	Server server;
	
	Client* c1 = new Client(10);
	c1->nickname("client1").username("user1");
	Client* c2 = new Client(11);
	c2->nickname("client2").username("user2");
	Client* c3 = new Client(12);
	c3->nickname("client3").username("user3");
	
	server.addClient(c1);
	server.addClient(c2);
	server.addClient(c3);
	
	// Modify one client and ensure others are unaffected
	server.getClient(10)->username("modified_user1");
	
	assert(server.getClient(10)->username() == "modified_user1");
	assert(server.getClient(11)->username() == "user2");
	assert(server.getClient(12)->username() == "user3");
	std::cout << "✓ test_server_multiple_clients_independence passed" << std::endl;
}

void test_server_client_removal_consistency() {
	Server server;
	
	Client* c1 = new Client(10);
	c1->nickname("remove1");
	Client* c2 = new Client(11);
	c2->nickname("keep");
	Client* c3 = new Client(12);
	c3->nickname("remove2");
	
	server.addClient(c1);
	server.addClient(c2);
	server.addClient(c3);
	
	assert(server.clientCount() == 3);
	
	server.removeClient(10);
	assert(server.clientCount() == 2);
	assert(!server.hasClient(10));
	assert(server.hasClient(11));
	assert(server.hasClient(12));
	
	server.removeClient(12);
	assert(server.clientCount() == 1);
	assert(server.hasClient(11));
	std::cout << "✓ test_server_client_removal_consistency passed" << std::endl;
}

void test_client_registration_flow() {
	Server server;
	Client* client = new Client(10);
	server.addClient(client);
	
	// Initial state
	assert(!client->isPassReceived());
	assert(!client->isNickReceived());
	assert(!client->isUserReceived());
	assert(!client->isRegistered());
	
	// PASS command
	client->passReceived(true);
	assert(client->isPassReceived());
	assert(!client->isRegistered());
	
	// NICK command
	client->nickname("alice");
	client->nickReceived(true);
	assert(client->isNickReceived());
	assert(!client->isRegistered());
	
	// USER command
	client->username("alice").realname("Alice Smith");
	client->userReceived(true);
	assert(client->isUserReceived());
	
	// Complete registration
	client->registered(true);
	assert(client->isRegistered());
	
	std::cout << "✓ test_client_registration_flow passed" << std::endl;
}

// ============== SERVER <-> CHANNEL INTEGRATION ==============

void test_server_channel_add_retrieve() {
	Server server;
	server.name("TestServer");
	
	Channel* channel = new Channel("#general");
	channel->topic("Welcome");
	
	server.addChannel(channel);
	
	Channel* retrieved = server.getChannel("#general");
	assert(retrieved != NULL);
	assert(retrieved->name() == "#general");
	assert(retrieved->topic() == "Welcome");
	std::cout << "✓ test_server_channel_add_retrieve passed" << std::endl;
}

void test_server_multiple_channels_independence() {
	Server server;
	
	Channel* ch1 = new Channel("#channel1");
	Channel* ch2 = new Channel("#channel2");
	Channel* ch3 = new Channel("#channel3");
	
	server.addChannel(ch1);
	server.addChannel(ch2);
	server.addChannel(ch3);
	
	// Modify one channel and ensure others are unaffected
	server.getChannel("#channel1")->limit(100);
	
	assert(server.getChannel("#channel1")->limit() == 100);
	assert(server.getChannel("#channel2")->limit() == 0);
	assert(server.getChannel("#channel3")->limit() == 0);
	std::cout << "✓ test_server_multiple_channels_independence passed" << std::endl;
}

void test_server_channel_removal_consistency() {
	Server server;
	
	Channel* ch1 = new Channel("#remove1");
	Channel* ch2 = new Channel("#keep");
	Channel* ch3 = new Channel("#remove2");
	
	server.addChannel(ch1);
	server.addChannel(ch2);
	server.addChannel(ch3);
	
	assert(server.channelCount() == 3);
	
	server.removeChannel("#remove1");
	assert(server.channelCount() == 2);
	assert(!server.hasChannel("#remove1"));
	assert(server.hasChannel("#keep"));
	assert(server.hasChannel("#remove2"));
	
	server.removeChannel("#remove2");
	assert(server.channelCount() == 1);
	assert(server.hasChannel("#keep"));
	std::cout << "✓ test_server_channel_removal_consistency passed" << std::endl;
}

// ============== CLIENT <-> CHANNEL INTEGRATION ==============

void test_client_joins_channel() {
	Client client(10);
	client.nickname("user1");
	
	Channel channel("#general");
	
	// Client joins channel
	client.joinChannel("#general");
	assert(client.isInChannel("#general"));
	
	// Channel adds client as member
	channel.addMember("user1");
	assert(channel.hasMember("user1"));
	
	std::cout << "✓ test_client_joins_channel passed" << std::endl;
}

void test_client_leaves_channel() {
	Client client(10);
	client.nickname("user2");
	
	Channel channel("#dev");
	
	// Setup: client is in channel
	client.joinChannel("#dev");
	channel.addMember("user2");
	
	assert(client.isInChannel("#dev"));
	assert(channel.hasMember("user2"));
	
	// Client leaves
	client.leaveChannel("#dev");
	channel.removeMember("user2");
	
	assert(!client.isInChannel("#dev"));
	assert(!channel.hasMember("user2"));
	std::cout << "✓ test_client_leaves_channel passed" << std::endl;
}

void test_client_multiple_channels_membership() {
	Client client(10);
	client.nickname("user3");
	
	Channel ch1("#channel1");
	Channel ch2("#channel2");
	Channel ch3("#channel3");
	
	// Client joins multiple channels
	client.joinChannel("#channel1");
	client.joinChannel("#channel2");
	client.joinChannel("#channel3");
	
	ch1.addMember("user3");
	ch2.addMember("user3");
	ch3.addMember("user3");
	
	assert(client.isInChannel("#channel1"));
	assert(client.isInChannel("#channel2"));
	assert(client.isInChannel("#channel3"));
	
	assert(ch1.hasMember("user3"));
	assert(ch2.hasMember("user3"));
	assert(ch3.hasMember("user3"));
	
	// Client leaves one channel
	client.leaveChannel("#channel2");
	ch2.removeMember("user3");
	
	assert(client.isInChannel("#channel1"));
	assert(!client.isInChannel("#channel2"));
	assert(client.isInChannel("#channel3"));
	
	assert(ch1.hasMember("user3"));
	assert(!ch2.hasMember("user3"));
	assert(ch3.hasMember("user3"));
	std::cout << "✓ test_client_multiple_channels_membership passed" << std::endl;
}

void test_channel_multiple_clients_membership() {
	Channel channel("#general");
	
	Client c1(10);
	c1.nickname("alice");
	Client c2(11);
	c2.nickname("bob");
	Client c3(12);
	c3.nickname("carol");
	
	// Multiple clients join channel
	c1.joinChannel("#general");
	c2.joinChannel("#general");
	c3.joinChannel("#general");
	
	channel.addMember("alice");
	channel.addMember("bob");
	channel.addMember("carol");
	
	assert(c1.isInChannel("#general"));
	assert(c2.isInChannel("#general"));
	assert(c3.isInChannel("#general"));
	
	assert(channel.hasMember("alice"));
	assert(channel.hasMember("bob"));
	assert(channel.hasMember("carol"));
	
	// One client leaves
	c2.leaveChannel("#general");
	channel.removeMember("bob");
	
	assert(c1.isInChannel("#general"));
	assert(!c2.isInChannel("#general"));
	assert(c3.isInChannel("#general"));
	
	assert(channel.hasMember("alice"));
	assert(!channel.hasMember("bob"));
	assert(channel.hasMember("carol"));
	std::cout << "✓ test_channel_multiple_clients_membership passed" << std::endl;
}

// ============== FULL SERVER INTEGRATION ==============

void test_full_server_client_channel_integration() {
	Server server;
	server.name("IRC-Server").host("127.0.0.1").port("6667").pass("admin");
	
	// Create clients
	Client* c1 = new Client(10);
	c1->nickname("alice").username("alice1").realname("Alice Smith").hostname("127.0.0.1");
	Client* c2 = new Client(11);
	c2->nickname("bob").username("bob1").realname("Bob Jones").hostname("127.0.0.1");
	Client* c3 = new Client(12);
	c3->nickname("carol").username("carol1").realname("Carol White").hostname("127.0.0.1");
	
	// Add clients to server
	server.addClient(c1);
	server.addClient(c2);
	server.addClient(c3);
	
	// Create channels
	Channel* ch1 = new Channel("#general");
	Channel* ch2 = new Channel("#dev");
	ch2->key("devpass");
	
	// Add channels to server
	server.addChannel(ch1);
	server.addChannel(ch2);
	
	// Client operations: alice and bob join #general
	server.getClient(10)->joinChannel("#general");
	server.getClient(11)->joinChannel("#general");
	
	server.getChannel("#general")->addMember("alice");
	server.getChannel("#general")->addMember("bob");
	
	// Client operations: bob and carol join #dev
	server.getClient(11)->joinChannel("#dev");
	server.getClient(12)->joinChannel("#dev");
	
	server.getChannel("#dev")->addMember("bob");
	server.getChannel("#dev")->addMember("carol");
	
	// Verify state
	assert(server.clientCount() == 3);
	assert(server.channelCount() == 2);
	
	assert(server.getClient(10)->isInChannel("#general"));
	assert(!server.getClient(10)->isInChannel("#dev"));
	
	assert(server.getClient(11)->isInChannel("#general"));
	assert(server.getClient(11)->isInChannel("#dev"));
	
	assert(!server.getClient(12)->isInChannel("#general"));
	assert(server.getClient(12)->isInChannel("#dev"));
	
	assert(server.getChannel("#general")->hasMember("alice"));
	assert(server.getChannel("#general")->hasMember("bob"));
	assert(!server.getChannel("#general")->hasMember("carol"));
	
	assert(!server.getChannel("#dev")->hasMember("alice"));
	assert(server.getChannel("#dev")->hasMember("bob"));
	assert(server.getChannel("#dev")->hasMember("carol"));
	
	std::cout << "✓ test_full_server_client_channel_integration passed" << std::endl;
}

void test_channel_operator_and_invite_workflow() {
	Server server;
	server.name("TestServer");
	
	// Create and add clients
	Client* admin = new Client(10);
	admin->nickname("admin").username("admin1").realname("Admin User");
	Client* guest = new Client(11);
	guest->nickname("guest").username("guest1").realname("Guest User");
	
	server.addClient(admin);
	server.addClient(guest);
	
	// Create and add channel
	Channel* channel = new Channel("#restricted");
	channel->addMode('i'); // Invite only
	server.addChannel(channel);
	
	// Admin joins and becomes operator
	server.getClient(10)->joinChannel("#restricted");
	server.getChannel("#restricted")->addMember("admin");
	server.getChannel("#restricted")->addOperator("admin");
	
	// Admin invites guest
	server.getChannel("#restricted")->addInvited("guest");
	
	// Verify roles
	assert(server.getChannel("#restricted")->hasMember("admin"));
	assert(server.getChannel("#restricted")->hasOperator("admin"));
	assert(!server.getChannel("#restricted")->hasOperator("guest"));
	assert(server.getChannel("#restricted")->hasInvited("guest"));
	
	// Guest joins after invitation
	server.getClient(11)->joinChannel("#restricted");
	server.getChannel("#restricted")->addMember("guest");
	server.getChannel("#restricted")->removeInvited("guest");
	
	assert(server.getChannel("#restricted")->hasMember("guest"));
	assert(!server.getChannel("#restricted")->hasInvited("guest"));
	std::cout << "✓ test_channel_operator_and_invite_workflow passed" << std::endl;
}

void test_client_removal_from_all_channels() {
	Server server;
	server.name("TestServer");
	
	// Create client
	Client* client = new Client(10);
	client->nickname("user1");
	server.addClient(client);
	
	// Create channels
	Channel* ch1 = new Channel("#ch1");
	Channel* ch2 = new Channel("#ch2");
	Channel* ch3 = new Channel("#ch3");
	
	server.addChannel(ch1);
	server.addChannel(ch2);
	server.addChannel(ch3);
	
	// Client joins all channels
	server.getClient(10)->joinChannel("#ch1");
	server.getClient(10)->joinChannel("#ch2");
	server.getClient(10)->joinChannel("#ch3");
	
	server.getChannel("#ch1")->addMember("user1");
	server.getChannel("#ch2")->addMember("user1");
	server.getChannel("#ch3")->addMember("user1");
	
	// Verify membership
	assert(server.getChannel("#ch1")->hasMember("user1"));
	assert(server.getChannel("#ch2")->hasMember("user1"));
	assert(server.getChannel("#ch3")->hasMember("user1"));
	
	// Remove client from server
	// Manual cleanup of channel memberships
	server.getChannel("#ch1")->removeMember("user1");
	server.getChannel("#ch2")->removeMember("user1");
	server.getChannel("#ch3")->removeMember("user1");
	
	server.removeClient(10);
	
	// Verify state
	assert(!server.hasClient(10));
	assert(!server.getChannel("#ch1")->hasMember("user1"));
	assert(!server.getChannel("#ch2")->hasMember("user1"));
	assert(!server.getChannel("#ch3")->hasMember("user1"));
	std::cout << "✓ test_client_removal_from_all_channels passed" << std::endl;
}

void test_channel_modes_with_users() {
	Server server;
	
	// Create channel with modes
	Channel* channel = new Channel("#test");
	channel->addMode('i'); // Invite only
	channel->addMode('t'); // Topic restricted
	channel->addMode('k'); // Key
	channel->addMode('l'); // Limit
	channel->key("secret");
	channel->limit(5);
	
	server.addChannel(channel);
	
	// Add operator
	Client* op = new Client(10);
	op->nickname("operator");
	server.addClient(op);
	
	server.getClient(10)->joinChannel("#test");
	server.getChannel("#test")->addMember("operator");
	server.getChannel("#test")->addOperator("operator");
	
	// Invite users
	for (int i = 1; i <= 3; ++i) {
		Client* c = new Client(10 + i);
		std::string nick = "user";
		char num = '0' + i;
		nick += num;
		c->nickname(nick);
		server.addClient(c);
		
		server.getChannel("#test")->addInvited(nick);
		server.getClient(10 + i)->joinChannel("#test");
		server.getChannel("#test")->addMember(nick);
		server.getChannel("#test")->removeInvited(nick);
	}
	
	// Verify channel state
	assert(server.getChannel("#test")->isInviteOnly());
	assert(server.getChannel("#test")->isTopicRestricted());
	assert(server.getChannel("#test")->isKeyMode());
	assert(server.getChannel("#test")->isLimitMode());
	assert(server.getChannel("#test")->hasMember("operator"));
	assert(server.getChannel("#test")->hasMember("user1"));
	assert(server.getChannel("#test")->hasMember("user3"));
	assert(server.getChannel("#test")->hasOperator("operator"));
	
	std::cout << "✓ test_channel_modes_with_users passed" << std::endl;
}

void test_topic_update_workflow() {
	Server server;
	time_t now = time(NULL);
	
	// Create channel
	Channel* channel = new Channel("#general");
	channel->addMode('t'); // Topic restricted
	server.addChannel(channel);
	
	// Add operator who can set topic
	Client* op = new Client(10);
	op->nickname("admin");
	server.addClient(op);
	
	server.getClient(10)->joinChannel("#general");
	server.getChannel("#general")->addMember("admin");
	server.getChannel("#general")->addOperator("admin");
	
	// Set topic
	server.getChannel("#general")->topic("Welcome to the channel");
	server.getChannel("#general")->topicSetter("admin");
	server.getChannel("#general")->topicSetTime(now);
	
	assert(server.getChannel("#general")->topic() == "Welcome to the channel");
	assert(server.getChannel("#general")->topicSetter() == "admin");
	assert(server.getChannel("#general")->topicSetTime() == now);
	assert(server.getChannel("#general")->isTopicRestricted());
	
	std::cout << "✓ test_topic_update_workflow passed" << std::endl;
}

void test_buffer_operations_in_context() {
	Server server;
	
	Client* client = new Client(10);
	client->nickname("user1");
	server.addClient(client);
	
	// Simulate receiving data
	server.getClient(10)->appendToReadBuffer("NICK alice\r\n");
	server.getClient(10)->appendToReadBuffer("USER alice 0 * :Alice\r\n");
	
	std::string& readBuf = server.getClient(10)->readBuffer();
	assert(readBuf.find("NICK alice") != std::string::npos);
	assert(readBuf.find("USER alice") != std::string::npos);
	
	// Simulate sending data
	server.getClient(10)->appendToWriteBuffer(":server 001 alice :Welcome\r\n");
	assert(!server.getClient(10)->writeBuffer().empty());
	
	// Clear buffers
	server.getClient(10)->clearReadBuffer();
	server.getClient(10)->clearWriteBuffer();
	assert(server.getClient(10)->readBuffer().empty());
	assert(server.getClient(10)->writeBuffer().empty());
	
	std::cout << "✓ test_buffer_operations_in_context passed" << std::endl;
}

void test_complex_channel_lifecycle() {
	Server server;
	
	// Create channel
	Channel* channel = new Channel("#project");
	server.addChannel(channel);
	
	// Owner creates channel
	Client* owner = new Client(10);
	owner->nickname("owner");
	server.addClient(owner);
	
	server.getClient(10)->joinChannel("#project");
	server.getChannel("#project")->addMember("owner");
	server.getChannel("#project")->addOperator("owner");
	
	// Set modes and properties
	server.getChannel("#project")->addMode('i');
	server.getChannel("#project")->addMode('t');
	server.getChannel("#project")->topic("Project discussion");
	server.getChannel("#project")->topicSetter("owner");
	
	// Add team members
	for (int i = 1; i <= 3; ++i) {
		Client* member = new Client(10 + i);
		std::string nick = "member";
		char num = '0' + i;
		nick += num;
		member->nickname(nick);
		server.addClient(member);
		
		server.getChannel("#project")->addInvited(nick);
		server.getClient(10 + i)->joinChannel("#project");
		server.getChannel("#project")->addMember(nick);
		server.getChannel("#project")->removeInvited(nick);
	}
	
	// Promote one member to operator
	server.getChannel("#project")->addOperator("member1");
	
	// Verify final state
	assert(server.getChannel("#project")->hasMember("owner"));
	assert(server.getChannel("#project")->hasMember("member1"));
	assert(server.getChannel("#project")->hasMember("member2"));
	assert(server.getChannel("#project")->hasMember("member3"));
	assert(server.getChannel("#project")->hasOperator("owner"));
	assert(server.getChannel("#project")->hasOperator("member1"));
	assert(!server.getChannel("#project")->hasOperator("member2"));
	
	std::cout << "✓ test_complex_channel_lifecycle passed" << std::endl;
}

// ============== TEST RUNNER ==============

int run_integration_tests() {
	std::cout << "\n========== INTEGRATION TESTS ==========" << std::endl;
	
	std::cout << "\n--- Server <-> Client Integration ---" << std::endl;
	test_server_client_add_retrieve();
	test_server_multiple_clients_independence();
	test_server_client_removal_consistency();
	test_client_registration_flow();
	
	std::cout << "\n--- Server <-> Channel Integration ---" << std::endl;
	test_server_channel_add_retrieve();
	test_server_multiple_channels_independence();
	test_server_channel_removal_consistency();
	
	std::cout << "\n--- Client <-> Channel Integration ---" << std::endl;
	test_client_joins_channel();
	test_client_leaves_channel();
	test_client_multiple_channels_membership();
	test_channel_multiple_clients_membership();
	
	std::cout << "\n--- Full Server Integration ---" << std::endl;
	test_full_server_client_channel_integration();
	test_channel_operator_and_invite_workflow();
	test_client_removal_from_all_channels();
	test_channel_modes_with_users();
	test_topic_update_workflow();
	test_buffer_operations_in_context();
	test_complex_channel_lifecycle();
	
	std::cout << "========== ALL INTEGRATION TESTS PASSED ==========" << std::endl;
	return 0;
}
