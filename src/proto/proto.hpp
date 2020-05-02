/*
 * proto.hpp
 *
 *  Created on: May 1, 2020
 *      Author: hkooper
 */

#ifndef SRC_PROTO_PROTO_HPP_
#define SRC_PROTO_PROTO_HPP_

#include "constants.hpp"

#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/name_generator.hpp>
#include <boost/crc.hpp>      // for boost::crc_basic, boost::crc_optimal

#include <boost/date_time/local_time/local_time.hpp>

namespace hqn {
namespace proto {
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
	std::cout << hqn::proto::COPYRIGHT << std::endl;
	if (server)
		std::cout << hqn::proto::HELP_SERVER_SCREEN << std::endl;
	else
		std::cout << hqn::proto::HELP_CLI_SCREEN << std::endl;
}

static void show_version() {
	std::cout << hqn::proto::COPYRIGHT << std::endl;
	std::cout << PACKAGE_STRING << std::endl;
}

enum receipt_code : uint8_t {
	ok = 0, to_address_invalid, to_address_rejected, ttl = 255,
};

enum message_type : uint8_t {
	cmd = 0, msg = 1, rcpt = 255,
};

enum content_type : uint8_t {
	native = 0, text_plain, text_json, text_xml, protobuf,
};

enum header_type : uint8_t {
	string, integer,
};

class address {
public:
	address(const std::string &cn, const boost::uuids::uuid &server) :
			_cn(cn), _server(server), _uuid(hqn::proto::uuid(cn.c_str())) {
	}
	address(const address &another) :
			_cn(another._cn), _server(another._server), _uuid(another._uuid) {

	}
	virtual ~address() {
	}
	virtual std::string str() {
		std::stringstream scl;
		scl << _uuid;
		std::stringstream svc;
		svc << _server;
		return scl.str().append("@").append(svc.str());

	}
private:
	std::string _cn; // CN from cert / name of subscription
	boost::uuids::uuid _server; // server's uuid
	boost::uuids::uuid _uuid; // generated when logged on/created
};

class subsription: public hqn::proto::address {
public:
	subsription(const std::string &_sname, const hqn::proto::address &_creator) :
			hqn::proto::address(_creator) {
		_uuid = hqn::proto::uuid(_sname.c_str());
	}
	virtual std::string str() override {
		std::stringstream scl;
		scl << _uuid;
		return scl.str().append("@").append(address::str());

	}

private:
	boost::uuids::uuid _uuid;
	std::string _name;
	std::vector<address> _subscribers; // subscribers
	std::vector<address> _blocked;  // black list who can send in this subscr
};

class env_hdr {
private:
	address _to;
	address _from;
	message_type _type;
};

class header {
private:
	std::string _name;
	std::string _value;
	header_type _type;
};

class message_header {
public:
	void content_length(size_t len) {
		_content_length = len;
	}
	uint64_t id() {
		return _id;
	}
	uint64_t timestamp() {
		return _timestamp;
	}
	size_t content_length() {
		return _content_length;
	}
	uint32_t ttl() {
		return _ttl;
	}

