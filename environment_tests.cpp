#include "catch.hpp"

#include "environment.hpp"
#include "semantic_error.hpp"

#include <cmath>
#include <string>

TEST_CASE( "Test default constructor", "[environment]" ) {

  Environment env;

  REQUIRE(env.is_known(Atom("pi")));
  REQUIRE(env.is_exp(Atom("pi")));

  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));

  REQUIRE(env.is_proc(Atom("+")));
  REQUIRE(env.is_proc(Atom("-")));
  REQUIRE(env.is_proc(Atom("*")));
  REQUIRE(env.is_proc(Atom("/")));
  REQUIRE(!env.is_proc(Atom("op")));
}

TEST_CASE( "Test get expression", "[environment]" ) {
  Environment env;

  REQUIRE(env.get_exp(Atom("pi")) == Expression(std::atan2(0, -1)));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());
}

TEST_CASE( "Test add expression", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  REQUIRE(env.is_known(Atom("one")));
  REQUIRE(env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == a);

  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);
  REQUIRE(env.is_known(Atom("hi")));
  REQUIRE(env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == b);

  REQUIRE_THROWS_AS(env.add_exp(Atom(1.0), b), SemanticError);
}

TEST_CASE( "Test get built-in procedure", "[environment]" ) {
  Environment env;

  INFO("default procedure")
  Procedure p1 = env.get_proc(Atom("doesnotexist"));
  Procedure p2 = env.get_proc(Atom("alsodoesnotexist"));
  REQUIRE(p1 == p2);
  std::vector<Expression> args;
  REQUIRE(p1(args) == Expression());
  REQUIRE(p2(args) == Expression());

  INFO("trying add procedure")
  Procedure padd = env.get_proc(Atom("+"));
  args.emplace_back(1.0);
  args.emplace_back(2.0);
  REQUIRE(padd(args) == Expression(3.0));
}

TEST_CASE("", ""){
	Environment env;
	env.add_exp(Atom("hello"), Expression(1.0));
	Environment newEnv;
	newEnv = env;
	REQUIRE(newEnv.is_exp(Atom("hello")));
}

