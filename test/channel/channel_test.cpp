#include <iostream>
#include <cassert>
#include <string>
#include <ctime>
#include "../../include/Channel.hpp"

// ============== CONSTRUCTOR TESTS ==============

void test_channel_default_constructor() {
	Channel channel;
	assert(channel.name() == "");
	assert(channel.topic() == "");
	assert(channel.topicSetter() == "");
	assert(channel.topicSetTime() == 0);
	assert(channel.key() == "");
	assert(channel.limit() == 0);
	assert(!channel.isInviteOnly());
	assert(!channel.isTopicRestricted());
	assert(!channel.isKeyMode());
	assert(!channel.isLimitMode());
	std::cout << "✓ test_channel_default_constructor passed" << std::endl;
}

void test_channel_name_constructor() {
	Channel channel("#general");
	assert(channel.name() == "#general");
	assert(channel.topic() == "");
	assert(channel.limit() == 0);
	std::cout << "✓ test_channel_name_constructor passed" << std::endl;
}

// ============== GETTER/SETTER TESTS ==============

void test_channel_name_getter_setter() {
	Channel channel;
	channel.name("#test");
	assert(channel.name() == "#test");
	std::cout << "✓ test_channel_name_getter_setter passed" << std::endl;
}

void test_channel_topic_getter_setter() {
	Channel channel;
	channel.topic("Welcome to the channel");
	assert(channel.topic() == "Welcome to the channel");
	std::cout << "✓ test_channel_topic_getter_setter passed" << std::endl;
}

void test_channel_topic_setter_getter_setter() {
	Channel channel;
	channel.topicSetter("admin");
	assert(channel.topicSetter() == "admin");
	std::cout << "✓ test_channel_topic_setter_getter_setter passed" << std::endl;
}

void test_channel_topic_set_time_getter_setter() {
	Channel channel;
	time_t now = time(NULL);
	channel.topicSetTime(now);
	assert(channel.topicSetTime() == now);
	std::cout << "✓ test_channel_topic_set_time_getter_setter passed" << std::endl;
}

void test_channel_key_getter_setter() {
	Channel channel;
	channel.key("secret123");
	assert(channel.key() == "secret123");
	std::cout << "✓ test_channel_key_getter_setter passed" << std::endl;
}

void test_channel_limit_getter_setter() {
	Channel channel;
	channel.limit(50);
	assert(channel.limit() == 50);
	std::cout << "✓ test_channel_limit_getter_setter passed" << std::endl;
}

void test_channel_setters_chaining() {
	Channel channel;
	time_t now = time(NULL);
	channel.name("#dev").topic("Dev channel").topicSetter("admin").topicSetTime(now).key("pass").limit(100);
	
	assert(channel.name() == "#dev");
	assert(channel.topic() == "Dev channel");
	assert(channel.topicSetter() == "admin");
	assert(channel.topicSetTime() == now);
	assert(channel.key() == "pass");
	assert(channel.limit() == 100);
	std::cout << "✓ test_channel_setters_chaining passed" << std::endl;
}

// ============== MODE TESTS ==============

void test_channel_add_mode() {
	Channel channel;
	assert(!channel.hasMode('i'));
	channel.addMode('i');
	assert(channel.hasMode('i'));
	std::cout << "✓ test_channel_add_mode passed" << std::endl;
}

void test_channel_remove_mode() {
	Channel channel;
	channel.addMode('t');
	assert(channel.hasMode('t'));
	channel.removeMode('t');
	assert(!channel.hasMode('t'));
	std::cout << "✓ test_channel_remove_mode passed" << std::endl;
}

void test_channel_multiple_modes() {
	Channel channel;
	channel.addMode('i');
	channel.addMode('t');
	channel.addMode('k');
	channel.addMode('l');
	
	assert(channel.hasMode('i'));
	assert(channel.hasMode('t'));
	assert(channel.hasMode('k'));
	assert(channel.hasMode('l'));
	std::cout << "✓ test_channel_multiple_modes passed" << std::endl;
}

void test_channel_invite_only_mode() {
	Channel channel;
	assert(!channel.isInviteOnly());
	channel.addMode('i');
	assert(channel.isInviteOnly());
	channel.removeMode('i');
	assert(!channel.isInviteOnly());
	std::cout << "✓ test_channel_invite_only_mode passed" << std::endl;
}

void test_channel_topic_restricted_mode() {
	Channel channel;
	assert(!channel.isTopicRestricted());
	channel.addMode('t');
	assert(channel.isTopicRestricted());
	channel.removeMode('t');
	assert(!channel.isTopicRestricted());
	std::cout << "✓ test_channel_topic_restricted_mode passed" << std::endl;
}

