//
// hqn-cli.cpp
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

unsigned info(const std::vector<std::string>&) {
	std::cout
			<< "Welcome to the example console. This command does not really\n"
			<< "do anything aside from printing this statement. Thus it does\n"
			<< "not need to look into the arguments that are passed to it.\n";
	return hqn::cli::console::Ok;
}

// In this command we implement a basic calculator
unsigned calc(const std::vector<std::string> &input) {
	if (input.size() != 4) {
		// The first element of the input array is always the name of the
		// command as registered in the console.
		std::cout << "Usage: " << input[0] << " num1 operator num2\n";
		// We can return an arbitrary error code, which we can catch later
		// as Console will return it.
		return hqn::cli::console::Error;
	}
	double num1 = std::stod(input[1]), num2 = std::stod(input[3]);

	char op = input[2][0];

	double result;
	switch (op) {
	case '*':
		result = num1 * num2;
		break;
	case '+':
		result = num1 + num2;
		break;
	case '/':
		result = num1 / num2;
		break;
	case '-':
		result = num1 - num2;
		break;
	default:
		std::cout << "The inserted operator is not supported\n";
		// Again, we can return an arbitrary error code to catch it later.
		return 2;
	}
	std::cout << "Result: " << result << '\n';
	return hqn::cli::console::Ok;
}

int main(int argc, char *argv[]) {
	try {

		boost::program_options::options_description desc("Options");
		desc.add_options()("help", "")("version", "")("host",
				boost::program_options::value<std::string>()->default_value(
						"localhost"))("port",
				boost::program_options::value<std::string>()->default_value(
						std::to_string(hqn::config::defaults::server_port)));

		boost::program_options::variables_map vm;
		boost::program_options::store(
				boost::program_options::parse_command_line(argc, argv, desc),
				vm);
		boost::program_options::notify(vm);

		if (vm.size() > 0) {
			if (vm.count("help") == 1) {
				hqn::config::show_help(false);

			} else if (vm.count("version") == 1) {
				hqn::config::show_version();

			} else if (vm.size() == 2) {
				hqn::wrap w(
						boost::any_cast<std::string>(vm["host"].value()).c_str(),
						boost::any_cast<std::string>(vm["port"].value()).c_str());
				hqn::config::show_version();
				hqn::cli::console c(">");
				c.registerCommand("info", info);
				c.registerCommand("calc", calc);
				c.executeCommand("help");
				c.executeFile("exampleScript");
				int retCode = 0;
				do {
					retCode = c.readLine();
					// We can also change the prompt based on last return value:
					if (retCode == hqn::cli::console::Ok)
						c.setGreeting(">");
					else
						c.setGreeting("!>");

					if (retCode == 1) {
						std::cout << "Received error code 1\n";
					} else if (retCode == 2) {
						std::cout << "Received error code 2\n";
					}
				} while (retCode != hqn::cli::console::Quit);
			} else {
				hqn::config::show_help(false);
			}
		}
	} catch (boost::system::error_code &e) {
		std::cerr << "Exception: " << e.message() << "\n";
	}

	return 0;
}
