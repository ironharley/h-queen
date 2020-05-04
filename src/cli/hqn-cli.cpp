//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>

#include "../client/hqn-client.h"

int main(int argc, char *argv[]) {
	try {
		namespace po = boost::program_options;
		po::options_description desc("Options");
		desc.add_options()("help", "")("version", "")("host",
				po::value<std::string>()->default_value("localhost"))("port",
				po::value<std::string>()->default_value(
						std::to_string(hqn::config::defaults::server_port)));

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.size() > 0) {
			if (vm.count("help") > 0) {
				hqn::proto::show_help(false);

			} else if (vm.count("version") > 0) {
				hqn::proto::show_version();

			} else if (vm.size() == 2) {
				wrap w(
						boost::any_cast<std::string>(vm["host"].value()).c_str(),
						boost::any_cast<std::string>(vm["port"].value()).c_str());
			}
		}
	} catch (boost::system::error_code &e) {
		std::cerr << "Exception: " << e.message() << "\n";
	}

	return 0;
}
