#include "catch.hpp"

#include "expression.hpp"

TEST_CASE( "Test default expression", "[expression]" ) {

  Expression exp;

  REQUIRE(!exp.isHeadNumber());
  REQUIRE(!exp.isHeadSymbol());
}

TEST_CASE( "Test double expression", "[expression]" ) {

  Expression exp(6.023);

  REQUIRE(exp.isHeadNumber());
  REQUIRE(!exp.isHeadSymbol());
}


TEST_CASE( "Test symbol expression", "[expression]" ) {

  Expression exp(Atom("asymbol"));

  REQUIRE(!exp.isHeadNumber());
  REQUIRE(exp.isHeadSymbol());
}

/*TEST_CASE("Testing Lambda", "[environment]") {
	Expression exp(Atom("lambda"));

	std::vector<Expression> args1 = { exp, Expression(Atom("+")), Expression(Atom("+ +")) };
	//REQUIRE_THROWS_AS(exp);
	REQUIRE_THROWS_AS(exp(args1), SemanticError);
	//REQUIRE_NOTHROW(args1);


	REQUIRE(!exp.isHeadNumber());
	REQUIRE(exp.isHeadSymbol());

}*/