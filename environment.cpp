#include "environment.hpp"

#include <cassert>
#include <cmath>

#include "environment.hpp"
#include "semantic_error.hpp"

/*********************************************************************** 
Helper Functions
**********************************************************************/

// predicate, the number of args is nargs
bool nargs_equal(const std::vector<Expression> & args, unsigned nargs){
  return args.size() == nargs;
}

/*********************************************************************** 
Each of the functions below have the signature that corresponds to the
typedef'd Procedure function pointer.
**********************************************************************/

// the default procedure always returns an expresison of type None
Expression default_proc(const std::vector<Expression> & args){
  args.size(); // make compiler happy we used this parameter
  return Expression();
};

Expression add(const std::vector<Expression> & args){

  // check all aruments are numbers, while adding
  std::complex<double> result = 0;
  bool complexArgs = false;
  for( auto & a :args){
    if(a.isHeadNumber()){
      result += a.head().asNumber();      
    }
	else if (a.isHeadComplex()) {
		result += a.head().asComplex();
		complexArgs = true;
	}
    else{
      throw SemanticError("Error in call to add, argument not a number");
    }
  }

  if (complexArgs == false) {
	  return Expression(result.real());
  }
  else {
	  return Expression(result);
  }
};

Expression mul(const std::vector<Expression> & args){
 
  // check all aruments are numbers, while multiplying
  std::complex<double> result = 1.0;
  bool complexArgs = false;
  for( auto & a :args){
    if(a.isHeadNumber()){
      result *= a.head().asNumber();      
    }
	else if (a.isHeadComplex()) {
		result *= a.head().asComplex();
		complexArgs = true;
	}
    else{
      throw SemanticError("Error in call to mul, argument not a number");
    }
  }

  if (complexArgs == false) {
	  return Expression(result.real());
  }
  else {
	  return Expression(result);
  }
};

Expression subneg(const std::vector<Expression> & args){

  std::complex<double> result = 0;
  bool complexArgs = false;
  // preconditions
  if(nargs_equal(args,1)){
    if(args[0].isHeadNumber()){
      result = -args[0].head().asNumber();
    }
	else if (args[0].isHeadComplex()) {
		result = -args[0].head().asComplex();
		complexArgs = true;
	}
    else{
      throw SemanticError("Error in call to negate: invalid argument.");
    }
  }
  else if(nargs_equal(args,2)){
    if( (args[0].isHeadNumber()) && (args[1].isHeadNumber()) ){
      result = args[0].head().asNumber() - args[1].head().asNumber();
    }
	else if ((args[0].isHeadNumber()) && (args[1].isHeadComplex())) {
		result = args[0].head().asNumber() - args[1].head().asComplex();
		complexArgs = true;
	}
	else if ((args[0].isHeadComplex()) && (args[1].isHeadNumber())) {
		result = args[0].head().asComplex() - args[1].head().asNumber();
		complexArgs = true;
	}
	else if ((args[0].isHeadComplex()) && (args[1].isHeadComplex())) {
		result = args[0].head().asComplex() - args[1].head().asComplex();
		complexArgs = true;
	}
    else{      
      throw SemanticError("Error in call to subtraction: invalid argument.");
    }
  }
  else{
    throw SemanticError("Error in call to subtraction or negation: invalid number of arguments.");
  }

  if (complexArgs == false) {
	  return Expression(result.real());
  }
  else {
	  return Expression(result);
  }
};

