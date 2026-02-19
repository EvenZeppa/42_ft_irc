#include "../include/Logger.hpp"

#include <ctime>
#include <iostream>
#include <sstream>

namespace {
bool g_enabled[4] = {true, true, true, true};

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

std::string intToString(int value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

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

std::string buildIdentifier(int fd, const std::string& nickname) {
	if (!nickname.empty()) {
		return "[NICK:" + nickname + "]";
	}
	return "[FD:" + intToString(fd) + "]";
}

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

std::ostream& streamFor(Logger::Type type) {
	if (type == Logger::ERROR) {
		return std::cerr;
	}
	return std::cout;
}
} // namespace

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
