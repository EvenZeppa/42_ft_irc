/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ezeppa <ezeppa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 14:30:00 by ezeppa            #+#    #+#             */
/*   Updated: 2026/03/03 15:43:19 by vbonnard           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	
	std::string _key;
	size_t _limit;

	std::set<char> _modes;

	std::set<std::string> _members;
	std::set<std::string> _operators;
	std::set<std::string> _invited;

	/**
	 * @brief Disallow copy construction.
	 */
	Channel(const Channel& other);
	/**
	 * @brief Disallow copy assignment.
	 * @return Reference to this instance.
	 */
	Channel& operator=(const Channel& other);
public:
	/** @brief Construct an empty channel instance. */
	Channel();
	/** @brief Construct a named channel. @param name Channel name. */
	Channel(const std::string& name);
	/** @brief Destroy the channel instance. */
	~Channel();

	/** @brief Get channel name. @return Channel name. */
	std::string name() const;
	/** @brief Get current topic. @return Topic text. */
	std::string topic() const;
	/** @brief Get topic setter nickname. @return Nickname that set topic. */
	std::string topicSetter() const;
	/** @brief Get topic set timestamp. @return Unix time value. */
	time_t topicSetTime() const;
	/** @brief Get channel key. @return Channel key string. */
	std::string key() const;
	/** @brief Get user limit. @return Channel user limit. */
	size_t limit() const;

	/** @brief Set channel name. @param name New channel name. @return Reference to this channel. */
	Channel& name(const std::string& name);
	/** @brief Set channel topic. @param topic New topic. @return Reference to this channel. */
	Channel& topic(const std::string& topic);
	/** @brief Set topic setter nickname. @param topicSetter Nickname of setter. @return Reference to this channel. */
	Channel& topicSetter(const std::string& topicSetter);
	/** @brief Set topic timestamp. @param topicSetTime Unix time value. @return Reference to this channel. */
	Channel& topicSetTime(time_t topicSetTime);
	/** @brief Set channel key. @param key New key. @return Reference to this channel. */
	Channel& key(const std::string& key);
	/** @brief Set channel limit. @param limit Maximum members. @return Reference to this channel. */
	Channel& limit(size_t limit);

	/** @brief Enable a channel mode. @param mode Mode flag. */
	void addMode(char mode);
	/** @brief Disable a channel mode. @param mode Mode flag. */
	void removeMode(char mode);
	/** @brief Check a channel mode. @param mode Mode flag. @return True if mode is enabled. */
	bool hasMode(char mode) const;

	/** @brief Check invite-only mode. @return True if mode +i is enabled. */
	bool isInviteOnly() const;
	/** @brief Check topic restriction mode. @return True if mode +t is enabled. */
	bool isTopicRestricted() const;
	/** @brief Check key mode. @return True if mode +k is enabled. */
	bool isKeyMode() const;
	/** @brief Check limit mode. @return True if mode +l is enabled. */
	bool isLimitMode() const;

	/** @brief Add a channel member. @param nickname Nickname to add. */
	void addMember(const std::string& nickname);
	/** @brief Remove a channel member. @param nickname Nickname to remove. */
	void removeMember(const std::string& nickname);
	/** @brief Check member existence. @param nickname Nickname to query. @return True if member exists. */
	bool hasMember(const std::string& nickname) const;

	/** @brief Grant operator status. @param nickname Nickname to promote. */
	void addOperator(const std::string& nickname);
	/** @brief Remove operator status. @param nickname Nickname to demote. */
	void removeOperator(const std::string& nickname);
	/** @brief Check operator status. @param nickname Nickname to query. @return True if operator. */
	bool hasOperator(const std::string& nickname) const;
	/** @brief If no operators remain but channel has members, promote first member to op. @return Promoted nickname or empty if none. */
	std::string promoteFirstMemberIfNoOps();

	/** @brief Add invited user. @param nickname Nickname to invite. */
	void addInvited(const std::string& nickname);
	/** @brief Remove invited user. @param nickname Nickname to remove from invite list. */
	void removeInvited(const std::string& nickname);
	/** @brief Check invite list membership. @param nickname Nickname to query. @return True if invited. */
	bool hasInvited(const std::string& nickname) const;

	/** @brief Stream channel debug information. @param os Output stream. @param channel Channel to serialize. @return Output stream reference. */
	friend std::ostream& operator<<(std::ostream& os, const Channel& channel);
};