Expression div(const std::vector<Expression> & args){

  std::complex<double> result = 0; 
  bool complexArgs = false;
  if (nargs_equal(args, 1)) {
	  if (args[0].isHeadNumber()) {
		  result = 1 / args[0].head().asNumber();
	  }
	  else if (args[0].isHeadComplex()) {
		  std::complex<double> divide(1.0, 0.0);
		  result = divide / args[0].head().asComplex();
		  complexArgs = true;
	  }
	  else {
		  throw SemanticError("Error: single argument is not a number or complex.");
	  }
  }
  else if(nargs_equal(args, 2)) {
	  if (args[0].isHeadNumber() && args[1].isHeadNumber()) {
		  result = args[0].head().asNumber() / args[1].head().asNumber();
		  //result = args[0].head().asNumber() * args[0].head().asNumber();
	  }
	  else if (args[0].isHeadNumber() && args[1].isHeadComplex()) {
		  result = args[0].head().asNumber() / args[1].head().asComplex();
		  complexArgs = true;
		  //result = args[0].head().asComplex() * args[0].head().asComplex();
	  }
	  else if (args[0].isHeadComplex() && args[1].isHeadNumber()) {
		  result = args[0].head().asComplex() / args[1].head().asNumber();
		  complexArgs = true;
	  }
	  else if (args[0].isHeadComplex() && args[1].isHeadComplex()) {
		  result = args[0].head().asComplex() / args[1].head().asComplex();
		  complexArgs = true;
	  }
	  else {
		  throw SemanticError("Error in call to division: an argument is not complex or number");
	  }
	  // Possible later use depending on requirements

	  /*for (auto & a : args) {
		  if (a.isHeadNumber()) {
			  result /= a.head().asNumber();
		  }
		  else if (a.isHeadComplex()) {
			  result /= a.head().asComplex();
			  complexArgs = true;
		  }
		  else {
			  throw SemanticError("Error in call to division: invalid argument.");
		  }
	  }*/
  }
  else {
	  throw SemanticError("Error in call to division: invalid number of arguments");
  }
  if (complexArgs == false) {
	  return Expression(result.real());
  }
  else {
	  return Expression(result);
  }
};

Expression real(const std::vector<Expression> & args) {
	std::complex<double> result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = args[0].head().asComplex();
			return Expression(result.real());
		}
		else {
			throw SemanticError("Error in call to complex real: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to complex real: invalid number of arguments");
	}
}

Expression imag(const std::vector<Expression> & args) {
	std::complex<double> result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = args[0].head().asComplex();
			return Expression(result.imag());
		}
		else {
			throw SemanticError("Error in call to complex imaginary: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to complex imaginary: invalid number of arguments.");
	}
}

Expression abs(const std::vector<Expression> & args) {
	std::complex<double> result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = std::abs(args[0].head().asComplex());
			return Expression(result.real());
		}
		else {
			throw SemanticError("Error in call to complex absolute: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to complex absolute: invalid number of arguments.");
	}
}

Expression arg(const std::vector<Expression> & args) {
	std::complex<double> result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = std::arg(args[0].head().asComplex());
			return Expression(result.real());
		}
		else {
			throw SemanticError("Error in call to complex angle/phase: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to complex angle/phase: invalid number of arguments.");
	}
}

Expression conj(const std::vector<Expression> & args) {
	std::complex<double> result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = std::conj(args[0].head().asComplex());
			return Expression(result);
		}
		else {
			throw SemanticError("Error in call to complex conjugate: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to complex conjugate: invalid number of arguments.");
	}
}

Expression sqrt(const std::vector<Expression> & args){

  std::complex<double> result = 0;
  bool complexArgs = false;

  if(nargs_equal(args,1)){
    if( (args[0].isHeadNumber()) && (args[0].head().asNumber() >= 0) ){
      result = std::sqrt(args[0].head().asNumber());
    }
	else if ((args[0].isHeadNumber()) && (args[0].head().asNumber() < 0)) {
		std::complex<double> makeComplex(args[0].head().asNumber(), 0.0);
		result = std::sqrt(makeComplex);
		complexArgs = true;
	}
	else if ((args[0].isHeadComplex())) {
		result = std::sqrt(args[0].head().asComplex());
		complexArgs = true;
	}
    else{      
      throw SemanticError("Error in call to square root: invalid argument (negative number).");
    }
  }
  else{
    throw SemanticError("Error in call to square root: invalid number of arguments.");
  }

  if (complexArgs == false) {
	  return Expression(result.real());
  }
  else {
	  return Expression(result);
  }
};

