#pragma once

#include <string>
#include <iostream>

#include <set>

class Channel {
private:
	std::string _name;
	std::string _topic;
	std::string _topicSetter;
	time_t _topicSetTime;
	
	std::string _key; // +k
	size_t _limit; // +l

	std::set<char> _modes; // Channel modes

	std::set<std::string> _members;
	std::set<std::string> _operators;
	std::set<std::string> _invited;

	Channel(const Channel& other);
	Channel& operator=(const Channel& other);
public:
	Channel();
	Channel(const std::string& name);
	~Channel();

	// Getters
	std::string name() const;
	std::string topic() const;
	std::string topicSetter() const;
	time_t topicSetTime() const;
	std::string key() const;
	size_t limit() const;

	// Setters
	Channel& name(const std::string& name);
	Channel& topic(const std::string& topic);
	Channel& topicSetter(const std::string& topicSetter);
	Channel& topicSetTime(time_t topicSetTime);
	Channel& key(const std::string& key);
	Channel& limit(size_t limit);

	// Mode operations
	void addMode(char mode);
	void removeMode(char mode);
	bool hasMode(char mode) const;

	bool isInviteOnly() const;
	bool isTopicRestricted() const;
	bool isKeyMode() const;
	bool isLimitMode() const;

	// Members operations
	void addMember(const std::string& nickname);
	void removeMember(const std::string& nickname);
	bool hasMember(const std::string& nickname) const;

	// Operators operations
	void addOperator(const std::string& nickname);
	void removeOperator(const std::string& nickname);
	bool hasOperator(const std::string& nickname) const;

	// Invited operations
	void addInvited(const std::string& nickname);
	void removeInvited(const std::string& nickname);
	bool hasInvited(const std::string& nickname) const;

	friend std::ostream& operator<<(std::ostream& os, const Channel& channel);
};