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
	bool init() {
		this->close();
		if (!(_valid = sqlite3_open(_file_preparatore.c_str(), &_wh)
				== SQLITE_OK)) {
			BOOST_LOG_TRIVIAL(warning) << "Can't open database: " << sqlite3_errmsg(_wh)
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

			BOOST_LOG_TRIVIAL(warning)
			<< "Database: new, initialization ...";

			ret =
					sqlite3_exec(_wh,
							"CREATE TABLE main.init(creation INT PRIMARY KEY NOT NULL, c_version TEXT NOT NULL)",
							hqn::db::db::callback, nullptr, &zErrMsg);
			std::string insert =
					"INSERT INTO main.init(creation, c_version) VALUES(";
			const char *ins =
					insert.append(std::to_string(hqn::config::timestamp_now())).append(
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
		return valid();
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

	sqlite3 *wh() {
		return _wh;;
	}
}
;
class mbox {
	hqn::db::db _db;
public:
	mbox(const hqn::db::db &db) : _db (db) {

	}
};
}
}

#endif /* SRC_SERVER_DB_HPP_ */
