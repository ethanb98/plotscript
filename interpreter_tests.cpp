#include "catch.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "semantic_error.hpp"
#include "interpreter.hpp"
#include "expression.hpp"

Expression run(const std::string & program){
  
  std::istringstream iss(program);
    
  Interpreter interp;
    
  bool ok = interp.parseStream(iss);
  if(!ok){
    std::cerr << "Failed to parse: " << program << std::endl; 
  }
  REQUIRE(ok == true);

  Expression result;
  REQUIRE_NOTHROW(result = interp.evaluate());

  return result;
}

TEST_CASE( "Test Interpreter parser with expected input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r)))";

  std::istringstream iss(program);
 
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == true);
}

TEST_CASE( "Test Interpreter parser with numerical literals", "[interpreter]" ) {

  std::vector<std::string> programs = {"(1)", "(+1)", "(+1e+0)", "(1e-0)"};
  
  for(auto program : programs){
    std::istringstream iss(program);
 
    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == true);
  }

  {
    std::istringstream iss("(define x 1abc)");
    
    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with truncated input", "[interpreter]" ) {

  {
    std::string program = "(f";
    std::istringstream iss(program);
  
    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
  
  {
    std::string program = "(begin (define r 10) (* pi (* r r";
    std::istringstream iss(program);

    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with extra input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r))) )";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with single non-keyword", "[interpreter]" ) {

  std::string program = "hello";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty input", "[interpreter]" ) {

  std::string program;
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty expression", "[interpreter]" ) {

  std::string program = "( )";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with bad number string", "[interpreter]" ) {

  std::string program = "(1abc)";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with incorrect input. Regression Test", "[interpreter]" ) {

  std::string program = "(+ 1 2) (+ 3 4)";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter result with literal expressions", "[interpreter]" ) {
  
  { // Number
    std::string program = "(4)";
    Expression result = run(program);
    REQUIRE(result == Expression(4.));
  }

  { // Symbol
    std::string program = "(pi)";
    Expression result = run(program);
    REQUIRE(result == Expression(atan2(0, -1)));
  }

}

TEST_CASE( "Test Interpreter result with simple procedures (add)", "[interpreter]" ) {

  { // add, binary case
    std::string program = "(+ 1 2)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(3.));
  }
  
  { // add, 3-ary case
    std::string program = "(+ 1 2 3)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(6.));
  }

  { // add, 6-ary case
    std::string program = "(+ 1 2 3 4 5 6)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(21.));
  }
}
  
