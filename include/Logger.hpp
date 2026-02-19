#pragma once

#include <string>

class Logger {
public:
	enum Type {
		IN,
		OUT,
		INFO,
		ERROR
	};

	static void log(Type type, int fd, const std::string& message, const std::string& nickname = "");
};
