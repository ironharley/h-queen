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
#include "console.hpp"

int main(int argc, char *argv[]) {
	try {

		boost::program_options::options_description desc("Options");
		desc.add_options()("help", "")("version", "")("host",
				boost::program_options::value<std::string>()->default_value("localhost"))("port",
						boost::program_options::value<std::string>()->default_value(
						std::to_string(hqn::config::defaults::server_port)));

		boost::program_options::variables_map vm;
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
		boost::program_options::notify(vm);

		if (vm.size() > 0) {
			if (vm.count("help") ==1) {
				hqn::proto::show_help(false);

			} else if (vm.count("version") ==1) {
				hqn::proto::show_version();

			} else if (vm.size() == 2) {
				hqn::wrap w(
						boost::any_cast<std::string>(vm["host"].value()).c_str(),
						boost::any_cast<std::string>(vm["port"].value()).c_str());
				hqn::cli::console c(">");
				int retCode = 0;
			    do {
			        retCode = c.readLine();
			        // We can also change the prompt based on last return value:
			        if ( retCode == hqn::cli::console::Ok )
			            c.setGreeting(">");
			        else
			            c.setGreeting("!>");

			        if ( retCode == 1 ) {
			            std::cout << "Received error code 1\n";
			        }
			        else if ( retCode == 2 ) {
			            std::cout << "Received error code 2\n";
			        }
			    }
			    while ( retCode != hqn::cli::console::Quit );
			} else {
				hqn::proto::show_help(false);
			}
		}
	} catch (boost::system::error_code &e) {
		std::cerr << "Exception: " << e.message() << "\n";
	}

	return 0;
}