	bool expired() {
		return hqn::proto::timestamp_now() >= timestamp() + ttl();
	}
private:
	uint64_t _id; // sequence generated
	uint64_t _timestamp = hqn::proto::timestamp_now(); // ts when created
	uint16_t _part_number; // current part (zero-started)
	uint16_t _parts_total; // total parts (ie 0 from 1)
	content_type _content_type;
	size_t _content_length;
	uint32_t _ttl = 3600; // seconds, 60-86400 def
};

class message {
private:
	message_header _duty_part;
	std::vector<header> _custom_headers;
	std::vector<char> _payload; // do not use it directly!!!

public:
	message() {
	}
	message(message_header &mh, const void *content, size_t len) {
		duty(mh);
		payload(content, len);
	}
	void duty(message_header &mh) {
		_duty_part = mh;
	}
	void payload(const void *content, size_t len) {
		_payload.clear();
		_payload.resize(len);
		_duty_part.content_length(len);
		std::memcpy(_payload.data(), content, len);
	}
	bool expired() {
		return _duty_part.expired();
	}
	uint64_t id() {
		return _duty_part.id();
	}
	uint64_t timestamp() {
		return _duty_part.timestamp();
	}

};

class receipt {
public:
	receipt(hqn::proto::message &_msg) :
			_message_id(_msg.id()), _message_timestamp(_msg.timestamp()) {
	}
	void code(receipt_code &code) {
		_code = code;
	}
	receipt_code code() {
		return _code;
	}
	uint64_t id() {
		return _message_id;
	}
	bool ok() {
		return _code == receipt_code::ok;
	}

private:
	uint64_t _message_id; // for message id
	uint64_t _message_timestamp; // ts of orig message
	uint64_t _delivery_timestamp = hqn::proto::timestamp_now(); // ts of receipt creation
	receipt_code _code = receipt_code::ok;
};

class serializable {
public:
	virtual size_t serialize_size() const = 0;
	virtual void serialize(char *out) const = 0;
	virtual void deserialize(const char *in) = 0;
	virtual ~serializable() {
	}
};

class serializer {
public:
	void save(const serializable &s) {
		char *data;
		size_t data_len;
		reserve_memory(data, data_len, s);
		s.serialize(data);
		//EEPROM::Save( data , data_len );
		delete[] data;
	}

	void load(serializable &s) {
		char *data;
		size_t data_len;
		reserve_memory(data, data_len, s);
		//EEPROM::Load( data, data_len );
		s.deserialize(data);
		delete[] data;
	}

private:
	char* reserve_memory(char *&data, size_t &data_len, const serializable &s) {
		return new char[s.serialize_size()];
	}
};

template<typename POD>
class serializablePOD {
public:
	static size_t serialize_size(POD str) {
		return sizeof(POD);
	}
	static char* serialize(char *target, POD value) {
		return (char*) memcpy(target, &value, serialize_size(value));
	}
	static const char* deserialize(const char *source, POD &target) {
		memcpy(&target, source, serialize_size(target));
		return source + serialize_size(target);
	}
};
template<>
size_t serializablePOD<char*>::serialize_size(char *str) {
	return sizeof(size_t) + strlen(str);
}

template<>
const char* serializablePOD<char*>::deserialize(const char *source,
		char *&target) {
	size_t length;
	memcpy(&length, source, sizeof(size_t));
	memcpy(&target, source + sizeof(size_t), length);
	return source + sizeof(size_t) + length;
}

class envelope: public serializable {
	virtual size_t serialize_size() const {
		return serializablePOD<unsigned int>::serialize_size(_sign)
				+ serializablePOD<unsigned long int>::serialize_size(_length)
				+ serializablePOD<char*>::serialize_size(_header)
				+ serializablePOD<char*>::serialize_size(_body)
				+ serializablePOD<unsigned int>::serialize_size(_crc32);

	}

	virtual void serialize(char *out) const {
		out = serializablePOD<int>::serialize(out, _sign);
		out = serializablePOD<unsigned long int>::serialize(out, _length);
		out = serializablePOD<char*>::serialize(out, _header);
		out = serializablePOD<char*>::serialize(out, _body);
		out = serializablePOD<int>::serialize(out, _crc32);

	}
	virtual void deserialize(const char *in) {
		in = serializablePOD<int>::deserialize(in, _sign);
		in = serializablePOD<unsigned long int>::deserialize(in, _length);
		in = serializablePOD<char*>::deserialize(in, _header);
		in = serializablePOD<char*>::deserialize(in, _body);
		in = serializablePOD<int>::deserialize(in, _crc32);

	}
private:
	int32_t crc32(void const *buffer, std::size_t byte_count) {
		boost::crc_32_type result;
		result.process_bytes(buffer, byte_count);
		return result.checksum();
	}

