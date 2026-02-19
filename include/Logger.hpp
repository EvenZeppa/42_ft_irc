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
	static void setEnabled(Type type, bool enabled);
	static bool isEnabled(Type type);
	static void enableAll();
	static void disableAll();
	static std::string enabledSummary();
};
