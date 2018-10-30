#include "catch.hpp"

#include "atom.hpp"

TEST_CASE( "Test constructors", "[atom]" ) {

  {
    INFO("Default Constructor");
    Atom a;

    REQUIRE(a.isNone());
    REQUIRE(!a.isNumber());
    REQUIRE(!a.isSymbol());
	REQUIRE(!a.isComplex());
  }

  {
    INFO("Number Constructor");
    Atom a(1.0);

    REQUIRE(!a.isNone());
    REQUIRE(a.isNumber());
    REQUIRE(!a.isSymbol());
	REQUIRE(!a.isComplex());
  }

  {
    INFO("Symbol Constructor");
    Atom a("hi");

    REQUIRE(!a.isNone());
    REQUIRE(!a.isNumber());
    REQUIRE(a.isSymbol());
	REQUIRE(!a.isComplex());
  }

  {
	  INFO("Complex Constructor")
	  Atom a(std::complex<double>(1.0, 1.0));

	  REQUIRE(!a.isNone());
	  REQUIRE(!a.isNumber());
	  REQUIRE(!a.isSymbol());
	  REQUIRE(a.isComplex());

  }
  
  {
    INFO("Token Constructor");
    Token t("hi");
    Atom a(t);

    REQUIRE(!a.isNone());
    REQUIRE(!a.isNumber());
    REQUIRE(a.isSymbol());
  }

  {
	  INFO("Copy Constructor");
	  Atom a("hi");
	  Atom b(1.0);
	  Atom c(std::complex<double>(1.0, 1.0));
	  Atom d(std::string("\"hi\""));

	  Atom e = a;
	  REQUIRE(!a.isNone());
	  REQUIRE(!e.isNumber());
	  REQUIRE(e.isSymbol());
	  REQUIRE(!e.isComplex());
	  REQUIRE(!e.isString());

	  Atom f = b;
	  REQUIRE(!a.isNone());
	  REQUIRE(f.isNumber());
	  REQUIRE(!f.isSymbol());
	  REQUIRE(!f.isComplex());
	  REQUIRE(!f.isString());

	  Atom g = c;
	  REQUIRE(!c.isNone());
	  REQUIRE(!g.isNumber());
	  REQUIRE(!g.isSymbol());
	  REQUIRE(g.isComplex());
	  REQUIRE(!g.isString());

	  Atom h = d;
	  REQUIRE(!d.isNone());
	  REQUIRE(!h.isNumber());
	  REQUIRE(!h.isSymbol());
	  REQUIRE(!h.isComplex());
	  REQUIRE(h.isString());
  }
}

TEST_CASE( "Test assignment", "[atom]" ) {

  {
    INFO("default to default");
    Atom a;
    Atom b;
    b = a;
    REQUIRE(b.isNone());
    REQUIRE(!b.isNumber());
    REQUIRE(!b.isSymbol());
  }

  {
    INFO("default to number");
    Atom a;
    Atom b(1.0);
    b = a;
    REQUIRE(b.isNone());
    REQUIRE(!b.isNumber());
    REQUIRE(!b.isSymbol());
  }

  {
    INFO("default to symbol");
    Atom a;
    Atom b("hi");
    b = a;
    REQUIRE(b.isNone());
    REQUIRE(!b.isNumber());
    REQUIRE(!b.isSymbol());
  }

  {
    INFO("number to default");
    Atom a(1.0);
    Atom b;
    b = a;
    REQUIRE(b.isNumber());
    REQUIRE(b.asNumber() == 1.0);
  }

  {
    INFO("number to number");
    Atom a(1.0);
    Atom b(2.0);
    b = a;
    REQUIRE(b.isNumber());
    REQUIRE(b.asNumber() == 1.0);
  }

  {
    INFO("number to symbol");
    Atom a("hi");
    Atom b(1.0);
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
    INFO("symbol to default");
    Atom a("hi");
    Atom b;
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
    INFO("symbol to number");
    Atom a("hi");
    Atom b(1.0);
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
    INFO("symbol to symbol");
    Atom a("hi");
    Atom b("bye");
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
	  INFO("complex to complex");
	  Atom a(std::complex<double>(1.0, 1.0));
	  Atom b(std::complex<double>(1.0, 1.0));
	  b = a;
	  REQUIRE(b.isComplex());
	  REQUIRE(b.asComplex() == std::complex<double>(1.0, 1.0));
  }

  {
	  INFO("string to string");
	  Atom a("\"hi\"");
	  Atom b("\"hi\"");
	  b = a;
	  REQUIRE(b.isString());
	  REQUIRE(b.asString() == std::string("\"hi\""));
  }
}

TEST_CASE( "test comparison", "[atom]" ) {

  {
    INFO("compare default to default");
    Atom a;
    Atom b;
    REQUIRE(a == b);
  }

  {
    INFO("compare default to number");
    Atom a;
    Atom b(1.0);
    REQUIRE(a != b);
  }

  {
    INFO("compare default to symbol");
    Atom a;
    Atom b("hi");
    REQUIRE(a != b);
  }

  {
    INFO("compare number to default");
    Atom a(1.0);
    Atom b;
    REQUIRE(a != b);
  }

  {
    INFO("compare number to number");
    Atom a(1.0);
    Atom b(1.0);
    Atom c(2.0);
    REQUIRE(a == b);
    REQUIRE(a != c);
  }

  {
    INFO("compare number to symbol");
    Atom a(1.0);
    Atom b("hi");
    REQUIRE(a != b);
  }

  {
    INFO("compare symbol to default");
    Atom a("hi");
    Atom b;
    REQUIRE(a != b);
  }

  {
    INFO("compare symbol to number");
    Atom a("hi");
    Atom b(1.0);
    REQUIRE(a != b);
  }

  {
    INFO("compare symbol to symbol");
    Atom a("hi");
    Atom b("hi");
    Atom c("bye");
    REQUIRE(a == b);
    REQUIRE(a != c);
  }

}