TEST_CASE("Test the add procedure: add", "[environment]") {
	Environment env;
	Procedure padd = env.get_proc(Atom("+"));
	std::vector<Expression> args;

	INFO("Testing real add for >2 arguments")
	args.emplace_back(1.0);
	args.emplace_back(2.0);
	args.emplace_back(3.0);
	args.emplace_back(4.0);
	REQUIRE(padd(args) == Expression(10.0));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing imag add for 2 arguments")
	args.emplace_back(std::complex<double>(0.0, 1.0));
	args.emplace_back(std::complex<double>(3.0, 2.0));
	REQUIRE(padd(args) == Expression(std::complex<double>(3.0, 3.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing imag add for >2 arguments")
	args.emplace_back(std::complex<double>(1.0, 5.0));
	args.emplace_back(std::complex<double>(2.0, 6.0));
	args.emplace_back(std::complex<double>(3.0, 7.0));
	args.emplace_back(std::complex<double>(4.0, 8.0));
	REQUIRE(padd(args) == Expression(std::complex<double>(10.0, 26.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing real add to imag add")
	args.emplace_back(2.0);
	args.emplace_back(std::complex<double>(2.0, 6.0));
	REQUIRE(padd(args) == Expression(std::complex<double>(4.0, 6.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing for throw error argument: not a number")
	args.emplace_back(4.0);
	args.emplace_back(std::string("String"));
	REQUIRE_THROWS_AS(padd(args), SemanticError); 
}

TEST_CASE("Test the muliply procedure: mul", "[environment]") {
	Environment env;
	Procedure pmul = env.get_proc(Atom("*"));
	std::vector<Expression> args;

	INFO("Testing real mul for 2 arguments")
	args.emplace_back(2.0);
	args.emplace_back(4.0);
	REQUIRE(pmul(args) == Expression(8.0));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing real mul for >2 arguments")
	args.emplace_back(1.0);
	args.emplace_back(2.0);
	args.emplace_back(3.0);
	args.emplace_back(4.0);
	REQUIRE(pmul(args) == Expression(24.0));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing imag mul for 2 arguments")
	args.emplace_back(std::complex<double>(1.0, 3.0));
	args.emplace_back(std::complex<double>(2.0, 5.0));
	REQUIRE(pmul(args) == Expression(std::complex<double>(-13.0, 11.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing imag mul for >2 arguments")
	args.emplace_back(std::complex<double>(1.0, 5.0));
	args.emplace_back(std::complex<double>(2.0, 6.0));
	args.emplace_back(std::complex<double>(3.0, 7.0));
	args.emplace_back(std::complex<double>(4.0, 8.0));
	REQUIRE(pmul(args) == Expression(std::complex<double>(400.0, -2160.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing real mul to imag mul")
	args.emplace_back(2.0);
	args.emplace_back(std::complex<double>(2.0, 6.0));
	REQUIRE(pmul(args) == Expression(std::complex<double>(4.0, 12.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing mul for throw error argument: not a number")
	args.emplace_back(4.0);
	args.emplace_back(std::string("NotANumber"));
	REQUIRE_THROWS_AS(pmul(args), SemanticError);
}

TEST_CASE("Test the negative and subtract procedure: subneg", "[environment]") {
	Environment env;
	Procedure psubneg = env.get_proc(Atom("-"));
	std::vector<Expression> args;

	INFO("Testing real subneg for 1 argument, positive->negative")
	args.emplace_back(1.0);
	REQUIRE(psubneg(args) == Expression(-1.0));

	args.clear();
	REQUIRE(args.empty());
	
	INFO("Testing real subneg for 1 argument, negative->positive")
	args.emplace_back(-1.0);
	REQUIRE(psubneg(args) == Expression(1.0));
	
	args.clear();
	REQUIRE(args.empty());

	INFO("Testing imag subneg for 1 argument, no real part, positive->negative")
	args.emplace_back(std::complex<double>(0.0, 2.0));
	REQUIRE(psubneg(args) == Expression(std::complex<double>(0.0, -2.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing imag subneg for 1 argument, no real part, negative->positive")
	args.emplace_back(std::complex<double>(0.0, -2.0));
	REQUIRE(psubneg(args) == Expression(std::complex<double>(0.0, 2.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing imag subneg for 1 argument, real negative->positive, imag negative->positive")
	args.emplace_back(std::complex<double>(-5.0, -23.0));
	REQUIRE(psubneg(args) == Expression(std::complex<double>(5.0, 23.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing imag subneg for 1 argument, real positive->negative, imag negative->positive")
	args.emplace_back(std::complex<double>(5.0, -23.0));
	REQUIRE(psubneg(args) == Expression(std::complex<double>(-5.0, 23.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing imag subneg for 1 argument, real negative->positive, imag positive->negative")
	args.emplace_back(std::complex<double>(-5.0, 23.0));
	REQUIRE(psubneg(args) == Expression(std::complex<double>(5.0, -23.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing imag subneg for 1 argument, real positive->negative, imag positive->negative")
		args.emplace_back(std::complex<double>(5.0, 23.0));
	REQUIRE(psubneg(args) == Expression(std::complex<double>(-5.0, -23.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing real subneg for 2 arguments")
	args.emplace_back(2.0);
	args.emplace_back(4.0);
	REQUIRE(psubneg(args) == Expression(-2.0));
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(4.0);
	args.emplace_back(2.0);
	REQUIRE(psubneg(args) == Expression(2.0));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing imag subneg for 2 arguments")
	args.emplace_back(std::complex<double>(1.0, 3.0));
	args.emplace_back(std::complex<double>(2.0, 5.0));
	REQUIRE(psubneg(args) == Expression(std::complex<double>(-1.0, -2.0)));
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(std::complex<double>(2.0, 5.0));
	args.emplace_back(std::complex<double>(1.0, 3.0));
	REQUIRE(psubneg(args) == Expression(std::complex<double>(1.0, 2.0)));
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(std::complex<double>(-2.0, -5.0));
	args.emplace_back(std::complex<double>(1.0, 3.0));
	REQUIRE(psubneg(args) == Expression(std::complex<double>(-3.0, -8.0)));
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(std::complex<double>(2.0, 5.0));
	args.emplace_back(std::complex<double>(-1.0, -3.0));
	REQUIRE(psubneg(args) == Expression(std::complex<double>(3.0, 8.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing subneg for number - complex")
	args.emplace_back(4.0);
	args.emplace_back(std::complex<double>(2.0, 2.0));
	REQUIRE(psubneg(args) == Expression(std::complex<double>(2.0, -2.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing imag sub to real sub")
	args.emplace_back(std::complex<double>(2.0, 6.0));
	args.emplace_back(2.0);
	REQUIRE(psubneg(args) == Expression(std::complex<double>(0.0, 6.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing subneg for throw error argument: not a number (negate)")
	args.emplace_back(std::string("NotPossible"));
	REQUIRE_THROWS_AS(psubneg(args), SemanticError);

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing subneg for throw error argument: not a number (subtraction)")
	args.emplace_back(4.0);
	args.emplace_back(std::string("NotANumber"));
	REQUIRE_THROWS_AS(psubneg(args), SemanticError);

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing subneg for throw error argument: >2 arguments")
	args.emplace_back(10.0);
	args.emplace_back(4.0);
	args.emplace_back(-2.0);
	REQUIRE_THROWS_AS(psubneg(args), SemanticError);
}

TEST_CASE("Test the division procedure: div", "[environment]") {
	Environment env;
	Procedure pdiv = env.get_proc(Atom("/"));
	std::vector<Expression> args;

	INFO("Testing div for 1 argument");
	args.emplace_back(2.0);
	REQUIRE(pdiv(args) == Expression(0.5));
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(std::complex<double>(0.0, 1.0));
	REQUIRE(pdiv(args) == Expression(std::complex<double>(0.0, -1.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing real div for 2 arguments")
	args.emplace_back(4.0);
	args.emplace_back(2.0);
	REQUIRE(pdiv(args) == Expression(2.0));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing imag div for 2 arguments")
	args.emplace_back(std::complex<double>(4.0, 4.0));
	args.emplace_back(std::complex<double>(2.0, 2.0));
	REQUIRE(pdiv(args) == Expression(std::complex<double>(2.0, 0.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing real/imag for 2 arguments")
	args.emplace_back(4.0);
	args.emplace_back(std::complex<double>(2.0, 2.0));
	REQUIRE(pdiv(args) == Expression(std::complex<double>(1.0, -1.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing imag/real for 2 arguments")
	args.emplace_back(std::complex<double>(2.0, 2.0));
	args.emplace_back(4.0);
	REQUIRE(pdiv(args) == Expression(std::complex<double>(0.5, 0.5)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing div for throw error argument: not a number")
	args.emplace_back(4.0);
	args.emplace_back(std::string("NotANumber"));
	REQUIRE_THROWS_AS(pdiv(args), SemanticError);
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(std::string("oops"));
	REQUIRE_THROWS_AS(pdiv(args), SemanticError);

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing div for throw error argument: too many args:")
	args.emplace_back(4.0);
	args.emplace_back(2.0);
	args.emplace_back(12.0);
	REQUIRE_THROWS_AS(pdiv(args), SemanticError);
}

TEST_CASE("Test the real procedure: real", "[environment]") {
	Environment env;
	Procedure preal = env.get_proc(Atom("real"));
	std::vector<Expression> args;

	INFO("Testing real for real of complex")
	args.emplace_back(std::complex<double>(15.0, 12.0));
	REQUIRE(preal(args) == Expression(15.0));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing real for throw error argument: Not Complex")
	args.emplace_back(4.0);
	REQUIRE_THROWS_AS(preal(args), SemanticError);

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing real for throw error argument: >1 arguments")
	args.emplace_back(std::complex<double>(12.0, 2.0));
	args.emplace_back(std::complex<double>(2.0, 4.0));
	REQUIRE_THROWS_AS(preal(args), SemanticError);
}

TEST_CASE("Test the imaginary procedure: imag", "[environment]") {
	Environment env;
	Procedure pimag = env.get_proc(Atom("imag"));
	std::vector<Expression> args;

	INFO("Testing imag for imaginary of complex")
	args.emplace_back(std::complex<double>(15.0, 12.0));
	REQUIRE(pimag(args) == Expression(12.0));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing imag for throw error argument: Not Complex")
	args.emplace_back(4.0);
	REQUIRE_THROWS_AS(pimag(args), SemanticError);

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing imag for throw error argument: >1 arguments")
	args.emplace_back(std::complex<double>(12.0, 2.0));
	args.emplace_back(std::complex<double>(2.0, 4.0));
	REQUIRE_THROWS_AS(pimag(args), SemanticError);
}

TEST_CASE("Test the absolute value/magnitude procedure: abs", "[environment]") {
	Environment env;
	Procedure pabs = env.get_proc(Atom("mag"));
	std::vector<Expression> args;

	INFO("Testing abs for magnitude of complex")
	args.emplace_back(std::complex<double>(0.0, 1.0));
	REQUIRE(pabs(args) == Expression(1.0));

	args.clear();
	REQUIRE(args.empty());

	/*INFO("Testing abs for magnitude of complex")
	args.emplace_back(std::complex<double>(2.0, 2.0));
	REQUIRE(pabs(args) == Expression(2.82843));

	args.clear();
	REQUIRE(args.empty());*/
	
	INFO("Testing abs for throw error argument: Not Complex")
	args.emplace_back(4.0);
	REQUIRE_THROWS_AS(pabs(args), SemanticError);

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing abs for throw error argument: >1 arguments")
	args.emplace_back(std::complex<double>(12.0, 2.0));
	args.emplace_back(std::complex<double>(2.0, 4.0));
	REQUIRE_THROWS_AS(pabs(args), SemanticError);
}

TEST_CASE("Test the argument(angle/phase) procedure: arg", "[environment]") {
	Environment env;
	Procedure parg = env.get_proc(Atom("arg"));
	std::vector<Expression> args;

	INFO("Testing arg for angle of complex")
	args.emplace_back(std::complex<double>(0.0, 0.0));
	REQUIRE(parg(args) == Expression(0));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing arg for throw error argument: Not Complex")
	args.emplace_back(4.0);
	REQUIRE_THROWS_AS(parg(args), SemanticError);

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing arg for throw error argument: >1 arguments")
	args.emplace_back(std::complex<double>(12.0, 2.0));
	args.emplace_back(std::complex<double>(2.0, 4.0));
	REQUIRE_THROWS_AS(parg(args), SemanticError);
}

TEST_CASE("Test the conjugate procedure: conj", "[environment]") {
	Environment env;
	Procedure pconj = env.get_proc(Atom("conj"));
	std::vector<Expression> args;

	INFO("Testing conj for conjugate of complex")
	args.emplace_back(std::complex<double>(0.0, 1.0));
	REQUIRE(pconj(args) == Expression(std::complex<double>(0.0, -1.0)));
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(std::complex<double>(2.0, -1.0));
	REQUIRE(pconj(args) == Expression(std::complex<double>(2.0, 1.0)));
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(std::complex<double>(-2.0, -1.0));
	REQUIRE(pconj(args) == Expression(std::complex<double>(-2.0, 1.0)));

	args.clear();
	REQUIRE(args.empty());


	INFO("Testing conj for throw error argument: Not Complex")
	args.emplace_back(4.0);
	REQUIRE_THROWS_AS(pconj(args), SemanticError);

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing conj for throw error argument: >1 arguments")
	args.emplace_back(std::complex<double>(12.0, 2.0));
	args.emplace_back(std::complex<double>(2.0, 4.0));
	REQUIRE_THROWS_AS(pconj(args), SemanticError);
}



TEST_CASE("Test the sqaure root procedure: sqrt", "[environment]") {
	Environment env;
	Procedure psqrt = env.get_proc(Atom("sqrt"));
	std::vector<Expression> args;

	INFO("Testing sqrt -1, I, -I");
	args.emplace_back(std::complex<double>(-1.0, 0.0));
	REQUIRE(psqrt(args) == Expression(std::complex<double>(0.0, 1.0)));
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(std::complex<double>(0.0, 1.0));
	REQUIRE(psqrt(args) == Expression(std::complex<double>((1/sqrt(2)), (1/sqrt(2)))));
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(std::complex<double>(0.0, -1.0));
	REQUIRE(psqrt(args) == Expression(std::complex<double>((1 / sqrt(2)), -(1 / sqrt(2)))));

	args.clear();
	REQUIRE(args.empty());
	
	INFO("Testing sqrt for positive real numbers")
	args.emplace_back(4.0);
	REQUIRE(psqrt(args) == Expression(2.0));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing sqrt for negative real numbers")
	args.emplace_back(-4.0);
	REQUIRE(psqrt(args) == Expression(std::complex<double>(0.0, 2.0)));
	
	args.clear();
	REQUIRE(args.empty());

	INFO("Testing sqrt for throw error argument: >1 arguments")
	args.emplace_back(std::complex<double>(12.0, 2.0));
	args.emplace_back(std::complex<double>(2.0, 4.0));
	REQUIRE_THROWS_AS(psqrt(args), SemanticError);

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing sqrt for throw error argument: not a number or complex")
	args.emplace_back(std::string("Not it"));
	REQUIRE_THROWS_AS(psqrt(args), SemanticError);
}

TEST_CASE("Test the power procedure: pow", "[environment]") {
	Environment env;
	Procedure ppow = env.get_proc(Atom("^"));
	std::vector<Expression> args;

	INFO("Testing pow for positive real numbers")
	args.emplace_back(4.0);
	args.emplace_back(2.0);
	REQUIRE(ppow(args) == Expression(16.0));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing pow for positive^negative real numbers")
	args.emplace_back(2.0);
	args.emplace_back(-2.0);
	REQUIRE(ppow(args) == Expression(0.25));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing pow for negative to the positive real numbers")
	args.emplace_back(-2.0);
	args.emplace_back(2.0);
	REQUIRE(ppow(args) == Expression(4.0));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing pow for negative^negative real numbers")
	args.emplace_back(-2.0);
	args.emplace_back(-2.0);
	REQUIRE(ppow(args) == Expression(0.25));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing pow for imaginary^real=i")
	args.emplace_back(std::complex<double>(0.0, 1.0));
	args.emplace_back(5.0);
	REQUIRE(ppow(args) == Expression(std::complex<double>(0.0, 1.0)));
	
	args.clear();
	REQUIRE(args.empty());

	INFO("Testing pow for imaginary^real=-1")
	args.emplace_back(std::complex<double>(0.0, 1.0));
	args.emplace_back(2.0);
	REQUIRE(ppow(args) == Expression(std::complex<double>(-1.0, 0.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing pow for real^imaginary=1")
	args.emplace_back(1.0);
	args.emplace_back(std::complex<double>(0.0, 1.0));
	REQUIRE(ppow(args) == Expression(std::complex<double>(1.0, 0.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing pow for complex^complex")
	args.emplace_back(std::complex<double>(0.0, 1.0));
	args.emplace_back(std::complex<double>(0.0, 0.0));
	REQUIRE(ppow(args) == Expression(std::complex<double>(1.0, 0.0)));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testinf pow for throw error argument: not a number or complex")
	args.emplace_back(std::string("nope, not today!"));
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(ppow(args), SemanticError);

	INFO("Testing pow for throw error argument: <2 arguments")
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(ppow(args), SemanticError);
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(std::complex<double>(1.0, 1.0));
	REQUIRE_THROWS_AS(ppow(args), SemanticError);

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing pow for throw error argument: >2 arguments")
	args.emplace_back(std::complex<double>(12.0, 2.0));
	args.emplace_back(std::complex<double>(4.0, 1.0));
	args.emplace_back(std::complex<double>(2.0, 4.0));
	REQUIRE_THROWS_AS(ppow(args), SemanticError);
}

TEST_CASE("Test the natural log procedure: ln", "[environment]") {
	Environment env;
	Procedure pln = env.get_proc(Atom("ln"));
	std::vector<Expression> args;
	
	INFO("Testing ln for argument > 0 and real")
	args.emplace_back(1.0);
	REQUIRE(pln(args) == Expression(0.0));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing ln for throw error argument: argument is imaginary")
	args.emplace_back(std::complex<double>(1.0, 0.0));
	REQUIRE_THROWS_AS(pln(args), SemanticError);

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing ln for throw error argument: argument <= 0")
	args.emplace_back(0.0);
	REQUIRE_THROWS_AS(pln(args), SemanticError);
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(-1.0);
	REQUIRE_THROWS_AS(pln(args), SemanticError);

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing ln for throw error argument: >1 arguments")
	args.emplace_back(2.0);
	args.emplace_back(4.0);
	REQUIRE_THROWS_AS(pln(args), SemanticError);
}

TEST_CASE("Test the sine procedure: sin", "[environment]") {
	Environment env;
	Procedure psin = env.get_proc(Atom("sin"));
	std::vector<Expression> args;

	INFO("Testing sin for argument is real")
	args.emplace_back((atan(1)*4)/2);
	REQUIRE(psin(args) == Expression(1.0));
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(0.0);
	REQUIRE(psin(args) == Expression(0.0));
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(atan(1) * 4);
	REQUIRE(psin(args) == Expression(0.0));
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(3 * (atan(1) * 4) / 2);
	REQUIRE(psin(args) == Expression(-1.0));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing sin for throw error argument: argument is imaginary")
	args.emplace_back(std::complex<double>(1.0, 0.0));
	REQUIRE_THROWS_AS(psin(args), SemanticError);

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing sin for throw error argument: >1 arguments")
	args.emplace_back(2.0);
	args.emplace_back(4.0);
	REQUIRE_THROWS_AS(psin(args), SemanticError);
}

TEST_CASE("Test the cosine procedure: cos", "[environment]") {
	Environment env;
	Procedure pcos = env.get_proc(Atom("cos"));
	std::vector<Expression> args;

	INFO("Testing cos for argument is real")
	args.emplace_back((atan(1) * 4) / 2);
	REQUIRE(pcos(args) == Expression(0.0));
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(0.0);
	REQUIRE(pcos(args) == Expression(1.0));
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(atan(1) * 4);
	REQUIRE(pcos(args) == Expression(-1.0));
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(3 * (atan(1) * 4) / 2);
	REQUIRE(pcos(args) == Expression(0.0));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing cos for throw error argument: argument is imaginary")
	args.emplace_back(std::complex<double>(1.0, 0.0));
	REQUIRE_THROWS_AS(pcos(args), SemanticError);

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing cos for throw error argument: >1 arguments")
	args.emplace_back(2.0);
	args.emplace_back(4.0);
	REQUIRE_THROWS_AS(pcos(args), SemanticError);
}

TEST_CASE("Test the tangent procedure: tan", "[environment]") {
	Environment env;
	Procedure ptan = env.get_proc(Atom("tan"));
	std::vector<Expression> args;

	INFO("Testing tan for argument is real")
	args.emplace_back(0.0);
	REQUIRE(ptan(args) == Expression(0.0));
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(atan(1));
	REQUIRE(ptan(args) == Expression(1.0));
	args.clear();
	REQUIRE(args.empty());
	args.emplace_back(-atan(1));
	REQUIRE(ptan(args) == Expression(-1.0));

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing tan for throw error argument: argument is imaginary")
	args.emplace_back(std::complex<double>(1.0, 0.0));
	REQUIRE_THROWS_AS(ptan(args), SemanticError);

	args.clear();
	REQUIRE(args.empty());

	INFO("Testing tan for throw error argument: >1 arguments")
	args.emplace_back(2.0);
	args.emplace_back(4.0);
	REQUIRE_THROWS_AS(ptan(args), SemanticError);

	args.clear();
	REQUIRE(args.empty());

	//INFO("Testing tan for throw error argument: argument is undefined")
	//args.emplace_back((atan(1) * 4) / 2);
	//REQUIRE_THROWS_AS(ptan(args), SemanticError);
}

TEST_CASE("Test the list procedure: list", "[environment]") {
	Environment env;
	Procedure plist = env.get_proc(Atom("list"));
	std::vector<Expression> args1 = { Expression(0), Expression(1), Expression(2), Expression(3) };
	std::vector<Expression> args2 = { Expression(0), Expression(1), Expression(2), Expression(3) };
	REQUIRE(plist(args1).isHeadList());
	REQUIRE(plist(args2).isHeadList());
	REQUIRE(plist(args1) == plist(args2));
	REQUIRE(plist(args1) == Expression(args2));
	REQUIRE(plist(args2) == plist(args1));
	REQUIRE(plist(args2) == Expression(args1));
	std::vector<Expression> args3 = {};
	REQUIRE(plist(args3).isHeadList());
}

TEST_CASE("Test the list procedure: first", "[environment]") {
	Environment env;
	Procedure plist = env.get_proc(Atom("list"));
	Procedure pfirst = env.get_proc(Atom("first"));

	INFO("Testing first for proper outcome")
	std::vector<Expression> args1 = { Expression(1), Expression(2), Expression(3) };
	std::vector<Expression> args2 = { plist(args1) };
	REQUIRE(pfirst(args2) == Expression(1));
	
	INFO("Testing first for multiple arguments")
	std::vector<Expression> args3 = { Expression(1), Expression(2), Expression(3) };
	std::vector<Expression> args4 = { plist(args1), plist(args3) };
	REQUIRE_THROWS_AS(pfirst(args4), SemanticError);

	INFO("Testing first for argument to first is an empty list")
	std::vector<Expression> args5 = { };
	std::vector<Expression> args6 = { plist(args5) };
	REQUIRE_THROWS_AS(pfirst(args6), SemanticError);

	INFO("Testing first for argument to first is not a list")
	std::vector<Expression> args7 = { Expression(1) };
	REQUIRE_THROWS_AS(pfirst(args7), SemanticError);
}

TEST_CASE("Test the list procedure: rest", "[environment]") {
	Environment env;
	Procedure plist = env.get_proc(Atom("list"));
	Procedure prest = env.get_proc(Atom("rest"));

	INFO("Testing rest for proper outcome")
	std::vector<Expression> args1 = { Expression(1), Expression(2), Expression(3) };
	std::vector<Expression> args2 = { plist(args1) };
	std::vector<Expression> args3 = { Expression(2), Expression(3) };
	REQUIRE(prest(args2) == plist(args3));

	INFO("Testing rest for multiple arguments")
	std::vector<Expression> args4 = { plist(args1), plist(args3) };
	REQUIRE_THROWS_AS(prest(args4), SemanticError);

	INFO("Testing rest for argument to rest is an empty list")
	std::vector<Expression> args5 = {};
	std::vector<Expression> args6 = { plist(args5) };
	REQUIRE_THROWS_AS(prest(args6), SemanticError);

	INFO("Testing rest for argument to rest is not a list")
	std::vector<Expression> args7 = { Expression(1) };
	REQUIRE_THROWS_AS(prest(args7), SemanticError);
}

TEST_CASE("Test the list procedure: length", "[environment]") {
	Environment env;
	Procedure plist = env.get_proc(Atom("list"));
	Procedure plength = env.get_proc(Atom("length"));

	INFO("Testing length for proper outcome")
	std::vector<Expression> args1 = { Expression(1), Expression(2), Expression(3) };
	std::vector<Expression> args2 = { plist(args1) };
	REQUIRE(plength(args2) == Expression(3));

	INFO("Testing length for multiple arguments")
	std::vector<Expression> args3 = { Expression(2), Expression(4), Expression(6) };
	std::vector<Expression> args4 = { plist(args1), plist(args3) };
	REQUIRE_THROWS_AS(plength(args4), SemanticError);

	INFO("Testing length for argument to length special case")
	std::vector<Expression> args5 = {};
	std::vector<Expression> args6 = { plist(args5) };
	REQUIRE(plength(args6) == Expression(0));

	INFO("Testing length for argument to length is not a list")
	std::vector<Expression> args7 = { Expression(1) };
	REQUIRE_THROWS_AS(plength(args7), SemanticError);
}

TEST_CASE("Test the list procedure: append", "[environment]") {
	Environment env;
	Procedure plist = env.get_proc(Atom("list"));
	Procedure pappend = env.get_proc(Atom("append"));

	INFO("Testing append for proper outcome")
	std::vector<Expression> args1 = { Expression(1), Expression(2), Expression(3) };
	std::vector<Expression> args2 = { plist(args1), Expression(2) };
	std::vector<Expression> args3 = { Expression(1), Expression(2), Expression(3), Expression(2) };
	REQUIRE(pappend(args2) == plist(args3));

	INFO("Testing append for multiple arguments")
	std::vector<Expression> args4 = { Expression(2), plist(args3) };
	REQUIRE_THROWS_AS(pappend(args4), SemanticError);

	INFO("Testing append for argument to append empty list special case")
	std::vector<Expression> args5 = {};
	std::vector<Expression> args6 = { plist(args5), Expression(2) };
	std::vector<Expression> args7 = { Expression(2) };
	REQUIRE(pappend(args6) == plist(args7));

	INFO("Testing append for argument to append is not a list")
	std::vector<Expression> args8 = { Expression(1) };
	REQUIRE_THROWS_AS(pappend(args8), SemanticError);
}

TEST_CASE("Test the list procedure: join", "[environment]") {
	Environment env;
	Procedure plist = env.get_proc(Atom("list"));
	Procedure pjoin = env.get_proc(Atom("join"));

	INFO("Testing join for proper outcome")
	std::vector<Expression> args1 = { Expression(1), Expression(2) };
	std::vector<Expression> args2 = { Expression(3), Expression(4) };
	std::vector<Expression> args3 = { plist(args1), plist(args2) };
	std::vector<Expression> args4 = { Expression(1), Expression(2), Expression(3), Expression(4) };
	REQUIRE(pjoin(args3) == plist(args4));

	INFO("Testing join for multiple arguments or too few arguments")
	std::vector<Expression> args5 = { plist(args1), plist(args2), plist(args3) };
	REQUIRE_THROWS_AS(pjoin(args5), SemanticError);
	std::vector<Expression> args6 = { plist(args1) };
	REQUIRE_THROWS_AS(pjoin(args6), SemanticError);

	INFO("Testing join for argument to join empty list special case")
	std::vector<Expression> args7 = {};
	std::vector<Expression> args8 = {};
	std::vector<Expression> args9 = { plist(args7), plist(args8) };
	REQUIRE(pjoin(args9) == plist(args8));

	INFO("Testing join for argument to join is not a list")
	std::vector<Expression> args10 = { Expression(1) };
	REQUIRE_THROWS_AS(pjoin(args10), SemanticError);
}

TEST_CASE("Test the list procedure: range", "[environment]") {
	Environment env;
	Procedure plist = env.get_proc(Atom("list"));
	Procedure prange = env.get_proc(Atom("range"));

	INFO("Testing range for proper outcome")
	std::vector<Expression> args1 = { Expression(1), Expression(5), Expression(1) };
	std::vector<Expression> args2 = { Expression(1), Expression(2), Expression(3), Expression(4), Expression(5) };
	REQUIRE(prange(args1) == plist(args2));

	INFO("Testing range for multiple throw arguments: too many or too few arguments")
	std::vector<Expression> args3 = { Expression(1), Expression(10), Expression(2), Expression(4) };
	REQUIRE_THROWS_AS(prange(args3), SemanticError);
	std::vector<Expression> args4 = { Expression(1) };
	REQUIRE_THROWS_AS(prange(args4), SemanticError);
	std::vector<Expression> args5 = {};
	REQUIRE_THROWS_AS(prange(args5), SemanticError);

	INFO("Testing range for throw argument: range negative range increase")
	std::vector<Expression> args6 = { Expression(1), Expression(10), Expression(-1) };
	REQUIRE_THROWS_AS(prange(args6), SemanticError);

	INFO("Testing range for throw argument: begin range than end range.")
	std::vector<Expression> args7 = { Expression(10), Expression(1), Expression(1) };
	REQUIRE_THROWS_AS(prange(args7), SemanticError);

	INFO("Testing range for throw argument: not a number")
	std::vector<Expression> args8;
	args8.emplace_back(std::complex<double>(0.0, 0.0));
	args8.emplace_back(Expression(10));
	args8.emplace_back(Expression(1));
	REQUIRE_THROWS_AS(prange(args8), SemanticError);

}

TEST_CASE( "Test reset", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);

  env.reset();
  REQUIRE(!env.is_known(Atom("one")));
  REQUIRE(!env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == Expression());
  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());
}

TEST_CASE( "Test semeantic errors", "[environment]" ) {

  Environment env;

  {
    Expression exp(Atom("begin"));
    
    REQUIRE_THROWS_AS(exp.eval(env), SemanticError);
  }
}

