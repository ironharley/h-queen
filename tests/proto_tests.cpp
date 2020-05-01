#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "HQN Proto tests"
#include "../src/proto/proto.hpp"

#include <boost/test/unit_test.hpp>

hqn::proto::proto p;
hqn::proto::content_type ct;

BOOST_AUTO_TEST_SUITE(Hqn_proto_sute_1)
;
BOOST_AUTO_TEST_CASE(case_1) {
	BOOST_CHECK(hqn::proto::timestamp_now() > 0);
}
BOOST_AUTO_TEST_CASE(case_2) {
	BOOST_CHECK_EQUAL(sizeof(char), sizeof(uint8_t));
	//BOOST_CHECK_MESSAGE(p.foo() == 2, "Invalid foo comparation");
//        BOOST_WARN( sizeof(m) < 201 );
}

BOOST_AUTO_TEST_CASE(message_test ) {
	hqn::proto::address from("somefrom@email.org",
			hqn::proto::uuid("test1.harle.queen"));
	BOOST_CHECK_MESSAGE(sizeof(char) != sizeof(uint8_t), from.str());
	hqn::proto::address to("someto@email.org",
			hqn::proto::uuid("test2.harle.queen"));
	BOOST_CHECK_MESSAGE(sizeof(char) != sizeof(uint8_t), to.str());
	hqn::proto::subsription sub("subscr1", to);
	BOOST_CHECK_MESSAGE(sizeof(char) != sizeof(uint8_t), sub.str());

	hqn::proto::message_header md;
	/*
	 md._content_type = hqn::proto::content_type::text_json;
	 md._from = from;
	 md._to = to;
	 md._part_number = 0;
	 md._parts_total = 1;
	 md._timestamp = hqn::proto::timestamp_now();
	 md._type = hqn::proto::message_type::rcpt;
	 md._ttl = 86400;
	 */

	std::string test_payload("{will see it}");
	hqn::proto::message m(md, test_payload.c_str(), test_payload.size());

	BOOST_REQUIRE_EQUAL(sizeof(m), 201);
	//BOOST_FAIL( "Should never reach this line" );
}
BOOST_AUTO_TEST_SUITE_END();
