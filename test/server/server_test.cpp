#include <iostream>
#include <cassert>
#include <string>
#include <sstream>
#include "../../include/Server.hpp"
#include "../../include/Client.hpp"
#include "../../include/Channel.hpp"

// ============== CONSTRUCTOR TESTS ==============

void test_server_default_constructor() {
	Server server;
	assert(server.host() == "127.0.0.1");
	assert(server.port() == "6667");
	assert(server.pass() == "");
	assert(server.name() == "emptyName");
	assert(server.clientCount() == 0);
	assert(server.channelCount() == 0);
	std::cout << "✓ test_server_default_constructor passed" << std::endl;
}

// ============== GETTER/SETTER TESTS ==============

void test_server_host_getter_setter() {
	Server server;
	server.host("192.168.1.1");
	assert(server.host() == "192.168.1.1");
	std::cout << "✓ test_server_host_getter_setter passed" << std::endl;
}

void test_server_port_getter_setter() {
	Server server;
	server.port("8080");
	assert(server.port() == "8080");
	std::cout << "✓ test_server_port_getter_setter passed" << std::endl;
}

void test_server_pass_getter_setter() {
	Server server;
	server.pass("secret123");
	assert(server.pass() == "secret123");
	std::cout << "✓ test_server_pass_getter_setter passed" << std::endl;
}

void test_server_name_getter_setter() {
	Server server;
	server.name("MyIRCServer");
	assert(server.name() == "MyIRCServer");
	std::cout << "✓ test_server_name_getter_setter passed" << std::endl;
}

void test_server_setters_chaining() {
	Server server;
	server.host("10.0.0.1").port("6667").pass("pass").name("TestServer");
	
	assert(server.host() == "10.0.0.1");
	assert(server.port() == "6667");
	assert(server.pass() == "pass");
	assert(server.name() == "TestServer");
	std::cout << "✓ test_server_setters_chaining passed" << std::endl;
}

// ============== CLIENT CRUD TESTS ==============

void test_server_add_client() {
	Server server;
	Client* client = new Client(10);
	client->nickname("alice");
	
	assert(server.clientCount() == 0);
	assert(server.addClient(client));
	assert(server.clientCount() == 1);
	assert(server.hasClient(10));
	std::cout << "✓ test_server_add_client passed" << std::endl;
}

void test_server_add_multiple_clients() {
	Server server;
	
	Client* c1 = new Client(10);
	c1->nickname("alice");
	Client* c2 = new Client(11);
	c2->nickname("bob");
	Client* c3 = new Client(12);
	c3->nickname("carol");
	
	assert(server.addClient(c1));
	assert(server.addClient(c2));
	assert(server.addClient(c3));
	
	assert(server.clientCount() == 3);
	assert(server.hasClient(10));
	assert(server.hasClient(11));
	assert(server.hasClient(12));
	std::cout << "✓ test_server_add_multiple_clients passed" << std::endl;
}

void test_server_add_duplicate_client_fd() {
	Server server;
	Client* c1 = new Client(10);
	Client* c2 = new Client(10); // Same FD
	
	assert(server.addClient(c1));
	assert(!server.addClient(c2)); // Should fail
	
	delete c2; // Must delete since server didn't take ownership
std::cout << "✓ test_server_add_duplicate_client_fd passed" << std::endl;
}

void test_server_get_client() {
Server server;
Client* client = new Client(10);
client->nickname("alice").username("alice1");

server.addClient(client);

Client* retrieved = server.getClient(10);
assert(retrieved != NULL);
assert(retrieved->nickname() == "alice");
assert(retrieved->username() == "alice1");
assert(retrieved->fd() == 10);
std::cout << "✓ test_server_get_client passed" << std::endl;
}

void test_server_get_client_const() {
Server server;
Client* client = new Client(10);
client->nickname("bob");

server.addClient(client);

const Server& constServer = server;
const Client* retrieved = constServer.getClient(10);
assert(retrieved != NULL);
assert(retrieved->nickname() == "bob");
std::cout << "✓ test_server_get_client_const passed" << std::endl;
}

