/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ezeppa <ezeppa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 00:00:00 by ezeppa            #+#    #+#             */
/*   Updated: 2026/03/03 00:00:00 by vbonnard           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

#include <ctime>
#include <iostream>
#include <sstream>

namespace {
bool g_enabled[4] = {true, true, true, true};

/** @brief Return ANSI color sequence for a log category. @param type Log category. @return ANSI escape string. */
const char* colorFor(Logger::Type type) {
	switch (type) {
		case Logger::IN:
			return "\033[34m";
		case Logger::OUT:
			return "\033[32m";
		case Logger::INFO:
			return "\033[33m";
		case Logger::ERROR:
			return "\033[31m";
	}
	return "\033[33m";
}

/** @brief Return category label text. @param type Log category. @return Static label string. */
const char* typeNameFor(Logger::Type type) {
	switch (type) {
		case Logger::IN:
			return "IN";
		case Logger::OUT:
			return "OUT";
		case Logger::INFO:
			return "INFO";
		case Logger::ERROR:
			return "ERROR";
	}
	return "UNKNOWN";
}

/** @brief Return direction marker for display. @param type Log category. @return Static direction string. */
const char* directionFor(Logger::Type type) {
	switch (type) {
		case Logger::IN:
			return ">>";
		case Logger::OUT:
			return "<<";
		case Logger::INFO:
			return "--";
		case Logger::ERROR:
			return "!!";
	}
	return "--";
}

/** @brief Convert integer to string in C++98 style. @param value Integer value. @return Decimal string form. */
std::string intToString(int value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

/** @brief Build current wall-clock timestamp. @return Formatted time token such as [HH:MM:SS]. */
std::string buildTimestamp() {
	std::time_t now = std::time(NULL);
	std::tm* local = std::localtime(&now);
	char buffer[16];
	if (!local) {
		return "[00:00:00]";
	}
	std::strftime(buffer, sizeof(buffer), "[%H:%M:%S]", local);
	return std::string(buffer);
}

/** @brief Build source identifier for logs. @param fd File descriptor. @param nickname Optional nickname. @return Identifier token. */
std::string buildIdentifier(int fd, const std::string& nickname) {
	if (!nickname.empty()) {
		return "[NICK:" + nickname + "]";
	}
	return "[FD:" + intToString(fd) + "]";
}

/** @brief Remove trailing CR/LF from message payload. @param message Input message. @return Sanitized message or fallback token. */
std::string sanitizeMessage(const std::string& message) {
	std::string clean = message;
	while (!clean.empty() && (clean[clean.size() - 1] == '\n' || clean[clean.size() - 1] == '\r')) {
		clean.erase(clean.size() - 1);
	}
	if (clean.empty()) {
		return "(empty)";
	}
	return clean;
}

/** @brief Select output stream for a category. @param type Log category. @return std::cerr for errors, otherwise std::cout. */
std::ostream& streamFor(Logger::Type type) {
	if (type == Logger::ERROR) {
		return std::cerr;
	}
	return std::cout;
}
}

void Logger::setEnabled(Type type, bool enabled) {
	if (type < IN || type > ERROR) {
		return;
	}
	g_enabled[type] = enabled;
}

bool Logger::isEnabled(Type type) {
	if (type < IN || type > ERROR) {
		return false;
	}
	return g_enabled[type];
}

void Logger::enableAll() {
	for (int i = IN; i <= ERROR; ++i) {
		g_enabled[i] = true;
	}
}

void Logger::disableAll() {
	for (int i = IN; i <= ERROR; ++i) {
		g_enabled[i] = false;
	}
}

std::string Logger::enabledSummary() {
	std::string summary;
	for (int i = IN; i <= ERROR; ++i) {
		if (!summary.empty()) {
			summary += " ";
		}
		summary += typeNameFor(static_cast<Type>(i));
		summary += "=";
		summary += (g_enabled[i] ? "ON" : "OFF");
	}
	return summary;
}

void Logger::log(Type type, int fd, const std::string& message, const std::string& nickname) {
	if (!isEnabled(type)) {
		return;
	}
	std::ostream& out = streamFor(type);
	out << colorFor(type)
		<< buildTimestamp() << " "
		<< buildIdentifier(fd, nickname) << " "
		<< directionFor(type) << " "
		<< sanitizeMessage(message)
		<< "\033[0m"
		<< std::endl;
}
