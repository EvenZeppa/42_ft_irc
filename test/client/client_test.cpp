#include <iostream>
#include <cassert>
#include <string>
#include <ctime>
#include "../../include/Client.hpp"

// ============== CONSTRUCTOR TESTS ==============

void test_client_default_constructor() {
	Client client;
	assert(client.realname() == "");
	assert(client.username() == "");
	assert(client.nickname() == "");
	assert(client.hostname() == "");
	assert(client.fd() == -1);
	assert(client.epollServerFd() == -1);
	assert(!client.isPassReceived());
	assert(!client.isNickReceived());
	assert(!client.isUserReceived());
	assert(!client.isRegistered());
	assert(client.lastPing() == 0);
	assert(client.lastPong() == 0);
	assert(client.readBuffer() == "");
	assert(client.writeBuffer() == "");
	std::cout << "✓ test_client_default_constructor passed" << std::endl;
}

void test_client_fd_constructor() {
	Client client(42);
	assert(client.fd() == 42);
	assert(client.realname() == "");
	assert(client.username() == "");
	assert(client.nickname() == "");
	assert(client.epollServerFd() == -1);
	std::cout << "✓ test_client_fd_constructor passed" << std::endl;
}

// ============== GETTER/SETTER TESTS ==============

void test_client_realname_getter_setter() {
	Client client;
	client.realname("John Doe");
	assert(client.realname() == "John Doe");
	std::cout << "✓ test_client_realname_getter_setter passed" << std::endl;
}

void test_client_username_getter_setter() {
	Client client;
	client.username("john");
	assert(client.username() == "john");
	std::cout << "✓ test_client_username_getter_setter passed" << std::endl;
}

void test_client_nickname_getter_setter() {
	Client client;
	client.nickname("john123");
	assert(client.nickname() == "john123");
	std::cout << "✓ test_client_nickname_getter_setter passed" << std::endl;
}

void test_client_hostname_getter_setter() {
	Client client;
	client.hostname("127.0.0.1");
	assert(client.hostname() == "127.0.0.1");
	std::cout << "✓ test_client_hostname_getter_setter passed" << std::endl;
}

void test_client_fd_getter_setter() {
	Client client;
	client.fd(100);
	assert(client.fd() == 100);
	std::cout << "✓ test_client_fd_getter_setter passed" << std::endl;
}

void test_client_epoll_fd_getter_setter() {
	Client client;
	client.epollServerFd(5);
	assert(client.epollServerFd() == 5);
	std::cout << "✓ test_client_epoll_fd_getter_setter passed" << std::endl;
}

void test_client_setters_chaining() {
	Client client;
	client.realname("Alice").username("alice").nickname("alice1").hostname("192.168.1.1").fd(10);
	
	assert(client.realname() == "Alice");
	assert(client.username() == "alice");
	assert(client.nickname() == "alice1");
	assert(client.hostname() == "192.168.1.1");
	assert(client.fd() == 10);
	std::cout << "✓ test_client_setters_chaining passed" << std::endl;
}

// ============== FULLMASK TEST ==============

void test_client_fullmask() {
	Client client;
	client.nickname("john").username("jdoe").hostname("example.com");
	assert(client.fullmask() == "john!jdoe@example.com");
	std::cout << "✓ test_client_fullmask passed" << std::endl;
}

void test_client_fullmask_empty() {
	Client client;
	assert(client.fullmask() == "!@");
	std::cout << "✓ test_client_fullmask_empty passed" << std::endl;
}

// ============== REGISTRATION STATE TESTS ==============

void test_client_pass_received() {
	Client client;
	assert(!client.isPassReceived());
	client.passReceived(true);
	assert(client.isPassReceived());
	client.passReceived(false);
	assert(!client.isPassReceived());
	std::cout << "✓ test_client_pass_received passed" << std::endl;
}

