#include "../include/Logger.hpp"

#include <ctime>
#include <iostream>
#include <sstream>

namespace {
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

void Logger::log(Type type, int fd, const std::string& message, const std::string& nickname) {
	std::ostream& out = streamFor(type);
	out << colorFor(type)
		<< buildTimestamp() << " "
		<< buildIdentifier(fd, nickname) << " "
		<< directionFor(type) << " "
		<< sanitizeMessage(message)
		<< "\033[0m"
		<< std::endl;
}
