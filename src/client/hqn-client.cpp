/*
 * hqn-client.cpp
 *
 *  Created on: May 2, 2020
 *      Author: hkooper
 */
#include <hqn-client.h>
namespace hqn {
client::client(boost::asio::io_service &io_service,
		boost::asio::ssl::context &context,
		boost::asio::ip::tcp::resolver::iterator endpoint_iterator) :
		socket_(io_service, context) {

	boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
	socket_.lowest_layer().async_connect(endpoint,
			boost::bind(&client::handle_connect, this,
					boost::asio::placeholders::error, ++endpoint_iterator));
}

std::string client::get_password() const {
	return "test";
}

void client::handle_connect(const boost::system::error_code &error,
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
						boost::asio::placeholders::error, ++endpoint_iterator));
	} else {
		std::cout << "Connect failed: " << error << "\n";
	}
}

void client::handle_handshake(const boost::system::error_code &error) {
	if (!error) {
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

void client::handle_write(const boost::system::error_code &error,
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

void client::handle_read(const boost::system::error_code &error,
		size_t bytes_transferred) {
	if (!error) {
/*
		std::cout << "Reply: ";
		std::cout.write(reply_, bytes_transferred);
		std::cout << "\n";
*/
	} else {
		std::cout << "Read failed: " << error.message() << "\n";
	}
}

wrap::wrap(const char *host, const char *port) {
	_home = std::getenv("HOME");
	_home = _home.append("/").append(".hqn");
	std::filesystem::create_directory(_home);
	boost::asio::io_service io_service;

	boost::asio::ip::tcp::resolver resolver(io_service);
	boost::asio::ip::tcp::resolver::query query(host, port);
	boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

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

	std::string lvf = _home;
	context_.load_verify_file(lvf.append("/ssl/device.pem"));

	std::string uccf = _home;
	context_.use_certificate_chain_file(uccf.append("/ssl/client.pem"));
	std::string pkf = _home;
	context_.use_private_key_file(pkf.append("/ssl/client.key"),
			boost::asio::ssl::context::pem);

	client c(io_service, context_, iterator);
	io_service.run();

}

bool wrap::client_cert_verification(bool preverified,
		boost::asio::ssl::verify_context &ctx) {
	char subject_name[256];
	char issuer_name[256];
	X509 *cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
	X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
	X509_NAME_oneline(X509_get_issuer_name(cert), issuer_name, 256);
	int ver = hqn::proto::proto::cert_verify(cert);
/*
	if (ver == -1) {
		std::cout << "NOT Verified " << std::endl;
	} else {
		std::cout << "Verifying " << std::to_string(ver) << "\n";
	}
*/
	return ver == 1;
}
}