void test_server_get_nonexistent_client() {
Server server;
Client* client = server.getClient(999);
assert(client == NULL);
std::cout << "✓ test_server_get_nonexistent_client passed" << std::endl;
}

void test_server_has_client() {
Server server;
Client* client = new Client(10);

assert(!server.hasClient(10));
server.addClient(client);
assert(server.hasClient(10));
std::cout << "✓ test_server_has_client passed" << std::endl;
}

void test_server_remove_client() {
Server server;
Client* client = new Client(10);
client->nickname("alice");

server.addClient(client);
assert(server.hasClient(10));
assert(server.clientCount() == 1);

assert(server.removeClient(10));
assert(!server.hasClient(10));
assert(server.clientCount() == 0);
std::cout << "✓ test_server_remove_client passed" << std::endl;
}

void test_server_remove_nonexistent_client() {
Server server;
assert(!server.removeClient(999));
std::cout << "✓ test_server_remove_nonexistent_client passed" << std::endl;
}

void test_server_client_count() {
Server server;
assert(server.clientCount() == 0);

Client* c1 = new Client(10);
server.addClient(c1);
assert(server.clientCount() == 1);

Client* c2 = new Client(11);
server.addClient(c2);
assert(server.clientCount() == 2);

server.removeClient(10);
assert(server.clientCount() == 1);

server.removeClient(11);
assert(server.clientCount() == 0);
std::cout << "✓ test_server_client_count passed" << std::endl;
}

void test_server_clients_map_access() {
Server server;
Client* c1 = new Client(10);
c1->nickname("alice");
Client* c2 = new Client(11);
c2->nickname("bob");

server.addClient(c1);
server.addClient(c2);

std::map<int, Client*>& clientsMap = server.clients();
assert(clientsMap.size() == 2);
assert(clientsMap.find(10) != clientsMap.end());
assert(clientsMap.find(11) != clientsMap.end());
assert(clientsMap[10]->nickname() == "alice");
assert(clientsMap[11]->nickname() == "bob");
std::cout << "✓ test_server_clients_map_access passed" << std::endl;
}

// ============== CHANNEL CRUD TESTS ==============

void test_server_add_channel() {
Server server;
Channel* channel = new Channel("#general");

assert(server.channelCount() == 0);
assert(server.addChannel(channel));
assert(server.channelCount() == 1);
assert(server.hasChannel("#general"));
std::cout << "✓ test_server_add_channel passed" << std::endl;
}

void test_server_add_multiple_channels() {
Server server;

Channel* ch1 = new Channel("#general");
Channel* ch2 = new Channel("#dev");
Channel* ch3 = new Channel("#random");

assert(server.addChannel(ch1));
assert(server.addChannel(ch2));
assert(server.addChannel(ch3));

assert(server.channelCount() == 3);
assert(server.hasChannel("#general"));
assert(server.hasChannel("#dev"));
assert(server.hasChannel("#random"));
std::cout << "✓ test_server_add_multiple_channels passed" << std::endl;
}

void test_server_add_duplicate_channel() {
Server server;
Channel* ch1 = new Channel("#general");
Channel* ch2 = new Channel("#general"); // Same name

assert(server.addChannel(ch1));
assert(!server.addChannel(ch2)); // Should fail

delete ch2; // Must delete since server didn't take ownership
	std::cout << "✓ test_server_add_duplicate_channel passed" << std::endl;
}

void test_server_get_channel() {
	Server server;
	Channel* channel = new Channel("#dev");
	channel->topic("Development channel");
	
	server.addChannel(channel);
	
	Channel* retrieved = server.getChannel("#dev");
	assert(retrieved != NULL);
	assert(retrieved->name() == "#dev");
	assert(retrieved->topic() == "Development channel");
	std::cout << "✓ test_server_get_channel passed" << std::endl;
}

