/*
 * constants.hpp
 *
 *  Created on: May 2, 2020
 *      Author: hkooper
 */
#ifndef SRC_PROTO_CONSTANTS_HPP_
#define SRC_PROTO_CONSTANTS_HPP_
#include <sys/types.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/name_generator.hpp>
#include <boost/date_time/local_time/local_time.hpp>

namespace hqn {
namespace config {
static const char *COPYRIGHT =
		"(c)HarleQueen craftsmen @ worldwide and handmade. Since 2020";

static const char *HELP_SERVER_SCREEN =
		"hqn-server [--config=/etc/harlequeen/harlequeen.cfg | --version | --help]";
static const char *HELP_CLI_SCREEN = "hqn-cli [--version | --help]";

static uint64_t timestamp_now() {
	boost::posix_time::ptime time_t_epoch(boost::gregorian::date(1970, 1, 1));
	boost::posix_time::ptime now =
			boost::posix_time::second_clock::local_time();
	boost::posix_time::time_duration diff = now - time_t_epoch;
	return diff.total_seconds();
}
static boost::uuids::uuid uuid(const char *ns_uuid_) {
	boost::uuids::name_generator_sha1 gen(boost::uuids::ns::dns());
	return gen(ns_uuid_);
}

static void show_help(bool server) {
	std::cout << hqn::config::COPYRIGHT << std::endl;
	if (server)
		std::cout << hqn::config::HELP_SERVER_SCREEN << std::endl;
	else
		std::cout << hqn::config::HELP_CLI_SCREEN << std::endl;
}

static void show_version() {
	std::cout << hqn::config::COPYRIGHT << std::endl;
	std::cout << PACKAGE_STRING << std::endl;
}

class defaults {
public:
	static const uint16_t server_port = 34434;


};
}}



#endif /* SRC_PROTO_CONSTANTS_HPP_ */