Expression pow(const std::vector<Expression> & args){

  std::complex<double> result = 0;  
  bool complexArgs = false;

  if(nargs_equal(args,2)){
    if( (args[0].isHeadNumber()) && (args[1].isHeadNumber()) ){
      result = std::pow(args[0].head().asNumber(), args[1].head().asNumber());
    }
	else if ((args[0].isHeadNumber()) && (args[1].isHeadComplex())) {
		result = std::pow(args[0].head().asNumber(), args[1].head().asComplex());
		complexArgs = true;
	}
	else if ((args[0].isHeadComplex()) && (args[1].isHeadNumber())) {
		result = std::pow(args[0].head().asComplex(), args[1].head().asNumber());
		complexArgs = true;
	}
	else if ((args[0].isHeadComplex()) && (args[1].isHeadComplex())) {
		result = std::pow(args[0].head().asComplex(), args[1].head().asComplex());
		complexArgs = true;
	}
    else{      
      throw SemanticError("Error in call to power: invalid argument.");
    }
  }
  else{
    throw SemanticError("Error in call to power: invalid number of arguments.");
  }
  
  if (complexArgs == false) {
	  return Expression(result.real());
  }
  else {
	  return Expression(result);
  }
};

Expression ln(const std::vector<Expression> & args){

  double result = 0;  

  if(nargs_equal(args,1)){
    if( (args[0].isHeadNumber()) && (args[0].head().asNumber() > 0) ){
		result = std::log(args[0].head().asNumber());
		return Expression(result);
    }
    else{      
      throw SemanticError("Error in call to natural log: invalid argument (0 or negative number).");
    }
  }
  else{
    throw SemanticError("Error in call to natural log: invalid number of arguments.");
  }
};

Expression sin(const std::vector<Expression> & args) {

	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			result = std::sin(args[0].head().asNumber());
			return Expression(result);
		}
		else {
			throw SemanticError("Error in call to sine: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to sine: invalid number of arguments.");
	}
};

Expression cos(const std::vector<Expression> & args) {

	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			result = std::cos(args[0].head().asNumber());
			return Expression(result);

		}
		else {
			throw SemanticError("Error in call to cosine: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to cosine: invalid number of arguments.");
	}
};

Expression tan(const std::vector<Expression> & args) {

	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			result = std::tan(args[0].head().asNumber());
			return Expression(result);

		}
		else {
			throw SemanticError("Error in call to tangent: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to tangent: invalid number of arguments.");
	}
};

Expression list(const std::vector<Expression> & args) {	
	std::vector<Expression> result;
	result = args;
	return Expression(result);
}

Expression first(const std::vector<Expression> & args) {
	//std::vector<Expression> result = args;
	if (nargs_equal(args, 1)) {
		if (!args[0].isHeadList()) {
			throw SemanticError("Error: argument to first is not a list.");
		}
		else {
			if (args[0].tailConstBegin() != args[0].tailConstEnd()) {
				return Expression(*args[0].tailConstBegin());
			}
			else {
				throw SemanticError("Error: argument to first is an empty list");
			}
		}
	}
	else {
		throw SemanticError("Error in call to first: Invalid number of arguments.");
	}
}

Expression rest(const std::vector<Expression> & args) {
	std::vector<Expression> result;
	if (nargs_equal(args, 1)) {
		if (!args[0].isHeadList()) {
			throw SemanticError("Error: argument to rest is not a list.");
		}
		else {
			if (args[0].tailConstBegin() != args[0].tailConstEnd()) {
				auto e = args[0].tailConstBegin();
				e++;
				while (e != args[0].tailConstEnd()) {
					result.push_back(Expression(*e));
					e++;
				}
				return Expression(result);
			}
			else {
				throw SemanticError("Error: argument to rest is an empty list");
			}
		}
	}
	else {
		throw SemanticError("Error in call to rest: Invalid number of arguments.");
	}
}