void test_channel_key_mode() {
	Channel channel;
	assert(!channel.isKeyMode());
	channel.addMode('k');
	channel.key("mykey");
	assert(channel.isKeyMode());
	assert(channel.key() == "mykey");
	
	// Removing +k should clear the key
	channel.removeMode('k');
	assert(!channel.isKeyMode());
	assert(channel.key() == "");
	std::cout << "✓ test_channel_key_mode passed" << std::endl;
}

void test_channel_limit_mode() {
	Channel channel;
	assert(!channel.isLimitMode());
	channel.addMode('l');
	channel.limit(25);
	assert(channel.isLimitMode());
	assert(channel.limit() == 25);
	
	// Removing +l should clear the limit
	channel.removeMode('l');
	assert(!channel.isLimitMode());
	assert(channel.limit() == 0);
	std::cout << "✓ test_channel_limit_mode passed" << std::endl;
}

void test_channel_duplicate_mode() {
	Channel channel;
	channel.addMode('i');
	channel.addMode('i'); // Adding again
	assert(channel.hasMode('i'));
	std::cout << "✓ test_channel_duplicate_mode passed" << std::endl;
}

void test_channel_remove_nonexistent_mode() {
	Channel channel;
	// Should not crash
	channel.removeMode('z');
	assert(!channel.hasMode('z'));
	std::cout << "✓ test_channel_remove_nonexistent_mode passed" << std::endl;
}

// ============== MEMBER TESTS ==============

void test_channel_add_member() {
	Channel channel;
	assert(!channel.hasMember("alice"));
	channel.addMember("alice");
	assert(channel.hasMember("alice"));
	std::cout << "✓ test_channel_add_member passed" << std::endl;
}

void test_channel_remove_member() {
	Channel channel;
	channel.addMember("bob");
	assert(channel.hasMember("bob"));
	channel.removeMember("bob");
	assert(!channel.hasMember("bob"));
	std::cout << "✓ test_channel_remove_member passed" << std::endl;
}

void test_channel_multiple_members() {
	Channel channel;
	channel.addMember("alice");
	channel.addMember("bob");
	channel.addMember("carol");
	
	assert(channel.hasMember("alice"));
	assert(channel.hasMember("bob"));
	assert(channel.hasMember("carol"));
	
	channel.removeMember("bob");
	assert(!channel.hasMember("bob"));
	assert(channel.hasMember("alice"));
	assert(channel.hasMember("carol"));
	std::cout << "✓ test_channel_multiple_members passed" << std::endl;
}

void test_channel_duplicate_member() {
	Channel channel;
	channel.addMember("user1");
	channel.addMember("user1"); // Adding again
	assert(channel.hasMember("user1"));
	std::cout << "✓ test_channel_duplicate_member passed" << std::endl;
}

void test_channel_remove_nonexistent_member() {
	Channel channel;
	// Should not crash
	channel.removeMember("nonexistent");
	assert(!channel.hasMember("nonexistent"));
	std::cout << "✓ test_channel_remove_nonexistent_member passed" << std::endl;
}

// ============== OPERATOR TESTS ==============

void test_channel_add_operator() {
	Channel channel;
	assert(!channel.hasOperator("admin"));
	channel.addOperator("admin");
	assert(channel.hasOperator("admin"));
	std::cout << "✓ test_channel_add_operator passed" << std::endl;
}

void test_channel_remove_operator() {
	Channel channel;
	channel.addOperator("op1");
	assert(channel.hasOperator("op1"));
	channel.removeOperator("op1");
	assert(!channel.hasOperator("op1"));
	std::cout << "✓ test_channel_remove_operator passed" << std::endl;
}

void test_channel_multiple_operators() {
	Channel channel;
	channel.addOperator("op1");
	channel.addOperator("op2");
	channel.addOperator("op3");
	
	assert(channel.hasOperator("op1"));
	assert(channel.hasOperator("op2"));
	assert(channel.hasOperator("op3"));
	
	channel.removeOperator("op2");
	assert(!channel.hasOperator("op2"));
	assert(channel.hasOperator("op1"));
	assert(channel.hasOperator("op3"));
	std::cout << "✓ test_channel_multiple_operators passed" << std::endl;
}

void test_channel_duplicate_operator() {
	Channel channel;
	channel.addOperator("op1");
	channel.addOperator("op1"); // Adding again
	assert(channel.hasOperator("op1"));
	std::cout << "✓ test_channel_duplicate_operator passed" << std::endl;
}