void test_client_nick_received() {
	Client client;
	assert(!client.isNickReceived());
	client.nickReceived(true);
	assert(client.isNickReceived());
	client.nickReceived(false);
	assert(!client.isNickReceived());
	std::cout << "✓ test_client_nick_received passed" << std::endl;
}

void test_client_user_received() {
	Client client;
	assert(!client.isUserReceived());
	client.userReceived(true);
	assert(client.isUserReceived());
	client.userReceived(false);
	assert(!client.isUserReceived());
	std::cout << "✓ test_client_user_received passed" << std::endl;
}

void test_client_registered() {
	Client client;
	assert(!client.isRegistered());
	client.registered(true);
	assert(client.isRegistered());
	client.registered(false);
	assert(!client.isRegistered());
	std::cout << "✓ test_client_registered passed" << std::endl;
}

void test_client_registration_sequence() {
	Client client;
	client.nickname("user1").username("test").realname("Test User");
	
	// Simulate registration sequence
	client.passReceived(true);
	assert(client.isPassReceived());
	assert(!client.isRegistered());
	
	client.nickReceived(true);
	assert(client.isNickReceived());
	assert(!client.isRegistered());
	
	client.userReceived(true);
	assert(client.isUserReceived());
	
	// Normally server would set this after PASS, NICK, USER complete
	client.registered(true);
	assert(client.isRegistered());
	std::cout << "✓ test_client_registration_sequence passed" << std::endl;
}

// ============== PING/PONG TIMESTAMP TESTS ==============

void test_client_ping_timestamp() {
	Client client;
	time_t now = time(NULL);
	client.lastPing(now);
	assert(client.lastPing() == now);
	std::cout << "✓ test_client_ping_timestamp passed" << std::endl;
}

void test_client_pong_timestamp() {
	Client client;
	time_t now = time(NULL);
	client.lastPong(now);
	assert(client.lastPong() == now);
	std::cout << "✓ test_client_pong_timestamp passed" << std::endl;
}

void test_client_ping_pong_difference() {
	Client client;
	time_t ping = 1000;
	time_t pong = 1005;
	client.lastPing(ping);
	client.lastPong(pong);
	assert(client.lastPong() - client.lastPing() == 5);
	std::cout << "✓ test_client_ping_pong_difference passed" << std::endl;
}

// ============== MODE TESTS ==============

void test_client_add_mode() {
	Client client;
	assert(!client.hasMode('i'));
	client.addMode('i');
	assert(client.hasMode('i'));
	std::cout << "✓ test_client_add_mode passed" << std::endl;
}

void test_client_remove_mode() {
	Client client;
	client.addMode('o');
	assert(client.hasMode('o'));
	client.removeMode('o');
	assert(!client.hasMode('o'));
	std::cout << "✓ test_client_remove_mode passed" << std::endl;
}

void test_client_multiple_modes() {
	Client client;
	client.addMode('i');
	client.addMode('o');
	client.addMode('w');
	
	assert(client.hasMode('i'));
	assert(client.hasMode('o'));
	assert(client.hasMode('w'));
	assert(!client.hasMode('a'));
	std::cout << "✓ test_client_multiple_modes passed" << std::endl;
}

void test_client_remove_nonexistent_mode() {
	Client client;
	// Should not crash
	client.removeMode('z');
	assert(!client.hasMode('z'));
	std::cout << "✓ test_client_remove_nonexistent_mode passed" << std::endl;
}

void test_client_duplicate_mode() {
	Client client;
	client.addMode('i');
	client.addMode('i'); // Adding again
	assert(client.hasMode('i'));
	std::cout << "✓ test_client_duplicate_mode passed" << std::endl;
}

// ============== BUFFER TESTS ==============

void test_client_read_buffer_empty() {
	Client client;
	assert(client.readBuffer() == "");
	assert(client.readBuffer().empty());
	std::cout << "✓ test_client_read_buffer_empty passed" << std::endl;
}