	int _sign = 0x5F48514E; // _HQN
	unsigned long int _length;
	char *_header;
	char *_body;
	int _crc32;  // from _sign till end of _body
};

class sequence: public serializable {
public:
	sequence(const std::string name) :
			_name((char*)name.c_str()), _value(0), _increment(1) {
	}
	sequence(const std::string name, uint64_t start, uint64_t inc) :
			_name((char*)name.c_str()), _value(start), _increment(inc) {
	}
	sequence(const std::string name, uint64_t inc) :
			_name((char*)name.c_str()), _value(0), _increment(inc) {
	}
	virtual size_t serialize_size() const {
		return serializablePOD<char*>::serialize_size(_name)
				+ serializablePOD<unsigned long int>::serialize_size(_value)
				+ serializablePOD<unsigned long int>::serialize_size(_increment);

	}
	virtual void serialize(char *out) const {
		out = serializablePOD<char*>::serialize(out, _name);
		out = serializablePOD<unsigned long int>::serialize(out, _value);
		out = serializablePOD<unsigned long int>::serialize(out, _increment);

	}
	virtual void deserialize(const char *in) {
		in = serializablePOD<char*>::deserialize(in, _name);
		in = serializablePOD<unsigned long int>::deserialize(in, _value);
		in = serializablePOD<unsigned long int>::deserialize(in, _increment);

	}
	uint64_t inc() {
		return (_value += _increment);
	}
	uint64_t get() {
		return _value;
	}
private:
	char *_name;
	unsigned long int _value;
	unsigned long int _increment;
};
class proto {
public:

	/**
	 * Based from here
	 * https://kahdev.wordpress.com/2009/02/15/verifying-using-a-certificate-store/
	 */
	static int cert_verify(X509 *cert) {
		int result = 0;

		try {
			X509_STORE *store = X509_STORE_new();

			//TODO chg here to path from settings
			loadToStore("etc/ssl/device.pem", store); // server cert
			loadToStore("etc/ssl/rootCA.pem", store); // ca cert

			// Create the context to verify the certificate.
			X509_STORE_CTX *ctx = X509_STORE_CTX_new();

			// Initial the store to verify the certificate.
			X509_STORE_CTX_init(ctx, store, cert, NULL);

			result = X509_verify_cert(ctx);

			X509_STORE_CTX_cleanup(ctx);
			X509_STORE_CTX_free(ctx);
			X509_STORE_free(store);
			ctx = NULL;
			store = NULL;
		} catch (const std::exception &ex) {
			std::cerr << "Exception: " << ex.what() << "\n";
		}
		return result;
	}

	/**
	 * Next one example
	 * http://www.zedwood.com/article/c-libssl-verify-x509-certificate
	 */
	static int sig_verify(const char *cert_pem, const char *intermediate_pem) {
		int result = 0;
		try {
			if (std::strlen(cert_pem) > 0
					&& std::strlen(intermediate_pem) > 0) {
				BIO *b = BIO_new(BIO_s_mem());
				BIO_puts(b, intermediate_pem);
				X509 *issuer = PEM_read_bio_X509(b, NULL, NULL, NULL);
				EVP_PKEY *signing_key = X509_get_pubkey(issuer);

				BIO *c = BIO_new(BIO_s_mem());
				BIO_puts(c, cert_pem);
				X509 *x509 = PEM_read_bio_X509(c, NULL, NULL, NULL);

				result = X509_verify(x509, signing_key);

				EVP_PKEY_free(signing_key);
				BIO_free(b);
				BIO_free(c);
				X509_free(x509);
				X509_free(issuer);
			}
		} catch (const std::exception &ex) {
			std::cerr << "Exception: " << ex.what() << "\n";
		}

		return result;
	}
private:

	/**
	 * Based from here
	 * https://kahdev.wordpress.com/2009/02/15/verifying-using-a-certificate-store/
	 */
	static void loadToStore(std::string file, X509_STORE *&store) {
		X509 *cert = loadCert(file);
		if (cert != NULL) {
			X509_STORE_add_cert(store, cert);
		} else {
			std::cerr << "Can not load certificate " << file << std::endl;
		}
	}

	static X509* loadCert(std::string file) {
		FILE *fp = fopen(file.c_str(), "r");
		X509 *cert = PEM_read_X509(fp, NULL, NULL, NULL);
		fclose(fp);
		return cert;
	}
};
}
}

#endif /* SRC_PROTO_PROTO_HPP_ */
