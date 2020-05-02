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

#include "../proto/proto.hpp"

enum {
	max_length = 1024
};

namespace hqn {
namespace client {

class client {
public:

	client(boost::asio::io_service &io_service,
			boost::asio::ssl::context &context,
			boost::asio::ip::tcp::resolver::iterator endpoint_iterator) :
			socket_(io_service, context) {

		boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
		socket_.lowest_layer().async_connect(endpoint,
				boost::bind(&client::handle_connect, this,
						boost::asio::placeholders::error, ++endpoint_iterator));
	}

	std::string get_password() const {
		return "test";
	}

	void handle_connect(const boost::system::error_code &error,
			boost::asio::ip::tcp::resolver::iterator endpoint_iterator) {
		if (!error) {
			socket_.async_handshake(boost::asio::ssl::stream_base::client,
					boost::bind(&client::handle_handshake, this,
							boost::asio::placeholders::error));
		} else if (endpoint_iterator
				!= boost::asio::ip::tcp::resolver::iterator()) {
			socket_.lowest_layer().close();
			boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
			socket_.lowest_layer().async_connect(endpoint,
					boost::bind(&client::handle_connect, this,
							boost::asio::placeholders::error,
							++endpoint_iterator));
		} else {
			std::cout << "Connect failed: " << error << "\n";
		}
	}

	void handle_handshake(const boost::system::error_code &error) {
		if (!error) {
			std::cout << "Enter message: ";
			std::cin.getline(request_, max_length);
			size_t request_length = strlen(request_);

			boost::asio::async_write(socket_,
					boost::asio::buffer(request_, request_length),
					boost::bind(&client::handle_write, this,
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
		} else {
			std::cout << "Handshake failed: " << error.message() << "\n";
		}
	}

	void handle_write(const boost::system::error_code &error,
			size_t bytes_transferred) {
		if (!error) {
			boost::asio::async_read(socket_,
					boost::asio::buffer(reply_, bytes_transferred),
					boost::bind(&client::handle_read, this,
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
		} else {
			std::cout << "Write failed: " << error.message() << "\n";
		}
	}

	void handle_read(const boost::system::error_code &error,
			size_t bytes_transferred) {
		if (!error) {
			std::cout << "Reply: ";
			std::cout.write(reply_, bytes_transferred);
			std::cout << "\n";
		} else {
			std::cout << "Read failed: " << error.message() << "\n";
		}
	}

private:
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
	char request_[max_length];
	char reply_[max_length];
};

class wrap {
public:
	wrap(const char *host, const char *port) {
		boost::asio::io_service io_service;

		boost::asio::ip::tcp::resolver resolver(io_service);
		boost::asio::ip::tcp::resolver::query query(host, port);
		boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(
				query);

		boost::asio::ssl::context context_(
				boost::asio::ssl::context::tlsv13_client);

		context_.set_options(
				boost::asio::ssl::context::default_workarounds
						| boost::asio::ssl::context::no_sslv2
						| boost::asio::ssl::context::single_dh_use);

		context_.set_verify_callback(
				boost::bind(&wrap::client_cert_verification, this, _1, _2));

		context_.set_verify_mode(
				boost::asio::ssl::context::verify_peer
						| boost::asio::ssl::context::verify_fail_if_no_peer_cert);
		context_.load_verify_file("etc/ssl/device.pem");

		context_.use_certificate_chain_file("etc/ssl/client.pem");
		context_.use_private_key_file("etc/ssl/client.key",
				boost::asio::ssl::context::pem);

		client c(io_service, context_, iterator);
		io_service.run();

	}
private:
	bool client_cert_verification(bool preverified,
			boost::asio::ssl::verify_context &ctx) {
		char subject_name[256];
		char issuer_name[256];
		X509 *cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
		X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
		X509_NAME_oneline(X509_get_issuer_name(cert), issuer_name, 256);
		int ver = hqn::proto::proto::cert_verify(cert);
		if (ver == -1) {
			std::cout << "NOT Verified " << std::endl;
		} else {
			std::cout << "Verifying " << std::to_string(ver) << "\n";
		}
		return ver == 1;
	}

};
}
}
int main(int argc, char *argv[]) {
	try {
		namespace po = boost::program_options;
		po::options_description desc("Options");
		desc.add_options()("help", "")("version", "")("host",
				po::value<std::string>()->default_value("localhost"))("port",
				po::value<std::string>()->default_value(std::to_string(hqn::config::defaults::server_port)));

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help") > 0) {
			hqn::proto::show_help(false);

		} else if (vm.count("version") > 0) {
			hqn::proto::show_version();

		} else {
			hqn::client::wrap w(
					boost::any_cast<std::string>(vm["host"].value()).c_str(),
					boost::any_cast<std::string>(vm["port"].value()).c_str());
		}
	} catch (boost::system::error_code &e) {
		std::cerr << "Exception: " << e.message() << "\n";
	}

	return 0;
}
