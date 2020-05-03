/*
 * db-tests.cpp
 *
 *  Created on: May 3, 2020
 *      Author: hkooper
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "HQN Sqlite3 tests"
#include "../src/server/db.hpp"

#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(Hqn_sqlie_sute_1)
;

std::string conf_file = "etc/harlequeen.cfg";
hqn::config::config cfg(conf_file);

BOOST_AUTO_TEST_CASE(init_1, * utf::label("init1")) {
	hqn::db::db sqlite(cfg.get_server_prefs());
	sqlite.init();
	BOOST_CHECK(sqlite.valid());
}
BOOST_AUTO_TEST_SUITE_END();