Expression length(const std::vector<Expression> & args) {
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadList()) {
			int counter = 0;
			for (auto e = args[0].tailConstBegin(); e != args[0].tailConstEnd(); e++)
			{
				counter++;
			}
			return Expression(counter);
		}
		else {
			throw SemanticError("Error: argument to length is not a list.");
		}
	}
	else {
		throw SemanticError("Error in call to length: Invalid number of arguments.");
	}
}

Expression append(const std::vector<Expression> & args) {
	std::vector<Expression> result;
	if (nargs_equal(args, 2)) {
		if (args[0].isHeadList()) {
			if (!args[1].isHeadList()) {
				for (auto e = args[0].tailConstBegin(); e != args[0].tailConstEnd(); e++) {
					result.push_back(Expression(*e));
				}
				result.push_back(args[1]);
				return Expression(result);
			}
			else {
				throw SemanticError("Error: argument to append is a list");
			}
		}
		else {
			throw SemanticError("Error: argument to append is not a list.");
		}
	}
	else {
		throw SemanticError("Error in call to append: Invalid number of arguments.");
	}
}

Expression join(const std::vector<Expression> & args) {
	std::vector<Expression> result;
	if (nargs_equal(args, 2)) {
		if (args[0].isHeadList()) {
			if (args[1].isHeadList()) {
				for (auto e = args[0].tailConstBegin(); e != args[0].tailConstEnd(); e++) {
					result.push_back(Expression(*e));
				}
				for (auto f = args[1].tailConstBegin(); f != args[1].tailConstEnd(); f++) {
					result.push_back(Expression(*f));
				}
				return Expression(result);
			}
			else {
				throw SemanticError("Error: argument to join is not a list.");
			}
		}
		else {
			throw SemanticError("Error: argument to join is not a list.");
		}
	}
	else {
		throw SemanticError("Error in call to join: Invalid number of arguments.");
	}
}

Expression range(const std::vector<Expression> & args) {
	std::vector<Expression> result;
	if (nargs_equal(args, 3)) {
		if (args[0].isHeadNumber() && args[1].isHeadNumber() && args[2].isHeadNumber()) {
			double e = args[0].head().asNumber();
			double f = args[1].head().asNumber();
			double g = args[2].head().asNumber();
			if( g > 0 ) {
				if (e < f) {
					while (e <= f) {
						result.push_back(Expression(e));
						e += g;
					}
					return Expression(result);
				}
				else {
					throw SemanticError("Error: begin greater than end in range.");
				}
			}
			else {
				throw SemanticError("Error: negative or zero increment in range.");
			}			
		}	
		else {
			throw SemanticError("Error: argmument to range is not a number.");
		}
	}
	else {
		throw SemanticError("Error in call to range: Invalid number of arguments");
	}
}

const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);
const std::complex<double> I (0.0,1.0);
const std::complex<double> negI (0.0, -1.0);

Environment::Environment(){

  reset();
}

Environment::Environment(const Environment & a) {
	envmap = a.envmap;
}

Environment & Environment::operator=(const Environment & a) {
	if (this != &a) {
		envmap = a.envmap;
	}
	return *this;
}

void Environment::shadow(const std::string & args, Environment & newenv) {
	if (newenv.envmap.find(args) != newenv.envmap.end() ) {
		newenv.envmap.erase(args);
	}
}

bool Environment::is_known(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  return envmap.find(sym.asSymbol()) != envmap.end();
}

bool Environment::is_exp(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ExpressionType);
}

