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
		std::string alias = "harlequeen";
		std::string log_parent = "/var/log";
		std::string log_file_mask = "server_%3N.log";
		uint16_t log_file_mb_rotation_size = 1;
		uint16_t log_file_mb_max_size = 5;
		uint32_t log_file_mb=1048576;
		std::string log_file_format = "[%TimeStamp%][%Severity%]:  %Message%";
		std::string engine_parent = "/var/lib";

	};

	explicit config(const std::string &file) :
			__file(file.c_str()) {
		boost::property_tree::ptree pt;
		boost::property_tree::ini_parser::read_ini(__file, pt);

		__server.port = pt.get<uint16_t>("server.port");
		__server.user = pt.get<std::string>("server.user");
		__server.alias = pt.get<std::string>("server.alias");
		__server.log_parent = pt.get<std::string>("server.log_parent");
		__server.log_file_format = pt.get<std::string>("server.log_file_format");
		__server.log_file_mask = pt.get<std::string>("server.log_file_mask");
		__server.log_file_mb_rotation_size = pt.get<uint16_t>(
				"server.log_file_mb_rotation_size");
		__server.log_file_mb_max_size = pt.get<uint16_t>(
				"server.log_file_mb_max_size");
		__server.log_file_mb = pt.get<uint32_t>(
						"server.log_file_mb");
		__server.engine_parent = pt.get<std::string>("server.engine_parent");

		hqn::config::config::init_log();
		BOOST_LOG_TRIVIAL(info)
		<< "config: " << __file;

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

	void init_log() {

		boost::log::register_simple_formatter_factory<
				boost::log::trivial::severity_level, char>("Severity");
#ifdef _DEBUG
		// Output message to console
		boost::log::add_console_log(std::cout, boost::log::keywords::format =
				__server.log_file_format, boost::log::keywords::auto_flush = true);
#endif

		boost::log::add_file_log(
				boost::log::keywords::file_name = __server.log_parent.append(
						"/").append(__server.alias).append("/").append(
						__server.log_file_mask),
				boost::log::keywords::rotation_size =
						__server.log_file_mb_rotation_size * __server.log_file_mb,
				boost::log::keywords::max_size = __server.log_file_mb_max_size
						* __server.log_file_mb,
				boost::log::keywords::time_based_rotation =
						boost::log::sinks::file::rotation_at_time_point(0, 0,
								0), boost::log::keywords::format = __server.log_file_format,
				boost::log::keywords::auto_flush = true);

		boost::log::add_common_attributes();

		// Only output message with INFO or higher severity in Release
#ifndef _DEBUG
		boost::log::core::get()->set_filter(
				boost::log::trivial::severity >= boost::log::trivial::info);
#endif

	}

};
}
}

#endif /* SRC_SERVER_CONFIG_HPP_ */