void test_client_append_to_read_buffer() {
	Client client;
	client.appendToReadBuffer("NICK john\r\n");
	assert(client.readBuffer() == "NICK john\r\n");
	
	client.appendToReadBuffer("USER john 0 * :John Doe\r\n");
	assert(client.readBuffer() == "NICK john\r\nUSER john 0 * :John Doe\r\n");
	std::cout << "✓ test_client_append_to_read_buffer passed" << std::endl;
}

void test_client_clear_read_buffer() {
	Client client;
	client.appendToReadBuffer("Some data");
	assert(!client.readBuffer().empty());
	
	client.clearReadBuffer();
	assert(client.readBuffer().empty());
	std::cout << "✓ test_client_clear_read_buffer passed" << std::endl;
}

void test_client_write_buffer_empty() {
	Client client;
	assert(client.writeBuffer() == "");
	assert(client.writeBuffer().empty());
	std::cout << "✓ test_client_write_buffer_empty passed" << std::endl;
}

void test_client_append_to_write_buffer() {
	Client client;
	client.appendToWriteBuffer(":server 001 john :Welcome\r\n");
	assert(client.writeBuffer() == ":server 001 john :Welcome\r\n");
	
	client.appendToWriteBuffer(":server 002 john :Your host\r\n");
	assert(client.writeBuffer() == ":server 001 john :Welcome\r\n:server 002 john :Your host\r\n");
	std::cout << "✓ test_client_append_to_write_buffer passed" << std::endl;
}

void test_client_clear_write_buffer() {
	Client client;
	client.appendToWriteBuffer("Response data");
	assert(!client.writeBuffer().empty());
	
	client.clearWriteBuffer();
	assert(client.writeBuffer().empty());
	std::cout << "✓ test_client_clear_write_buffer passed" << std::endl;
}

void test_client_buffer_modification_via_reference() {
	Client client;
	std::string& readBuf = client.readBuffer();
	readBuf = "Modified directly";
	assert(client.readBuffer() == "Modified directly");
	
	std::string& writeBuf = client.writeBuffer();
	writeBuf = "Direct write";
	assert(client.writeBuffer() == "Direct write");
	std::cout << "✓ test_client_buffer_modification_via_reference passed" << std::endl;
}

void test_client_partial_buffer_processing() {
	Client client;
	client.appendToReadBuffer("NICK john\r\nUSER john 0 * :John\r\nPING :server\r\n");
	
	// Simulate processing first command
	std::string& buf = client.readBuffer();
	size_t pos = buf.find("\r\n");
	std::string cmd = buf.substr(0, pos);
	buf.erase(0, pos + 2);
	
	assert(cmd == "NICK john");
	assert(client.readBuffer() == "USER john 0 * :John\r\nPING :server\r\n");
	std::cout << "✓ test_client_partial_buffer_processing passed" << std::endl;
}

// ============== CHANNEL MEMBERSHIP TESTS ==============

void test_client_join_channel() {
	Client client;
	assert(!client.isInChannel("#general"));
	client.joinChannel("#general");
	assert(client.isInChannel("#general"));
	std::cout << "✓ test_client_join_channel passed" << std::endl;
}

void test_client_leave_channel() {
	Client client;
	client.joinChannel("#dev");
	assert(client.isInChannel("#dev"));
	
	client.leaveChannel("#dev");
	assert(!client.isInChannel("#dev"));
	std::cout << "✓ test_client_leave_channel passed" << std::endl;
}

void test_client_multiple_channels() {
	Client client;
	client.joinChannel("#general");
	client.joinChannel("#dev");
	client.joinChannel("#random");
	
	assert(client.isInChannel("#general"));
	assert(client.isInChannel("#dev"));
	assert(client.isInChannel("#random"));
	
	client.leaveChannel("#dev");
	assert(!client.isInChannel("#dev"));
	assert(client.isInChannel("#general"));
	assert(client.isInChannel("#random"));
	std::cout << "✓ test_client_multiple_channels passed" << std::endl;
}

