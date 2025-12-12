#include "../include/Channel.hpp"

Channel::Channel() :
	_name(""),
	_topic(""),
	_topicSetter(""),
	_topicSetTime(0),
	_key(""),
	_limit(0),
	_modes(),
	_members(),
	_operators(),
	_invited()
{}

Channel::Channel(const std::string& name) :
	_name(name),
	_topic(""),
	_topicSetter(""),
	_topicSetTime(0),
	_key(""),
	_limit(0),
	_modes(),
	_members(),
	_operators(),
	_invited()
{}

Channel::~Channel() {}

// Getters
std::string Channel::name() const { return _name; }
std::string Channel::topic() const { return _topic; }
std::string Channel::topicSetter() const { return _topicSetter; }
time_t Channel::topicSetTime() const { return _topicSetTime; }
std::string Channel::key() const { return _key; }
size_t Channel::limit() const { return _limit; }

// Setters
Channel& Channel::name(const std::string& name) { _name = name; return *this; }
Channel& Channel::topic(const std::string& topic) { _topic = topic; return *this; }
Channel& Channel::topicSetter(const std::string& topicSetter) { _topicSetter = topicSetter; return *this; }
Channel& Channel::topicSetTime(time_t topicSetTime) { _topicSetTime = topicSetTime; return *this; }
Channel& Channel::key(const std::string& key) { _key = key; return *this; }
Channel& Channel::limit(size_t limit) { _limit = limit; return *this; }

// Mode operations
void Channel::addMode(char mode) { _modes.insert(mode); }
void Channel::removeMode(char mode) {
	if (mode == 'k') _key = "";
	if (mode == 'l') _limit = 0;
	_modes.erase(mode);
}
bool Channel::hasMode(char mode) const {
	return _modes.find(mode) != _modes.end();
}

bool Channel::isInviteOnly() const { return hasMode('i'); }
bool Channel::isTopicRestricted() const { return hasMode('t'); }
bool Channel::isKeyMode() const { return hasMode('k'); }
bool Channel::isLimitMode() const { return hasMode('l'); }

// Members operations
void Channel::addMember(const std::string& nickname) { _members.insert(nickname); }
void Channel::removeMember(const std::string& nickname) { _members.erase(nickname); }
bool Channel::hasMember(const std::string& nickname) const {
	return _members.find(nickname) != _members.end();
}

// Operators operations
void Channel::addOperator(const std::string& nickname) { _operators.insert(nickname); }
void Channel::removeOperator(const std::string& nickname) { _operators.erase(nickname); }
bool Channel::hasOperator(const std::string& nickname) const {
	return _operators.find(nickname) != _operators.end();
}

// Invited operations
void Channel::addInvited(const std::string& nickname) { _invited.insert(nickname); }
void Channel::removeInvited(const std::string& nickname) { _invited.erase(nickname); }
bool Channel::hasInvited(const std::string& nickname) const {
	return _invited.find(nickname) != _invited.end();
}

std::ostream& operator<<(std::ostream& os, const Channel& channel) {
	os  << "Channel: " << channel._name
		<< ", Topic: " << channel._topic
		<< ", TopicSetter: " << channel._topicSetter
		<< ", TopicSetTime: " << channel._topicSetTime
		<< ", Key: " << channel._key
		<< ", Limit: " << channel._limit
		<< ", Modes: ";
	for (std::set<char>::const_iterator it = channel._modes.begin(); it != channel._modes.end(); ++it) {
		os << *it;
	}
	os << ", Members: ";
	for (std::set<std::string>::const_iterator it = channel._members.begin(); it != channel._members.end(); ++it) {
		os << *it << " ";
	}
	os << ", Operators: ";
	for (std::set<std::string>::const_iterator it = channel._operators.begin(); it != channel._operators.end(); ++it) {
		os << *it << " ";
	}
	os << ", Invited: ";
	for (std::set<std::string>::const_iterator it = channel._invited.begin(); it != channel._invited.end(); ++it) {
		os << *it << " ";
	}
	return os;
}