TEST_CASE( "Test Interpreter special forms: begin and define", "[interpreter]" ) {

  {
    std::string program = "(define answer 42)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }

  {
    std::string program = "(begin (define answer 42)\n(answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }
  
  {
    std::string program = "(begin (define answer (+ 9 11)) (answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(20.));
  }

  {
    std::string program = "(begin (define a 1) (define b 1) (+ a b))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(2.));
  }
}

TEST_CASE( "Test a medium-sized expression", "[interpreter]" ) {

  {
    std::string program = "(+ (+ 10 1) (+ 30 (+ 1 1)))";
    Expression result = run(program);
    REQUIRE(result == Expression(43.));
  }
}

TEST_CASE( "Test arithmetic procedures", "[interpreter]" ) {

  {
    std::vector<std::string> programs = {"(+ 1 -2)",
					 "(+ -3 1 1)",
					 "(- 1)",
					 "(- 1 2)",
					 "(* 1 -1)",
					 "(* 1 1 -1)",
					 "(/ -1 1)",
					 "(/ 1 -1)"};

    for(auto s : programs){
      Expression result = run(s);
      REQUIRE(result == Expression(-1.));
    }
  }
}


TEST_CASE( "Test some semantically invalid expresions", "[interpreter]" ) {
  
  std::vector<std::string> programs = {"(@ none)", // so such procedure
				       "(- 1 1 2)", // too many arguments
				       "(define begin 1)", // redefine special form
				       "(define pi 3.14)"}; // redefine builtin symbol
    for(auto s : programs){
      Interpreter interp;

      std::istringstream iss(s);
      
      bool ok = interp.parseStream(iss);
      REQUIRE(ok == true);
      
      REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
    }
}

TEST_CASE( "Test for exceptions from semantically incorrect input", "[interpreter]" ) {

  std::string input = R"(
(+ 1 a)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test malformed define", "[interpreter]" ) {

    std::string input = R"(
(define a 1 2)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test using number as procedure", "[interpreter]" ) {
    std::string input = R"(
(1 2 3)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test input of lambda", "[interpreter]") {

	std::string program = "(begin (define a 1) (define x 100) (define f (lambda (x) (begin (define b 12) (+ a b x)))) (f 2) )";
	INFO(program);
	Expression result = run(program);
	REQUIRE(result == Expression(15.));
}

TEST_CASE("Test of lambda begin", "[interpreter]") {
	std::string program = "(begin (lambda (x) (* 2 x)) (define f1 (lambda (x y) (/ x y))) (f1 4 2))";

	INFO(program);
	Expression result = run(program);
	REQUIRE(result == Expression(2.));
}

TEST_CASE("Test input of lambda apply", "[interpreter]") {
	std::string program = "(apply + (list 1 2 3 4))";
	INFO(program);
	Expression result = run(program);
	REQUIRE(result == Expression(10.));
}

TEST_CASE("Test input of lambda apply begin", "[interpreter]") {
	std::string program = "(begin (define complexAsList(lambda(x) (list(real x) (imag x)))) (apply complexAsList (list (+ 1 (* 3 I)))))";
	INFO(program);
	Expression result = run(program);
	std::vector<Expression> args1 = { Expression(1), Expression(3) };
	REQUIRE(result == Expression(args1));
}

TEST_CASE("Testing list", "[interpreter]") {
	std::string program = "(list 1 2 3)";
	INFO(program);
	Expression result = run(program);
	std::vector<Expression> args1 = { Expression(1), Expression(2), Expression(3) };
	REQUIRE(result == Expression(args1));
}

TEST_CASE("Test lambda to pass", "[interpreter]") {
	std::string program = "(begin (define a (lambda (x y) (+ x y))) (a 2 3))";
	INFO(program);
	std::istringstream iss(program);
	Interpreter interp;
	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE(interp.evaluate() == Expression(5));
}

TEST_CASE("Testing transferString", "[interpreter]") {
	std::string program = "(* 1 2 (+ 2 3))";
	Expression result = run(program);
	std::string ans = "(10)";
	REQUIRE(ans == result.transferString());
}

TEST_CASE("Testing transferString with string", "[interpreter]") {
	std::string program = "(get-property \"hello\" (+ 2 3))";
	Expression result = run(program);
	std::string ans = "NONE";
	REQUIRE(ans == result.transferString());
}


TEST_CASE("Test lambda apply input error second argument not list", "[interpreter]") {
	std::string program = "(apply + 3)";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test lambda apply input error first argument not procedure", "[interpreter]") {
	std::string program = "(apply (+ z I) (list 0))";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test lambda apply input error bad division case", "[interpreter]") {
	std::string program = "(apply / (list 1 2 4))";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test input of map begin", "[interpreter]") {
	std::string program = "(begin (define f (lambda (x) (sin x))) (map f (list (/ (- pi) 2) 0 (/ pi 2))))";
	INFO(program);
	std::vector<Expression> args1 = { Expression(-1), Expression(0), Expression(1) };
	Expression result = run(program);
	REQUIRE(result == Expression(args1));
}

TEST_CASE("Test input map unary divide", "[interpreter]") {
	std::string program = "(map / (list 1 2 4))";
	INFO(program);
	std::vector<Expression> args1 = { Expression(1.), Expression(0.5), Expression(0.25) };
	Expression result = run(program);
	REQUIRE(result == Expression(args1));
}

/*TEST_CASE("Test input for discrete-plot", "[interpreter]") {
	std::string program = "(discrete-plot (list (list -1 -1) (list 1 1)) (list (list \"title\" \"The-Title\") (list \"abscissa-label\" \"X-Label\") (list \"ordinate-label\" \"Y-Label\")))";
	INFO(program);
	std::vector<Expression> point1 = { Expression(-10), Expression(10) };
	std::vector<Expression> line1p1 = { Expression(-10), Expression(0) };
	std::vector<Expression> line1p2 = { Expression(-10), Expression(10) };
	std::vector<Expression> line1 = { Expression(line1p1), Expression(line1p2) };
	std::vector<Expression> point2 = { Expression(10), Expression(-10) };
	std::vector<Expression> line2p1 = { Expression(10), Expression(0) };
	std::vector<Expression> line2p2 = { Expression(10), Expression(-10) };
	std::vector<Expression> line2 = { Expression(line2p1), Expression(line2p2) };
	std::vector<Expression> line3p1 = { Expression(0), Expression(10) };
	std::vector<Expression> line3p2 = { Expression(0), Expression(-10) };
	std::vector<Expression> line3 = { Expression(line3p1), Expression(line3p2) };
	std::vector<Expression> line4p1 = { Expression(-10), Expression(0) };
	std::vector<Expression> line4p2 = { Expression(10), Expression(0) };
	std::vector<Expression> line4 = { Expression(line4p1), Expression(line4p2) };
	std::vector<Expression> line5p1 = { Expression(-10), Expression(10) };
	std::vector<Expression> line5p2 = { Expression(10), Expression(10) };
	std::vector<Expression> line5 = { Expression(line5p1), Expression(line5p2) };
	std::vector<Expression> line6p1 = { Expression(-10), Expression(-10) };
	std::vector<Expression> line6p2 = { Expression(10), Expression(-10) };
	std::vector<Expression> line6 = { Expression(line6p1), Expression(line6p2) };
	std::vector<Expression> line7p1 = { Expression(-10), Expression(10) };
	std::vector<Expression> line7p2 = { Expression(-10), Expression(-10) };
	std::vector<Expression> line7 = { Expression(line7p1), Expression(line7p2) };
	std::vector<Expression> line8p1 = { Expression(10), Expression(10) };
	std::vector<Expression> line8p2 = { Expression(10), Expression(-10) };
	std::vector<Expression> line8 = { Expression(line8p1), Expression(line8p2) };

	std::vector<Expression> args1 = { Expression(point1), Expression(line1), 
		Expression(point2), Expression(line2), Expression(line3), Expression(line4), 
		Expression(line5), Expression(line6), Expression(line7), Expression(line8), 
		Expression(std::string("-1")), Expression(std::string("1")), 
		Expression(std::string("-1")), Expression(std::string("1")), 
		Expression(std::string("The Title")), Expression(std::string("X Label")), 
		Expression(std::string("Y Label")) };

	Expression result = run(program);
	REQUIRE(result == Expression(args1));
}*/

TEST_CASE("Test for text", "[interpreter]") {
	std::string program = "(\"hello\")";
	INFO(program);
	Expression result = run(program);
	REQUIRE(result == Expression(std::string("\"hello\"")));
}

TEST_CASE("Test set-property", "[interpreter]") {
	std::string program = "(set-property \"number\" \"three\" (3))";
	INFO(program);
	Expression result = run(program);
	REQUIRE(result == Expression(3));
}

TEST_CASE("Test set-property low req", "[interpreter]") {
	std::string program = "(set-property \"number\" (3))";
	INFO(program);
	std::istringstream iss(program);
	Interpreter interp;


	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test set-property no string req", "[interpreter]") {
	std::string program = "(set-property (+ 1 2) \"number\" \"three\")";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test get-property", "[interpreter]") {
	std::string program = "(begin (define a (+ 1 I)) (define b (set-property \"note\" (3) a)) (get-property \"note\" b))";
	INFO(program);
	Expression result = run(program);
	REQUIRE(result == Expression(3));
}

TEST_CASE("Test get-property low req", "[interpreter]") {
	std::string program = "(get-property \"note\")";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test get-property no string req", "[interpreter]") {
	std::string program = "(get-property (3) b)";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test ispoint for false", "[interpreter]") {
	std::map<std::string, Expression> propmap;
	std::string program = "(set-property \"size\" 20 (make-point 0 0))";
	Expression result(10);
	REQUIRE(result.isPoint() == false);
}

TEST_CASE("Test isline for false", "[interpreter]") {
	std::map<std::string, Expression> propmap;
	std::string program = "(set-property \"thickness\" (4) (make-line (make-point 0 0) (make-point 20 20)))";
	Expression result(10);
	REQUIRE(result.isLine() == false);
}

TEST_CASE("Test istext for false", "[interpreter]") {
	std::map<std::string, Expression> propmap;
	std::string program = "(set-property \"position\" (make-point 10 10) (make-text \"hi\"))";
	Expression result(0);
	REQUIRE(result.isText() == false);
}

TEST_CASE("Test lambda map input error second argument not list", "[interpreter]") {
	std::string program = "(map + 3)";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test lambda map input error first argument not procedure", "[interpreter]") {
	std::string program = "(map 3 (list 1 2 3))";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test lambda map input error too many arguments case", "[interpreter]") {
	std::string program = "(begin (define addtwo (lambda (x y) (+ x y))) (map addtwo (list 1 2 3)))";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test handle lookup isHeadComplex", "[interpreter]") {
	std::string program = "(I)";
	INFO(program);
	Expression result = run(program);
	REQUIRE(result == Expression(std::complex<double>(0.0, 1.0)));
}

TEST_CASE("Test Lambda tail length incorrect", "[interpreter]") {
	std::string program = "(lambda (x))";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Lambda tail 0 incorrect", "[interpreter]") {
	std::string program = "(lambda 3 (+ x 2))";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Lambda tail 1 incorrect", "[interpreter]") {
	std::string program = "(lambda (x) 1)";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Lambda tail is proc incorrect", "[interpreter]") {
	std::string program = "(lambda (+) (x y))";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test apply tail incorrect length", "[interpreter]") {
	std::string program = "(apply +)";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test apply tail incorrect argument", "[interpreter]") {
	std::string program = "(apply 3 (list 1 2 3))";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test apply tail incorrect special argument", "[interpreter]") {
	std::string program = "(apply define (list 1 2 3))";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test append tail[1].head a list", "[interpreter]") {
	std::string program = "(append (list 1 2 3) (list 1 2 3))";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test join tail[1].head not a list", "[interpreter]") {
	std::string program = "(join (list 1 2 3) 3)";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test join tail[0].head not a list", "[interpreter]") {
	std::string program = "(join 3 (list 1 2 3))";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test apply not a procedure", "[interpreter]") {
	std::string program = "(apply 3 (list 1 2 3))";
	INFO(program);
	Interpreter interp;

	std::istringstream iss(program);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