Expression Environment::get_exp(const Atom & sym) const{

  Expression exp;
  
  if(sym.isSymbol()){
    auto result = envmap.find(sym.asSymbol());
    if((result != envmap.end()) && (result->second.type == ExpressionType)){
      exp = result->second.exp;
    }
  }

  return exp;
}

void Environment::add_exp(const Atom & sym, const Expression & exp){
	if(!sym.isSymbol()){
		throw SemanticError("Attempt to add non-symbol to environment (add_exp error)");
	}
	// error if overwriting symbol map
	if(envmap.find(sym.asSymbol()) != envmap.end()){
		throw SemanticError("Attempt to overwrite symbol in environment (add_exp error)");
	}
	envmap.emplace(sym.asSymbol(), EnvResult(ExpressionType, exp)); 
}

bool Environment::is_proc(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ProcedureType);
}

Procedure Environment::get_proc(const Atom & sym) const{

  //Procedure proc = default_proc;

  if(sym.isSymbol()){
    auto result = envmap.find(sym.asSymbol());
    if((result != envmap.end()) && (result->second.type == ProcedureType)){
      return result->second.proc;
    }
  }

  return default_proc;
}

/*
Reset the environment to the default state. First remove all entries and
then re-add the default ones.
 */
void Environment::reset(){

  envmap.clear();
  
  // Built-In value of pi
  envmap.emplace("pi", EnvResult(ExpressionType, Expression(PI)));

  // Procedure: add;
  envmap.emplace("+", EnvResult(ProcedureType, add)); 

  // Procedure: subneg;
  envmap.emplace("-", EnvResult(ProcedureType, subneg)); 

  // Procedure: mul;
  envmap.emplace("*", EnvResult(ProcedureType, mul)); 

  // Procedure: div;
  envmap.emplace("/", EnvResult(ProcedureType, div)); 

  // Built-In value of Euler's number
  envmap.emplace("e", EnvResult(ExpressionType, Expression(EXP)));

  // Procedure: sqrt;
  envmap.emplace("sqrt", EnvResult(ProcedureType, sqrt));

  // Procedure: pow;
  envmap.emplace("^", EnvResult(ProcedureType, pow));

  // Procedure: ln;
  envmap.emplace("ln", EnvResult(ProcedureType, ln));

  // Procedure: sin;
  envmap.emplace("sin", EnvResult(ProcedureType, sin));

  // Procedure: cos;
  envmap.emplace("cos", EnvResult(ProcedureType, cos));

  // Procedure: tan;
  envmap.emplace("tan", EnvResult(ProcedureType, tan));

  // Built-In value of Imaginary I
  envmap.emplace("I", EnvResult(ExpressionType, Expression(I)));

  // Built-In value of negative Imaginary -I
  envmap.emplace("-I", EnvResult(ExpressionType, Expression(negI)));

  // Procedure: real;
  envmap.emplace("real", EnvResult(ProcedureType, real));

  // Procedure: imag;
  envmap.emplace("imag", EnvResult(ProcedureType, imag));

  // Procedure: mag;
  envmap.emplace("mag", EnvResult(ProcedureType, abs));

  // Procedure: arg;
  envmap.emplace("arg", EnvResult(ProcedureType, arg));

  // Procedure: conj;
  envmap.emplace("conj", EnvResult(ProcedureType, conj));

  // Procedure: List;
  envmap.emplace("list", EnvResult(ProcedureType, list));

  // Procedure: first;
  envmap.emplace("first", EnvResult(ProcedureType, first));

  // Procedure: rest;
  envmap.emplace("rest", EnvResult(ProcedureType, rest));

  // Procedure: length;
  envmap.emplace("length", EnvResult(ProcedureType, length));

  // Procedure: append;
  envmap.emplace("append", EnvResult(ProcedureType, append));

  // Procedure: join;
  envmap.emplace("join", EnvResult(ProcedureType, join));

  // Procedure: range;
  envmap.emplace("range", EnvResult(ProcedureType, range));
}