void test_channel_member_and_operator() {
	Channel channel;
	channel.addMember("alice");
	channel.addOperator("alice");
	
	assert(channel.hasMember("alice"));
	assert(channel.hasOperator("alice"));
	
	// Remove as operator but keep as member
	channel.removeOperator("alice");
	assert(channel.hasMember("alice"));
	assert(!channel.hasOperator("alice"));
	std::cout << "✓ test_channel_member_and_operator passed" << std::endl;
}

// ============== INVITED TESTS ==============

void test_channel_add_invited() {
	Channel channel;
	assert(!channel.hasInvited("guest"));
	channel.addInvited("guest");
	assert(channel.hasInvited("guest"));
	std::cout << "✓ test_channel_add_invited passed" << std::endl;
}

void test_channel_remove_invited() {
	Channel channel;
	channel.addInvited("guest1");
	assert(channel.hasInvited("guest1"));
	channel.removeInvited("guest1");
	assert(!channel.hasInvited("guest1"));
	std::cout << "✓ test_channel_remove_invited passed" << std::endl;
}

void test_channel_multiple_invited() {
	Channel channel;
	channel.addInvited("user1");
	channel.addInvited("user2");
	channel.addInvited("user3");
	
	assert(channel.hasInvited("user1"));
	assert(channel.hasInvited("user2"));
	assert(channel.hasInvited("user3"));
	
	channel.removeInvited("user2");
	assert(!channel.hasInvited("user2"));
	assert(channel.hasInvited("user1"));
	assert(channel.hasInvited("user3"));
	std::cout << "✓ test_channel_multiple_invited passed" << std::endl;
}

void test_channel_duplicate_invited() {
	Channel channel;
	channel.addInvited("inv1");
	channel.addInvited("inv1"); // Adding again
	assert(channel.hasInvited("inv1"));
	std::cout << "✓ test_channel_duplicate_invited passed" << std::endl;
}

void test_channel_invited_becomes_member() {
	Channel channel;
	channel.addMode('i'); // Invite-only
	channel.addInvited("guest");
	assert(channel.hasInvited("guest"));
	
	// Guest joins
	channel.addMember("guest");
	
	// Remove from invite list after joining
	channel.removeInvited("guest");
	assert(channel.hasMember("guest"));
	assert(!channel.hasInvited("guest"));
	std::cout << "✓ test_channel_invited_becomes_member passed" << std::endl;
}

// ============== TOPIC TESTS ==============

void test_channel_topic_update() {
	Channel channel;
	time_t time1 = 1000;
	time_t time2 = 2000;
	
	channel.topic("First topic").topicSetter("alice").topicSetTime(time1);
	assert(channel.topic() == "First topic");
	assert(channel.topicSetter() == "alice");
	assert(channel.topicSetTime() == time1);
	
	// Update topic
	channel.topic("Second topic").topicSetter("bob").topicSetTime(time2);
	assert(channel.topic() == "Second topic");
	assert(channel.topicSetter() == "bob");
	assert(channel.topicSetTime() == time2);
	std::cout << "✓ test_channel_topic_update passed" << std::endl;
}

void test_channel_topic_restricted_workflow() {
	Channel channel;
	channel.name("#test");
	
	// Initially anyone can set topic
	assert(!channel.isTopicRestricted());
	channel.topic("Open topic").topicSetter("anyone");
	
	// Enable topic restriction
	channel.addMode('t');
	assert(channel.isTopicRestricted());
	
	// Now only operators should set (this is logic test, not enforced here)
	channel.topic("Restricted topic").topicSetter("operator");
	assert(channel.topic() == "Restricted topic");
	std::cout << "✓ test_channel_topic_restricted_workflow passed" << std::endl;
}

void test_channel_empty_topic() {
	Channel channel;
	channel.topic("");
	assert(channel.topic() == "");
	std::cout << "✓ test_channel_empty_topic passed" << std::endl;
}

void test_channel_long_topic() {
	Channel channel;
	std::string longTopic(1000, 'a');
	channel.topic(longTopic);
	assert(channel.topic() == longTopic);
	std::cout << "✓ test_channel_long_topic passed" << std::endl;
}

// ============== KEY AND LIMIT TESTS ==============

void test_channel_key_workflow() {
	Channel channel;
	channel.name("#secret");
	
	// Set key
	channel.addMode('k');
	channel.key("password123");
	assert(channel.isKeyMode());
	assert(channel.key() == "password123");
	
	// Change key
	channel.key("newpassword");
	assert(channel.key() == "newpassword");
	
	// Remove key
	channel.removeMode('k');
	assert(!channel.isKeyMode());
	assert(channel.key() == "");
	std::cout << "✓ test_channel_key_workflow passed" << std::endl;
}

