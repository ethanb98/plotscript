#include "catch.hpp"

#include "message_queue.hpp"
#include "expression.hpp"
#include <string>
#include <utility>


TEST_CASE("Testing the basics of message_queue", "[message_queue]") {
	MsgSafeQueue<std::pair<std::string, Expression>> msg;
	REQUIRE(msg.empty());
}

TEST_CASE("Testing single pair", "[message_queue]") {
	MsgSafeQueue<std::pair<std::string, Expression>> msg;
	std::pair<std::string, Expression> par = { "henlo", Expression(3.0) };
	REQUIRE(msg.empty());
	REQUIRE(msg.try_pop(par) == false);
	msg.push(par);
	REQUIRE(!msg.empty());	
	REQUIRE(msg.try_pop(par) == true);
}

TEST_CASE("Testing multiple threads", "[message_queue]") {
	MsgSafeQueue<std::pair<std::string, Expression>> msg;
	std::pair<std::string, Expression> par = { "henlo", Expression(3.0) };
	std::pair<std::string, Expression> parb;
	REQUIRE(msg.empty());
	REQUIRE(par != parb);
	msg.push(par);
	msg.wait_and_pop(parb);
	REQUIRE(par == parb);
}