/*
 * db.hpp
 *
 *  Created on: May 3, 2020
 *      Author: hkooper
 */

#ifndef SRC_SERVER_DB_HPP_
#define SRC_SERVER_DB_HPP_

#include "config.hpp"
#include <sqlite3.h>
#include <boost/date_time/local_time/local_time.hpp>

namespace hqn {
namespace db {

class db {
	hqn::config::config::server _svc_cfg;
	sqlite3 *_wh;
	bool _valid = false;
	std::string _file_preparatore;

	static int callback(void *nu, int argc, char **argv, char **azColName) {
		return 0;
	}

	static uint64_t timestamp_now() {
		boost::posix_time::ptime time_t_epoch(
				boost::gregorian::date(1970, 1, 1));
		boost::posix_time::ptime now =
				boost::posix_time::second_clock::local_time();
		boost::posix_time::time_duration diff = now - time_t_epoch;
		return diff.total_seconds();
	}

public:
	db(const hqn::config::config::server &svc_cfg) :
			_svc_cfg(svc_cfg), _wh(nullptr) {
		_file_preparatore =
				_svc_cfg.engine_parent.append("/").append(_svc_cfg.alias).append(
						"/hqndb-").append(PACKAGE_VERSION).append(".db");
	}
	~db() {
		this->close();
	}
	void init() {
		this->close();
		if (!(_valid = sqlite3_open(_file_preparatore.c_str(), &_wh)
				== SQLITE_OK)) {
			std::cerr << "Can't open database: " << sqlite3_errmsg(_wh)
					<< std::endl;
			;
		}
		int ret = sqlite3_wal_autocheckpoint(_wh, 1024);
		char *zErrMsg = 0;
		ret = sqlite3_exec(_wh, "SELECT * FROM main.init",
				hqn::db::db::callback, nullptr, &zErrMsg);
		//
		// check if database is new - try to init
		//
		if (ret != SQLITE_OK) {
			//todo add check in errmsg 'no such table: main.init' here
			std::cerr << "Initial check request: " << std::to_string(ret)
					<< " - " << zErrMsg << std::endl;

			BOOST_LOG_TRIVIAL(warning)
			<< "Database: new, initialization ...";

			ret =
					sqlite3_exec(_wh,
							"CREATE TABLE main.init(creation INT PRIMARY KEY NOT NULL, c_version TEXT NOT NULL)",
							hqn::db::db::callback, nullptr, &zErrMsg);
			std::string insert =
					"INSERT INTO main.init(creation, c_version) VALUES(";
			const char *ins =
					insert.append(std::to_string(hqn::db::db::timestamp_now())).append(
							", '").append(PACKAGE_VERSION).append("')").c_str();

			ret = sqlite3_exec(_wh, ins, hqn::db::db::callback, nullptr,
					&zErrMsg);
			if (ret != SQLITE_OK) {
				std::cerr << "Initial insert: " << std::to_string(ret) << " - "
						<< zErrMsg << std::endl;
				BOOST_LOG_TRIVIAL(error)
				<< "Database initialization failed: [ code = "
						<< std::to_string(ret) << ", message: " << zErrMsg
						<< " where insert: '" << ins << "']";

				_valid = false;
			} else {
				sqlite3_exec(_wh, "COMMIT", NULL, NULL, NULL);
				ret = sqlite3_exec(_wh, "SELECT * FROM main.init",
						hqn::db::db::callback, nullptr, &zErrMsg);
				if (ret != SQLITE_OK) {
					BOOST_LOG_TRIVIAL(error)
					<< "Database initialization failed: [ code = "
							<< std::to_string(ret) << ", message: " << zErrMsg
							<< "]";
					_valid = false;
				}
			}
		}
		if (valid()) {
			BOOST_LOG_TRIVIAL(info)
			<< "Database initialization ok";

		}
	}

	bool opened() {
		return _wh != nullptr;
	}

	void close() {
		_valid = false;
		if (this->opened()) {
			sqlite3_close(_wh);
		}
	}

	bool valid() {
		return this->opened() && _valid;
	}
}
;
}
}

#endif /* SRC_SERVER_DB_HPP_ */