void test_client_isInChannel_nonexistent() {
	Client client;
	assert(!client.isInChannel("#nonexistent"));
	std::cout << "✓ test_client_isInChannel_nonexistent passed" << std::endl;
}

void test_client_leave_channel_not_joined() {
	Client client;
	// Should not crash
	client.leaveChannel("#neverjoined");
	assert(!client.isInChannel("#neverjoined"));
	std::cout << "✓ test_client_leave_channel_not_joined passed" << std::endl;
}

void test_client_rejoin_channel() {
	Client client;
	client.joinChannel("#test");
	client.leaveChannel("#test");
	client.joinChannel("#test");
	assert(client.isInChannel("#test"));
	std::cout << "✓ test_client_rejoin_channel passed" << std::endl;
}

void test_client_duplicate_join() {
	Client client;
	client.joinChannel("#dup");
	client.joinChannel("#dup");
	assert(client.isInChannel("#dup"));
	std::cout << "✓ test_client_duplicate_join passed" << std::endl;
}

// ============== EDGE CASE TESTS ==============

void test_client_empty_strings() {
	Client client;
	client.nickname("").username("").realname("").hostname("");
	assert(client.nickname() == "");
	assert(client.username() == "");
	assert(client.realname() == "");
	assert(client.hostname() == "");
	assert(client.fullmask() == "!@");
	std::cout << "✓ test_client_empty_strings passed" << std::endl;
}

void test_client_special_characters() {
	Client client;
	client.nickname("user|special").username("u-s_e.r").hostname("host-name.com");
	assert(client.nickname() == "user|special");
	assert(client.username() == "u-s_e.r");
	assert(client.hostname() == "host-name.com");
	std::cout << "✓ test_client_special_characters passed" << std::endl;
}

void test_client_long_strings() {
	Client client;
	std::string longNick(500, 'a');
	client.nickname(longNick);
	assert(client.nickname() == longNick);
	std::cout << "✓ test_client_long_strings passed" << std::endl;
}

void test_client_negative_fd() {
	Client client;
	client.fd(-1);
	assert(client.fd() == -1);
	std::cout << "✓ test_client_negative_fd passed" << std::endl;
}

void test_client_large_fd() {
	Client client;
	client.fd(999999);
	assert(client.fd() == 999999);
	std::cout << "✓ test_client_large_fd passed" << std::endl;
}

void test_client_timestamp_edge_cases() {
	Client client;
	client.lastPing(0);
	client.lastPong(0);
	assert(client.lastPing() == 0);
	assert(client.lastPong() == 0);
	
	time_t max = 2147483647; // Max 32-bit time_t
	client.lastPing(max);
	client.lastPong(max);
	assert(client.lastPing() == max);
	assert(client.lastPong() == max);
	std::cout << "✓ test_client_timestamp_edge_cases passed" << std::endl;
}

// ============== COMPLEX SCENARIO TESTS ==============

void test_client_full_lifecycle() {
	Client client(42);
	
	// Connection setup
	client.hostname("192.168.1.100");
	client.epollServerFd(10);
	
	// Registration
	client.appendToReadBuffer("PASS secret\r\n");
	client.passReceived(true);
	client.clearReadBuffer();
	
	client.appendToReadBuffer("NICK alice\r\n");
	client.nickname("alice");
	client.nickReceived(true);
	client.clearReadBuffer();
	
	client.appendToReadBuffer("USER alice 0 * :Alice Smith\r\n");
	client.username("alice").realname("Alice Smith");
	client.userReceived(true);
	client.clearReadBuffer();
	
	client.registered(true);
	
	// Add modes
	client.addMode('i');
	client.addMode('w');
	
	// Join channels
	client.joinChannel("#general");
	client.joinChannel("#dev");
	
	// Ping/Pong
	time_t now = time(NULL);
	client.lastPing(now);
	client.lastPong(now + 1);
	
	// Verify state
	assert(client.fd() == 42);
	assert(client.nickname() == "alice");
	assert(client.username() == "alice");
	assert(client.realname() == "Alice Smith");
	assert(client.isRegistered());
	assert(client.hasMode('i'));
	assert(client.hasMode('w'));
	assert(client.isInChannel("#general"));
	assert(client.isInChannel("#dev"));
	assert(client.fullmask() == "alice!alice@192.168.1.100");
	
	std::cout << "✓ test_client_full_lifecycle passed" << std::endl;
}