void test_channel_limit_workflow() {
	Channel channel;
	channel.name("#limited");
	
	// Set limit
	channel.addMode('l');
	channel.limit(10);
	assert(channel.isLimitMode());
	assert(channel.limit() == 10);
	
	// Change limit
	channel.limit(20);
	assert(channel.limit() == 20);
	
	// Remove limit
	channel.removeMode('l');
	assert(!channel.isLimitMode());
	assert(channel.limit() == 0);
	std::cout << "✓ test_channel_limit_workflow passed" << std::endl;
}

void test_channel_zero_limit() {
	Channel channel;
	channel.limit(0);
	assert(channel.limit() == 0);
	std::cout << "✓ test_channel_zero_limit passed" << std::endl;
}

void test_channel_large_limit() {
	Channel channel;
	channel.limit(999999);
	assert(channel.limit() == 999999);
	std::cout << "✓ test_channel_large_limit passed" << std::endl;
}

// ============== EDGE CASE TESTS ==============

void test_channel_empty_name() {
	Channel channel;
	channel.name("");
	assert(channel.name() == "");
	std::cout << "✓ test_channel_empty_name passed" << std::endl;
}

void test_channel_special_characters_in_name() {
	Channel channel;
	channel.name("#test-channel_123");
	assert(channel.name() == "#test-channel_123");
	std::cout << "✓ test_channel_special_characters_in_name passed" << std::endl;
}

void test_channel_empty_key() {
	Channel channel;
	channel.key("");
	assert(channel.key() == "");
	std::cout << "✓ test_channel_empty_key passed" << std::endl;
}

void test_channel_special_characters_in_key() {
	Channel channel;
	channel.key("p@ss!w0rd#123");
	assert(channel.key() == "p@ss!w0rd#123");
	std::cout << "✓ test_channel_special_characters_in_key passed" << std::endl;
}

// ============== COMPLEX SCENARIO TESTS ==============

void test_channel_full_lifecycle() {
	Channel channel("#general");
	time_t now = time(NULL);
	
	// Set up channel
	channel.topic("Welcome!").topicSetter("admin").topicSetTime(now);
	channel.addMode('t'); // Topic restricted
	channel.addMode('i'); // Invite only
	
	// Add operator (channel creator)
	channel.addMember("admin");
	channel.addOperator("admin");
	
	// Invite users
	channel.addInvited("alice");
	channel.addInvited("bob");
	
	// Users join
	channel.addMember("alice");
	channel.addMember("bob");
	channel.removeInvited("alice");
	channel.removeInvited("bob");
	
	// Promote alice to operator
	channel.addOperator("alice");
	
	// Verify state
	assert(channel.name() == "#general");
	assert(channel.topic() == "Welcome!");
	assert(channel.topicSetter() == "admin");
	assert(channel.isTopicRestricted());
	assert(channel.isInviteOnly());
	assert(channel.hasMember("admin"));
	assert(channel.hasMember("alice"));
	assert(channel.hasMember("bob"));
	assert(channel.hasOperator("admin"));
	assert(channel.hasOperator("alice"));
	assert(!channel.hasOperator("bob"));
	assert(!channel.hasInvited("alice"));
	assert(!channel.hasInvited("bob"));
	
	std::cout << "✓ test_channel_full_lifecycle passed" << std::endl;
}

void test_channel_private_with_key_and_limit() {
	Channel channel("#vip");
	
	// Set up restricted channel
	channel.addMode('i'); // Invite only
	channel.addMode('t'); // Topic restricted
	channel.addMode('k'); // Key protected
	channel.addMode('l'); // Limited
	
	channel.key("vippass");
	channel.limit(5);
	
	// Add operator
	channel.addMember("owner");
	channel.addOperator("owner");
	
	// Invite and add members
	for (int i = 1; i <= 4; ++i) {
		std::string user = "user";
		user += ('0' + i);
		channel.addInvited(user);
		channel.addMember(user);
		channel.removeInvited(user);
	}
	
	// Verify
	assert(channel.isInviteOnly());
	assert(channel.isTopicRestricted());
	assert(channel.isKeyMode());
	assert(channel.isLimitMode());
	assert(channel.key() == "vippass");
	assert(channel.limit() == 5);
	assert(channel.hasMember("owner"));
	assert(channel.hasMember("user1"));
	assert(channel.hasMember("user4"));
	assert(channel.hasOperator("owner"));
	
	std::cout << "✓ test_channel_private_with_key_and_limit passed" << std::endl;
}