void test_server_get_channel_const() {
	Server server;
	Channel* channel = new Channel("#test");
	
	server.addChannel(channel);
	
	const Server& constServer = server;
	const Channel* retrieved = constServer.getChannel("#test");
	assert(retrieved != NULL);
	assert(retrieved->name() == "#test");
	std::cout << "✓ test_server_get_channel_const passed" << std::endl;
}

void test_server_get_nonexistent_channel() {
	Server server;
	Channel* channel = server.getChannel("#nonexistent");
	assert(channel == NULL);
	std::cout << "✓ test_server_get_nonexistent_channel passed" << std::endl;
}

void test_server_has_channel() {
	Server server;
	Channel* channel = new Channel("#test");
	
	assert(!server.hasChannel("#test"));
	server.addChannel(channel);
	assert(server.hasChannel("#test"));
	std::cout << "✓ test_server_has_channel passed" << std::endl;
}

void test_server_remove_channel() {
	Server server;
	Channel* channel = new Channel("#temp");
	
	server.addChannel(channel);
	assert(server.hasChannel("#temp"));
	assert(server.channelCount() == 1);
	
	assert(server.removeChannel("#temp"));
	assert(!server.hasChannel("#temp"));
	assert(server.channelCount() == 0);
	std::cout << "✓ test_server_remove_channel passed" << std::endl;
}

void test_server_remove_nonexistent_channel() {
	Server server;
	assert(!server.removeChannel("#nonexistent"));
	std::cout << "✓ test_server_remove_nonexistent_channel passed" << std::endl;
}

void test_server_channel_count() {
	Server server;
	assert(server.channelCount() == 0);
	
	Channel* ch1 = new Channel("#ch1");
	server.addChannel(ch1);
	assert(server.channelCount() == 1);
	
	Channel* ch2 = new Channel("#ch2");
	server.addChannel(ch2);
	assert(server.channelCount() == 2);
	
	server.removeChannel("#ch1");
	assert(server.channelCount() == 1);
	
	server.removeChannel("#ch2");
	assert(server.channelCount() == 0);
	std::cout << "✓ test_server_channel_count passed" << std::endl;
}

void test_server_channels_map_access() {
	Server server;
	Channel* ch1 = new Channel("#general");
	Channel* ch2 = new Channel("#dev");
	
	server.addChannel(ch1);
	server.addChannel(ch2);
	
	std::map<std::string, Channel*>& channelsMap = server.channels();
	assert(channelsMap.size() == 2);
	assert(channelsMap.find("#general") != channelsMap.end());
	assert(channelsMap.find("#dev") != channelsMap.end());
	assert(channelsMap["#general"]->name() == "#general");
	assert(channelsMap["#dev"]->name() == "#dev");
	std::cout << "✓ test_server_channels_map_access passed" << std::endl;
}

// ============== INTEGRATION TESTS ==============

void test_server_client_channel_integration() {
	Server server;
	server.name("TestServer");
	
	// Add clients
	Client* c1 = new Client(10);
	c1->nickname("alice").username("alice1");
	Client* c2 = new Client(11);
	c2->nickname("bob").username("bob1");
	
	server.addClient(c1);
	server.addClient(c2);
	
	// Add channel
	Channel* ch = new Channel("#general");
	server.addChannel(ch);
	
	// Clients join channel
	server.getClient(10)->joinChannel("#general");
	server.getClient(11)->joinChannel("#general");
	
	server.getChannel("#general")->addMember("alice");
	server.getChannel("#general")->addMember("bob");
	
	// Verify
	assert(server.clientCount() == 2);
	assert(server.channelCount() == 1);
	assert(server.getClient(10)->isInChannel("#general"));
	assert(server.getClient(11)->isInChannel("#general"));
	assert(server.getChannel("#general")->hasMember("alice"));
	assert(server.getChannel("#general")->hasMember("bob"));
	
	std::cout << "✓ test_server_client_channel_integration passed" << std::endl;
}