void test_client_disconnect_cleanup() {
	Client client(50);
	
	client.nickname("user1");
	client.joinChannel("#ch1");
	client.joinChannel("#ch2");
	client.appendToReadBuffer("incomplete data");
	client.appendToWriteBuffer("pending output");
	
	// Simulate cleanup before disconnect
	client.leaveChannel("#ch1");
	client.leaveChannel("#ch2");
	client.clearReadBuffer();
	client.clearWriteBuffer();
	
	assert(!client.isInChannel("#ch1"));
	assert(!client.isInChannel("#ch2"));
	assert(client.readBuffer().empty());
	assert(client.writeBuffer().empty());
	
	std::cout << "✓ test_client_disconnect_cleanup passed" << std::endl;
}

// ============== TEST RUNNER ==============

int run_client_tests() {
	std::cout << "\n========== CLIENT TESTS ==========" << std::endl;
	
	std::cout << "\n--- Constructor Tests ---" << std::endl;
	test_client_default_constructor();
	test_client_fd_constructor();
	
	std::cout << "\n--- Getter/Setter Tests ---" << std::endl;
	test_client_realname_getter_setter();
	test_client_username_getter_setter();
	test_client_nickname_getter_setter();
	test_client_hostname_getter_setter();
	test_client_fd_getter_setter();
	test_client_epoll_fd_getter_setter();
	test_client_setters_chaining();
	
	std::cout << "\n--- Fullmask Tests ---" << std::endl;
	test_client_fullmask();
	test_client_fullmask_empty();
	
	std::cout << "\n--- Registration State Tests ---" << std::endl;
	test_client_pass_received();
	test_client_nick_received();
	test_client_user_received();
	test_client_registered();
	test_client_registration_sequence();
	
	std::cout << "\n--- Ping/Pong Timestamp Tests ---" << std::endl;
	test_client_ping_timestamp();
	test_client_pong_timestamp();
	test_client_ping_pong_difference();
	
	std::cout << "\n--- Mode Tests ---" << std::endl;
	test_client_add_mode();
	test_client_remove_mode();
	test_client_multiple_modes();
	test_client_remove_nonexistent_mode();
	test_client_duplicate_mode();
	
	std::cout << "\n--- Buffer Tests ---" << std::endl;
	test_client_read_buffer_empty();
	test_client_append_to_read_buffer();
	test_client_clear_read_buffer();
	test_client_write_buffer_empty();
	test_client_append_to_write_buffer();
	test_client_clear_write_buffer();
	test_client_buffer_modification_via_reference();
	test_client_partial_buffer_processing();
	
	std::cout << "\n--- Channel Membership Tests ---" << std::endl;
	test_client_join_channel();
	test_client_leave_channel();
	test_client_multiple_channels();
	test_client_isInChannel_nonexistent();
	test_client_leave_channel_not_joined();
	test_client_rejoin_channel();
	test_client_duplicate_join();
	
	std::cout << "\n--- Edge Case Tests ---" << std::endl;
	test_client_empty_strings();
	test_client_special_characters();
	test_client_long_strings();
	test_client_negative_fd();
	test_client_large_fd();
	test_client_timestamp_edge_cases();
	
	std::cout << "\n--- Complex Scenario Tests ---" << std::endl;
	test_client_full_lifecycle();
	test_client_disconnect_cleanup();
	
	std::cout << "========== ALL CLIENT TESTS PASSED ==========" << std::endl;
	return 0;
}
