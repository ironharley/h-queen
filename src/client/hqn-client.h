/*
 * hqn-client.h
 *
 *  Created on: May 4, 2020
 *      Author: hkooper
 */

#ifndef SRC_CLIENT_HQN_CLIENT_H_
#define SRC_CLIENT_HQN_CLIENT_H_

#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>

#include "../proto/proto.hpp"

namespace hqn {
enum {
	max_length = 1024
};


class client {
public:

	client(boost::asio::io_service &io_service,
			boost::asio::ssl::context &context,
			boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

	std::string get_password() const;

	void handle_connect(const boost::system::error_code &error,
			boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

	void handle_handshake(const boost::system::error_code &error);

	void handle_write(const boost::system::error_code &error,
			size_t bytes_transferred);

	void handle_read(const boost::system::error_code &error,
			size_t bytes_transferred);
private:
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
	char request_[max_length];
	char reply_[max_length];
};

class wrap {
public:
	wrap(const char *host, const char *port);

private:
	bool client_cert_verification(bool preverified,
			boost::asio::ssl::verify_context &ctx);

	std::string _home;
};

extern "C" {
wrap * create(const char *host, const char *port) {
    return new wrap(host, port);
}
}}
#endif /* SRC_CLIENT_HQN_CLIENT_H_ */
