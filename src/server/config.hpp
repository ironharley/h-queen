/*
 * config.hpp
 *
 *  Created on: May 1, 2020
 *      Author: hkooper
 */

#ifndef SRC_SERVER_CONFIG_HPP_
#define SRC_SERVER_CONFIG_HPP_

#include <stdlib.h>
#include <pwd.h>
#include <stdio.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "boost/log/trivial.hpp"
#include "boost/log/utility/setup.hpp"
#include <boost/log/expressions.hpp>

#include <iostream>
#include <exception>
#include "../proto/constants.hpp"

namespace hqn {
namespace config {
static void init_log() {
	static const std::string COMMON_FMT(
			"[%TimeStamp%][%Severity%]:  %Message%");

	boost::log::register_simple_formatter_factory<
			boost::log::trivial::severity_level, char>("Severity");

	// Output message to console
	boost::log::add_console_log(std::cout, boost::log::keywords::format =
			COMMON_FMT, boost::log::keywords::auto_flush = true);

	// Output message to file, rotates when file reached 1mb or at midnight every day. Each log file
	// is capped at 1mb and total is 20mb
	boost::log::add_file_log(boost::log::keywords::file_name =
			"/var/log/hqn/server_%3N.log",
			boost::log::keywords::rotation_size = 1 * 1024 * 1024,
			boost::log::keywords::max_size = 20 * 1024 * 1024,
			boost::log::keywords::time_based_rotation =
					boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
			boost::log::keywords::format = COMMON_FMT,
			boost::log::keywords::auto_flush = true);

	boost::log::add_common_attributes();

	// Only output message with INFO or higher severity in Release
#ifndef _DEBUG
	boost::log::core::get()->set_filter(
			boost::log::trivial::severity >= boost::log::trivial::info);
#endif

}
class env {
public:
	static std::string current_user() {
		uid_t uid = geteuid();
		if (!uid)
			throw std::runtime_error("root forbidden");
		struct passwd *pw = getpwuid(uid);
		if (pw) {
			return std::string(pw->pw_name);
		}
		return {};
	}
};
class config {
public:
	struct server {
		uint16_t port;
		std::string user;
	};

	explicit config(const std::string &file) :
			__file(file.c_str()) {
		boost::property_tree::ptree pt;
		boost::property_tree::ini_parser::read_ini(__file, pt);

		__server.port = pt.get<uint16_t>("server.port");
		__server.user = pt.get<std::string>("server.user");

		std::string cuser = env::current_user();
		_valid = !__server.user.compare(cuser);
		if (!_valid) {
			std::cerr << "Illegal user " << cuser << ", only " << __server.user
					<< " allowed" << std::endl;
		}
	}

	uint16_t port() {
		return __server.port;
	}

	server get_server_prefs() {
		return __server;
	}

	bool valid() {
		return _valid;
	}
private:
	const char *__file;
	server __server;
	bool _valid;

};
}
}

#endif /* SRC_SERVER_CONFIG_HPP_ */