void test_server_client_disconnect_cleanup() {
	Server server;
	
	// Add client with channels
	Client* client = new Client(10);
	client->nickname("user1");
	client->joinChannel("#ch1");
	client->joinChannel("#ch2");
	
	server.addClient(client);
	
	Channel* ch1 = new Channel("#ch1");
	Channel* ch2 = new Channel("#ch2");
	ch1->addMember("user1");
	ch2->addMember("user1");
	
	server.addChannel(ch1);
	server.addChannel(ch2);
	
	// Cleanup before disconnect
	server.getChannel("#ch1")->removeMember("user1");
	server.getChannel("#ch2")->removeMember("user1");
	server.removeClient(10);
	
	assert(!server.hasClient(10));
	assert(!server.getChannel("#ch1")->hasMember("user1"));
	assert(!server.getChannel("#ch2")->hasMember("user1"));
	
	std::cout << "✓ test_server_client_disconnect_cleanup passed" << std::endl;
}

void test_server_empty_channel_removal() {
	Server server;
	
	Channel* ch = new Channel("#temp");
	ch->addMember("user1");
	server.addChannel(ch);
	
	// User leaves
	server.getChannel("#temp")->removeMember("user1");
	
	// Channel is now empty, could be removed
	assert(server.hasChannel("#temp"));
	server.removeChannel("#temp");
	assert(!server.hasChannel("#temp"));
	
	std::cout << "✓ test_server_empty_channel_removal passed" << std::endl;
}

void test_server_multiple_operations() {
	Server server;
	server.name("MultiOpServer");
	
	// Add 3 clients
	for (int i = 0; i < 3; ++i) {
		Client* c = new Client(10 + i);
		c->nickname("user");
		server.addClient(c);
	}
	
	// Add 2 channels
	Channel* ch1 = new Channel("#ch1");
	Channel* ch2 = new Channel("#ch2");
	server.addChannel(ch1);
	server.addChannel(ch2);
	
	// Remove 1 client
	server.removeClient(11);
	
	// Remove 1 channel
	server.removeChannel("#ch2");
	
	assert(server.clientCount() == 2);
	assert(server.channelCount() == 1);
	assert(server.hasClient(10));
	assert(!server.hasClient(11));
	assert(server.hasClient(12));
	assert(server.hasChannel("#ch1"));
	assert(!server.hasChannel("#ch2"));
	
	std::cout << "✓ test_server_multiple_operations passed" << std::endl;
}

// ============== EDGE CASE TESTS ==============

void test_server_empty_server() {
	Server server;
	assert(server.clientCount() == 0);
	assert(server.channelCount() == 0);
	assert(!server.hasClient(0));
	assert(!server.hasChannel("#any"));
	assert(server.getClient(0) == NULL);
	assert(server.getChannel("#any") == NULL);
	std::cout << "✓ test_server_empty_server passed" << std::endl;
}

void test_server_special_characters() {
	Server server;
	server.name("Test-Server_123");
	server.pass("p@ss!w0rd#123");
	server.host("192.168.1.100");
	
	assert(server.name() == "Test-Server_123");
	assert(server.pass() == "p@ss!w0rd#123");
	assert(server.host() == "192.168.1.100");
	std::cout << "✓ test_server_special_characters passed" << std::endl;
}

void test_server_large_fd() {
	Server server;
	Client* client = new Client(999999);
	server.addClient(client);
	assert(server.hasClient(999999));
	std::cout << "✓ test_server_large_fd passed" << std::endl;
}

void test_server_many_clients() {
	Server server;
	const int count = 100;
	
	for (int i = 0; i < count; ++i) {
		Client* c = new Client(1000 + i);
		server.addClient(c);
	}
	
	assert(server.clientCount() == count);
	assert(server.hasClient(1000));
	assert(server.hasClient(1050));
	assert(server.hasClient(1099));
	std::cout << "✓ test_server_many_clients passed" << std::endl;
}

void test_server_many_channels() {
	Server server;
	const int count = 50;
	
	for (int i = 0; i < count; ++i) {
		std::string name = "#channel";
		// Simple integer to string conversion for C++98
		std::ostringstream oss;
		oss << i;
		name += oss.str();
		Channel* ch = new Channel(name);
		server.addChannel(ch);
	}
	
	assert(server.channelCount() == count);
	assert(server.hasChannel("#channel0"));
	assert(server.hasChannel("#channel25"));
	assert(server.hasChannel("#channel49"));
	std::cout << "✓ test_server_many_channels passed" << std::endl;
}