void test_channel_mode_changes() {
	Channel channel("#dynamic");
	
	// Start public
	assert(!channel.isInviteOnly());
	assert(!channel.isTopicRestricted());
	
	// Make private
	channel.addMode('i');
	channel.addMode('t');
	assert(channel.isInviteOnly());
	assert(channel.isTopicRestricted());
	
	// Back to public
	channel.removeMode('i');
	channel.removeMode('t');
	assert(!channel.isInviteOnly());
	assert(!channel.isTopicRestricted());
	
	std::cout << "✓ test_channel_mode_changes passed" << std::endl;
}

void test_channel_user_lifecycle_in_channel() {
	Channel channel("#test");
	
	// User invited
	channel.addInvited("user1");
	assert(channel.hasInvited("user1"));
	assert(!channel.hasMember("user1"));
	
	// User joins
	channel.addMember("user1");
	channel.removeInvited("user1");
	assert(!channel.hasInvited("user1"));
	assert(channel.hasMember("user1"));
	assert(!channel.hasOperator("user1"));
	
	// User promoted
	channel.addOperator("user1");
	assert(channel.hasOperator("user1"));
	assert(channel.hasMember("user1"));
	
	// User demoted
	channel.removeOperator("user1");
	assert(!channel.hasOperator("user1"));
	assert(channel.hasMember("user1"));
	
	// User leaves
	channel.removeMember("user1");
	assert(!channel.hasMember("user1"));
	assert(!channel.hasOperator("user1"));
	
	std::cout << "✓ test_channel_user_lifecycle_in_channel passed" << std::endl;
}

// ============== TEST RUNNER ==============

int run_channel_tests() {
	std::cout << "\n========== CHANNEL TESTS ==========" << std::endl;
	
	std::cout << "\n--- Constructor Tests ---" << std::endl;
	test_channel_default_constructor();
	test_channel_name_constructor();
	
	std::cout << "\n--- Getter/Setter Tests ---" << std::endl;
	test_channel_name_getter_setter();
	test_channel_topic_getter_setter();
	test_channel_topic_setter_getter_setter();
	test_channel_topic_set_time_getter_setter();
	test_channel_key_getter_setter();
	test_channel_limit_getter_setter();
	test_channel_setters_chaining();
	
	std::cout << "\n--- Mode Tests ---" << std::endl;
	test_channel_add_mode();
	test_channel_remove_mode();
	test_channel_multiple_modes();
	test_channel_invite_only_mode();
	test_channel_topic_restricted_mode();
	test_channel_key_mode();
	test_channel_limit_mode();
	test_channel_duplicate_mode();
	test_channel_remove_nonexistent_mode();
	
	std::cout << "\n--- Member Tests ---" << std::endl;
	test_channel_add_member();
	test_channel_remove_member();
	test_channel_multiple_members();
	test_channel_duplicate_member();
	test_channel_remove_nonexistent_member();
	
	std::cout << "\n--- Operator Tests ---" << std::endl;
	test_channel_add_operator();
	test_channel_remove_operator();
	test_channel_multiple_operators();
	test_channel_duplicate_operator();
	test_channel_member_and_operator();
	
	std::cout << "\n--- Invited Tests ---" << std::endl;
	test_channel_add_invited();
	test_channel_remove_invited();
	test_channel_multiple_invited();
	test_channel_duplicate_invited();
	test_channel_invited_becomes_member();
	
	std::cout << "\n--- Topic Tests ---" << std::endl;
	test_channel_topic_update();
	test_channel_topic_restricted_workflow();
	test_channel_empty_topic();
	test_channel_long_topic();
	
	std::cout << "\n--- Key and Limit Tests ---" << std::endl;
	test_channel_key_workflow();
	test_channel_limit_workflow();
	test_channel_zero_limit();
	test_channel_large_limit();
	
	std::cout << "\n--- Edge Case Tests ---" << std::endl;
	test_channel_empty_name();
	test_channel_special_characters_in_name();
	test_channel_empty_key();
	test_channel_special_characters_in_key();
	
	std::cout << "\n--- Complex Scenario Tests ---" << std::endl;
	test_channel_full_lifecycle();
	test_channel_private_with_key_and_limit();
	test_channel_mode_changes();
	test_channel_user_lifecycle_in_channel();
	
	std::cout << "========== ALL CHANNEL TESTS PASSED ==========" << std::endl;
	return 0;
}
