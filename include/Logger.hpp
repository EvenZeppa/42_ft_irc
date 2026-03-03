/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ezeppa <ezeppa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 00:00:00 by ezeppa            #+#    #+#             */
/*   Updated: 2025/03/03 00:00:00 by ezeppa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class Logger {
public:
	/**
	 * @brief Supported log categories.
	 */
	enum Type {
		IN,
		OUT,
		INFO,
		ERROR
	};

	/**
	 * @brief Write a formatted log line if the category is enabled.
	 * @param type Log category.
	 * @param fd Related file descriptor.
	 * @param message Raw message to print.
	 * @param nickname Optional nickname used in identifier.
	 */
	static void log(Type type, int fd, const std::string& message, const std::string& nickname = "");
	/**
	 * @brief Enable or disable a specific log category.
	 * @param type Log category.
	 * @param enabled Desired state.
	 */
	static void setEnabled(Type type, bool enabled);
	/**
	 * @brief Check whether a log category is enabled.
	 * @param type Log category.
	 * @return True when enabled.
	 */
	static bool isEnabled(Type type);
	/**
	 * @brief Enable all log categories.
	 */
	static void enableAll();
	/**
	 * @brief Disable all log categories.
	 */
	static void disableAll();
	/**
	 * @brief Build a compact status string of all category switches.
	 * @return Status string (e.g. IN=ON OUT=OFF ...).
	 */
	static std::string enabledSummary();
};
