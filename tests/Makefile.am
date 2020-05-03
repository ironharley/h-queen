check_PROGRAMS = proto_tests db-tests
proto_tests_CPPFLAGS = -I../src/ ${BOOST_CPPFLAGS}
proto_tests_LDFLAGS = ${BOOST_LDFLAGS}
proto_tests_LDADD = -lboost_unit_test_framework
proto_tests_SOURCES = proto_tests.cpp

db_tests_CPPFLAGS = -I../src/ ${BOOST_CPPFLAGS}
db_tests_LDFLAGS = ${BOOST_LDFLAGS}
db_tests_LDADD = -lboost_unit_test_framework
db_tests_SOURCES = db-tests.cpp