// ============== COMPLEX SCENARIO TESTS ==============

void test_server_full_irc_scenario() {
	Server server;
	server.name("IRCServer").host("127.0.0.1").port("6667").pass("admin");
	
	// Create clients
	Client* admin = new Client(10);
	admin->nickname("admin").username("admin").realname("Administrator");
	admin->registered(true);
	
	Client* user1 = new Client(11);
	user1->nickname("alice").username("alice1").realname("Alice");
	user1->registered(true);
	
	Client* user2 = new Client(12);
	user2->nickname("bob").username("bob1").realname("Bob");
	user2->registered(true);
	
	server.addClient(admin);
	server.addClient(user1);
	server.addClient(user2);
	
	// Create channels
	Channel* general = new Channel("#general");
	general->topic("Welcome!").topicSetter("admin");
	general->addMember("admin");
	general->addMember("alice");
	general->addMember("bob");
	general->addOperator("admin");
	
	Channel* private_ch = new Channel("#private");
	private_ch->addMode('i');
	private_ch->key("secret");
	private_ch->addMember("admin");
	private_ch->addOperator("admin");
	private_ch->addInvited("alice");
	
	server.addChannel(general);
	server.addChannel(private_ch);
	
	// Verify complete state
	assert(server.clientCount() == 3);
	assert(server.channelCount() == 2);
	assert(server.getChannel("#general")->hasMember("alice"));
	assert(server.getChannel("#private")->isInviteOnly());
	assert(server.getChannel("#private")->hasInvited("alice"));
	assert(server.getClient(10)->isRegistered());
	
	std::cout << "✓ test_server_full_irc_scenario passed" << std::endl;
}

// ============== TEST RUNNER ==============

int run_server_tests() {
	std::cout << "\n========== SERVER TESTS ==========" << std::endl;
	
	std::cout << "\n--- Constructor Tests ---" << std::endl;
	test_server_default_constructor();
	
	std::cout << "\n--- Getter/Setter Tests ---" << std::endl;
	test_server_host_getter_setter();
	test_server_port_getter_setter();
	test_server_pass_getter_setter();
	test_server_name_getter_setter();
	test_server_setters_chaining();
	
	std::cout << "\n--- Client CRUD Tests ---" << std::endl;
	test_server_add_client();
	test_server_add_multiple_clients();
	test_server_add_duplicate_client_fd();
	test_server_get_client();
	test_server_get_client_const();
	test_server_get_nonexistent_client();
	test_server_has_client();
	test_server_remove_client();
	test_server_remove_nonexistent_client();
	test_server_client_count();
	test_server_clients_map_access();
	
	std::cout << "\n--- Channel CRUD Tests ---" << std::endl;
	test_server_add_channel();
	test_server_add_multiple_channels();
	test_server_add_duplicate_channel();
	test_server_get_channel();
	test_server_get_channel_const();
	test_server_get_nonexistent_channel();
	test_server_has_channel();
	test_server_remove_channel();
	test_server_remove_nonexistent_channel();
	test_server_channel_count();
	test_server_channels_map_access();
	
	std::cout << "\n--- Integration Tests ---" << std::endl;
	test_server_client_channel_integration();
	test_server_client_disconnect_cleanup();
	test_server_empty_channel_removal();
	test_server_multiple_operations();
	
	std::cout << "\n--- Edge Case Tests ---" << std::endl;
	test_server_empty_server();
	test_server_special_characters();
	test_server_large_fd();
	test_server_many_clients();
	test_server_many_channels();
	
	std::cout << "\n--- Complex Scenario Tests ---" << std::endl;
	test_server_full_irc_scenario();
	
	std::cout << "========== ALL SERVER TESTS PASSED ==========" << std::endl;
	return 0;
}
