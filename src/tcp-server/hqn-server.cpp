//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/name_generator.hpp>

#include "../proto/proto.cpp"
#include "config.cpp"

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;
namespace hqn {
namespace server {

class session {
public:

	session(boost::asio::io_service &io_service,
			boost::asio::ssl::context &context) :
			socket_(io_service, context) {
	}

	ssl_socket::lowest_layer_type& socket() {
		return socket_.lowest_layer();
	}

	void start() {
		socket_.async_handshake(boost::asio::ssl::stream_base::server,
				boost::bind(&session::handle_handshake, this,
						boost::asio::placeholders::error));
	}

	void handle_handshake(const boost::system::error_code &error) {
		if (!error) {
			socket_.async_read_some(boost::asio::buffer(data_, max_length),
					boost::bind(&session::handle_read, this,
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
		} else {
			delete this;
		}
	}

	void handle_read(const boost::system::error_code &error,
			size_t bytes_transferred) {
		if (!error) {
			boost::asio::async_write(socket_,
					boost::asio::buffer(data_, bytes_transferred),
					boost::bind(&session::handle_write, this,
							boost::asio::placeholders::error));
		} else {
			delete this;
		}
	}

	void handle_write(const boost::system::error_code &error) {
		if (!error) {
			socket_.async_read_some(boost::asio::buffer(data_, max_length),
					boost::bind(&session::handle_read, this,
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
		} else {
			delete this;
		}
	}

private:
	ssl_socket socket_;

	enum {
		max_length = 1024
	};
	char data_[max_length];
};

class server {
public:

	server(boost::asio::io_service &io_service, unsigned short port) :
			io_service_(io_service), acceptor_(io_service,
					boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
							port)), context_(
					boost::asio::ssl::context::tlsv13_server) {

		/*
		 std::cout << "name@nobody.net uuid in dns namespace, sha1 version: " << uuid("name@nobody.net") << std::endl;
		 std::cout << "name2@nobody.net uuid in dns namespace, sha1 version: " << uuid("name2@nobody.net") << std::endl;
		 std::cout << "name@nobody.net uuid in dns namespace, sha1 version: " << uuid("name@nobody.net") << std::endl;
		 */

		context_.set_options(
				boost::asio::ssl::context::default_workarounds
						| boost::asio::ssl::context::no_sslv2
						| boost::asio::ssl::context::single_dh_use);

		//context_.set_password_callback(boost::bind(&server::get_password, this));

		context_.use_certificate_chain_file("etc/ssl/device.pem");
		context_.use_private_key_file("etc/ssl/device.key",
				boost::asio::ssl::context::pem);
		context_.use_tmp_dh_file("etc/ssl/dh2048.pem");

		/**
		 * verify client auth
		 */
		context_.set_verify_callback(
				boost::bind(&server::client_cert_verification, this, _1, _2));
		context_.set_verify_mode(
				boost::asio::ssl::context::verify_fail_if_no_peer_cert
						| boost::asio::ssl::context::verify_peer);
		context_.load_verify_file("etc/ssl/device.pem");

		session *new_session = new session(io_service_, context_);
		acceptor_.async_accept(new_session->socket(),
				boost::bind(&server::handle_accept, this, new_session,
						boost::asio::placeholders::error));
	}

	bool client_cert_verification(bool preverified,
			boost::asio::ssl::verify_context &ctx) {
		X509 *cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
		int ver = hqn::proto::proto::cert_verify(cert);
		if (ver == 1) {
			char subject_name[256];
			X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
			// todo identifying client by CN

			std::cout << "Verifying " << subject_name << " res "
					<< std::to_string(ver) << std::endl;
		}
		return ver == 1;
	}

	void handle_accept(session *new_session,
			const boost::system::error_code &error) {
		if (!error) {
			new_session->start();
			new_session = new session(io_service_, context_);
			acceptor_.async_accept(new_session->socket(),
					boost::bind(&server::handle_accept, this, new_session,
							boost::asio::placeholders::error));
		} else {
			delete new_session;
		}
	}

private:
	boost::uuids::uuid uuid(const char *ns_uuid_) const {
		boost::uuids::name_generator_sha1 gen(boost::uuids::ns::dns());
		return gen(ns_uuid_);
	}

	boost::asio::io_service &io_service_;
	boost::asio::ip::tcp::acceptor acceptor_;
	boost::asio::ssl::context context_;

};
}
}

int main(int argc, char *argv[]) {
	const char *HELP_SCREEN =
			"hqn-server [--config=/etc/harlequeen/harlequeen.cfg | --version | --help]";
	int res = 1;
	try {
		hqn::config::init_log();

		namespace po = boost::program_options;
		po::options_description desc("Options");
		desc.add_options()("help", "")("version", "")("config",
				po::value<std::string>()->default_value(
						"/etc/harlequeen/harlequeen.cfg"));

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help") == 1) {
			std::cout << HELP_SCREEN << std::endl;

		} else if (vm.count("version") == 1) {
			std::cout << PACKAGE_STRING << std::endl;

		} else {
			std::string conf_file = "/etc/harlequeen/harlequeen.cfg";
			if (vm.count("config") > 0) {
				conf_file =
						boost::any_cast<std::string>(vm["config"].value()).c_str();
			}

			BOOST_LOG_TRIVIAL(info)
			<< "help parsed: " << vm.count("help");
			BOOST_LOG_TRIVIAL(info)
			<< "config: " << conf_file.c_str();

			boost::asio::io_service io_service;

			using namespace std;
			hqn::config::config cfg(conf_file);
			hqn::server::server s(io_service, cfg.port());

			io_service.run();
			res = 0;
		}
	} catch (std::exception &e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return res;
}
