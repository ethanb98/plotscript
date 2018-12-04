#include "expression.hpp"

#include <sstream>
#include <list>

#include <iostream>
#include <algorithm>

#include "environment.hpp"
#include "semantic_error.hpp"

volatile sig_atomic_t global_status_flag = 0;

Expression::Expression(){}

Expression::Expression(const Atom & a) {
  m_head = a;
}

// recursive copy
Expression::Expression(const Expression & a) {

  m_head = a.m_head;
  propmap = a.propmap;
  for(auto e : a.m_tail){
    m_tail.push_back(e);
  }
}

// constructor for list
Expression::Expression(const std::vector<Expression> & a) {
	m_head.setList();
	m_tail = a;
}

// constructor for a lambda kind
Expression::Expression(const Atom & a, const std::vector<Expression> & exp) {
	m_head = a;
	for (auto e : exp) {
		m_tail.push_back(e);
	}
}

// Tests equality to one another
Expression & Expression::operator=(const Expression & a){

  // prevent self-assignment
  if(this != &a){
    m_head = a.m_head;
	propmap = a.propmap;
    m_tail.clear();
    for(auto e : a.m_tail){
      m_tail.push_back(e);
    } 
  }
  
  return *this;
}

Atom & Expression::head(){
  return m_head;
}

const Atom & Expression::head() const{
  return m_head;
}

bool Expression::isHeadNumber() const noexcept{
  return m_head.isNumber();
}

bool Expression::isHeadSymbol() const noexcept{
  return m_head.isSymbol();
}

bool Expression::isHeadComplex() const noexcept{
	return m_head.isComplex();
}

bool Expression::isHeadList() const noexcept {
	return m_head.isList();
}

bool Expression::isHeadLambda() const noexcept {
	return m_head.isLambda();
}

bool Expression::isHeadString() const noexcept {
	return m_head.isString();
}

void Expression::append(const Atom & a){
  m_tail.emplace_back(a);
}

Expression * Expression::tail(){
  Expression * ptr = nullptr;
  
  if(m_tail.size() > 0){
    ptr = &m_tail.back();
  }

  return ptr;
}

Expression::ConstIteratorType Expression::tailConstBegin() const noexcept{
  return m_tail.cbegin();
}

Expression::ConstIteratorType Expression::tailConstEnd() const noexcept{
  return m_tail.cend();
}

Expression apply(const Atom & op, const std::vector<Expression> & args, const Environment & env){
	// if it is a lambda
	if (env.is_exp(op)) {
		// artificial temp environment to delete and remove stuff
		// gets deleted on function end
		Environment newenv = env;
		Expression exp = newenv.get_exp(op);
		Expression newexp = *exp.tailConstBegin();
		Expression endexp = *exp.tail();
		int counter = 0;
		unsigned int count = 0;
		// iterate to find the size of the tail
		for (auto f = newexp.tailConstBegin(); f != newexp.tailConstEnd(); f++) {
			count++;
		}
		// if the args size does not equal the tail size, throw error to avoid core dump
		if (args.size() != count) {
			throw SemanticError("Error in call to procedure: invalid number of arguments.");
		}
		// iterate through the tail and call shadow function to check if redefined definition
		for (auto e = newexp.tailConstBegin(); e != newexp.tailConstEnd(); e++) {
			newenv.shadow((*e).head().asSymbol(), newenv);
			newenv.add_exp(Atom((*e).head()), args[counter]);
			counter++;
		}
		return endexp.eval(newenv);
	}

  // head must be a symbol
  if(!op.isSymbol()){
    throw SemanticError("Error during evaluation: procedure name not a symbol (apply)");
  }
  
  // must map to a proc
  if(!env.is_proc(op)){
    throw SemanticError("Error during evaluation: symbol does not name a procedure (apply)");
  }

  
  // map from symbol to proc
  Procedure proc = env.get_proc(op);
  
  // call proc with args
  return proc(args);
}

Expression Expression::handle_lookup(const Atom & head, const Environment & env){
    if(head.isSymbol()){ // if symbol is in env return value
		if(env.is_exp(head)){
			return env.get_exp(head);
		}
		else{
			throw SemanticError("Error during evaluation: unknown symbol");
		}
    }
    else if(head.isNumber()){
      return Expression(head);
    }
	else if (head.isComplex()) {
	  return Expression(head);
	}
	else if (head.isString()) {
		return Expression(head);
	}
    else{
      throw SemanticError("Error during evaluation: Invalid type in terminal expression");
    }
}

Expression Expression::handle_begin(Environment & env){
  
  if(m_tail.size() == 0){
    throw SemanticError("Error during evaluation: zero arguments to begin");
  }

  // evaluate each arg from tail, return the last
  Expression result;
  for(Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it){
    result = it->eval(env);
  }
  
  return result;
}


Expression Expression::handle_define(Environment & env){

  // tail must have size 3 or error
  if(m_tail.size() != 2){
    throw SemanticError("Error during evaluation: invalid number of arguments to define");
  }
  
  // tail[0] must be symbol
  if(!m_tail[0].isHeadSymbol()){
    throw SemanticError("Error during evaluation: first argument to define not symbol");
  }

  // but tail[0] must not be a special-form or procedure
  std::string s = m_tail[0].head().asSymbol();
  if((s == "define") || (s == "begin")){
    throw SemanticError("Error during evaluation: attempt to redefine a special-form");
  }
  
  if ((s == "e") || (s == "I") || (s == "pi")) {
	  throw SemanticError("Error during evaluation: attempt to define a expression");
  }

  if(env.is_proc(m_head)){
    throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
  }
	
  // eval tail[1]
  Expression result = m_tail[1].eval(env);

  /*if(env.is_exp(m_head)){
    throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
  }*/
      
  //and add to env
  env.add_exp(m_tail[0].head(), result);
  
  return result;
}

Expression Expression::handle_lambda(Environment & env) {
	// lambda tail must be of size 2
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of lambda arguments to define");
	}
	// tail[0] and tail[1] must be symbol
	if (!m_tail[0].isHeadSymbol() || !m_tail[1].isHeadSymbol()) {
		throw SemanticError("Error during evaluation: lambda argument to define not symbol");
	}

	// if the head is a procedure, throw error
	if (env.is_proc(m_head) || env.is_proc(m_tail[0].head().asSymbol())) {
		throw SemanticError("Error during evaluation: attempt to use non-supported lambda procedure");
	}

	// if head is an expression
	if (env.is_exp(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a lambda previously defined symbol");
	}

	std::vector<Expression> vars;
	std::vector<Expression> allArgs;

	// iterate through, make sure to output head and then the rest if the tail
	// push to vector of expressions to create a list
	vars.push_back(m_tail[0].head());
	for (auto e = m_tail[0].tailConstBegin(); e != m_tail[0].tailConstEnd(); e++) {
		vars.push_back(Expression(*e));
	}
	// push vector of expressions to a new vector, include tail[1]
	allArgs.push_back(vars);
	allArgs.push_back(m_tail[1]);
	// make result equal to the expression of allArgs
	Expression result = Expression(allArgs);
	// set lambda to true
	result.head().setLambda();
	// return the result as a list followed by expression
	return result;
}

Expression Expression::handle_apply(Environment & env) {

	// tail must have size 2 or error
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of lambda arguments to define");
	}

	// tail[0] must be symbol
	if (!m_tail[0].isHeadSymbol()) {
		throw SemanticError("Error during evaluation: first argument to define not symbol");
	}
	// evaluate the environment of m_tail[1]
	Expression exp = m_tail[1].eval(env);
	std::vector<Expression> args;

	// tail[0] must not be a special-form or procedure
	std::string s = m_tail[0].head().asSymbol();
	if ((s == "define") || (s == "begin")) {
		throw SemanticError("Error during evaluation: attempt to redefine a special-form");
	}

	// if the tail[1] environment expression is a list, throw error
	if (!exp.isHeadList()) {
		throw SemanticError("Error during evaluation: second argument is not a list");
	}
	
	// if head of tail[0] is a lambda expression, iterate through and output
	if (env.is_exp(m_tail[0].head()))
	{
			// iterate through expression and push into vector of expressions
			for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); e++) {
				args.push_back(*e);
			}
			// return the application of the vector to get the correct mathematical output
			return apply(m_tail[0].head(), args, env);
	}

	// create a counter and iterate through the tail to ensure the size is 0
	int count = 0;
	for (auto it = m_tail[0].tailConstBegin(); it != m_tail[0].tailConstEnd(); it++) {
		count++;
	}

	// if size is not 0, or environment is not a procedure, throw error
	if (!env.is_proc(m_tail[0].head()) || count != 0) {
		throw SemanticError("Error during evaluation: first argument is not a procedure");
	}

	// otherwise, iterate through the tail, and push onto vector of expressions args
	for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); e++) {
		args.push_back(*e);
	}

	// return the application of vector to get the correct mathematical output
	Atom op = m_tail[0].head();
	return apply(op, args, env);
}

Expression Expression::handle_map(Environment & env) {
	// evaluate the environment of tail[1] and set to expression
	Expression exp = m_tail[1].eval(env);
	std::vector<Expression> args;
	std::vector<Expression> result;

	// tail must have size 2 or error
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of lambda arguments to define");
	}

	// tail[0] must be symbol
	if (!m_tail[0].isHeadSymbol()) {
		throw SemanticError("Error during evaluation: first argument to define not symbol");
	}

	// but tail[0] must not be a special-form or procedure
	std::string s = m_tail[0].head().asSymbol();
	if ((s == "define") || (s == "begin")) {
		throw SemanticError("Error during evaluation: attempt to redefine a special-form");
	}

	// if the expression is not a list, throw error
	if (!exp.isHeadList()) {
		throw SemanticError("Error during evaluation: second argument to map not a list");
	}

	// if the environment has an expression of lambda, iterate
	if (env.is_exp(m_tail[0].head()))
	{
		// iterate through and push one by one into a temporary vector of expressions args. Take
		// the application of the expression in args and push that onto the vector of expressions
		// called result. Clear args and repeat to do the math to each number in the list unarilly
		for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); e++) {
			args.emplace_back(*e);
			result.push_back(apply(m_tail[0].head(), args, env));
			args.clear();
		}
		return Expression(result);
	}

	// count tail size to ensure it is 0
	int count = 0;
	for (auto it = m_tail[0].tailConstBegin(); it != m_tail[0].tailConstEnd(); it++) {
		count++;
	}

	// if count is not 0, or not a procedure, throw error
	if (!env.is_proc(m_tail[0].head()) || count != 0) {
		throw SemanticError("Error during evaluation: first argument to map not a procedure");
	}

	// otherwise, iterate just like the lambda function, but for a function that is not a lambda
	for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); e++) {
		args.emplace_back(*e);
		result.push_back(apply(m_tail[0].head(), args, env));
		args.clear();
	}

	// return the expression of result
	return Expression(result);
}

// Sets the property as the value and key
Expression Expression::handle_set(Environment & env) {
	// lambda tail must be of size 2
	if (m_tail.size() != 3) {
		throw SemanticError("Error during evaluation: invalid number of set-property arguments to define");
	}
	// tail[0] must be a string
	if (!m_tail[0].isHeadString()) {
		throw SemanticError("Error during evaluation: first argument to set-property not a string");
	}
	
	Expression exp = m_tail[2].eval(env);
	std::string key = m_tail[0].head().asString();
	Expression eval = m_tail[1].eval(env);

	exp.propmap[key] = eval;

	return exp;
}

// returns the definition of the property
Expression Expression::handle_get(Environment & env) {
	// lambda tail must be of size 2
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of get-property arguments to define");
	}
	// tail[0] must be a string
	if (!m_tail[0].isHeadString()) {
		throw SemanticError("Error during evaluation: first argument to get-property not a string");
	}

	Expression exp = m_tail[1].eval(env);
	std::string key = m_tail[0].head().asString();

	return exp.propmap[key];
}

// returns discrete plot information as required
Expression Expression::handle_discrete(Environment & env) {
	// function tail must be of size 2
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of discrete-plot arguments to define");
	}
	Expression test1 = m_tail[0].eval(env);
	Expression test2 = m_tail[1].eval(env);
	// tail[0] must be a list
	if (!test1.isHeadList()) {
		throw SemanticError("Error during evaluation: first argument to discrete-plot not a list");
	}
	// tail[1] must be a list
	if (!test2.isHeadList()) {
		throw SemanticError("Error during evaluation: second argument to discrete-plot not a list");
	}

	// Create the variables to be used later
	const double N = 20;
	const double orig = 0;
	Expression exp = m_tail[0].eval(env);
	Expression last = m_tail[1].eval(env);
	double xMax = 0;
	double xMin = 5000;
	double yMax = 0;
	double yMin = 5000;

	// Iterate through the lists to find the maxes and mins
	for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e) {
		xMax = std::max((*e).m_tail[0].head().asNumber(), xMax);
		xMin = std::min((*e).m_tail[0].head().asNumber(), xMin);
		yMax = std::max((*e).m_tail[1].head().asNumber(), yMax);
		yMin = std::min((*e).m_tail[1].head().asNumber(), yMin);
	}

	// Find the x and y scale values
	double xScale = N / (xMax - xMin);
	double yScale = N / (yMax - yMin);

	std::string tempXMax = Atom(xMax).asString();
	std::string tempXMin = Atom(xMin).asString();
	std::string tempYMax = Atom(yMax).asString();
	std::string tempYMin = Atom(yMin).asString();

	// remake the maxes and mins to the x and y scale values
	double xMaxTemp = xMax * xScale;
	double xMinTemp = xMin * xScale;
	double yMaxTemp = yMax * yScale;
	double yMinTemp = yMin * yScale;
	double negYMinTemp = yMinTemp * -1;
	double negYMaxTemp = yMaxTemp * -1;

	// Create the expressions for the x and y positions
	Expression xMaxPos = Expression(Atom(xMaxTemp));
	Expression xMinPos = Expression(Atom(xMinTemp));
	Expression yMaxPos = Expression(Atom(yMaxTemp));
	Expression yMinPos = Expression(Atom(yMinTemp));
	Expression origPos = Expression(Atom(orig));
	Expression negYMaxPos = Expression(Atom(negYMaxTemp));
	Expression negYMinPos = Expression(Atom(negYMinTemp));

	// Create the atoms for points and lines
	Atom pointatom = Atom("make-point");
	Atom lineatom = Atom("make-line");

	double xpos = 0;
	double ypos = 0;
	//double yaxis = 0;
	double xax = (xMaxTemp + xMinTemp) / 2;
	//double yax = (yMaxTemp + yMinTemp) / 2;
	std::vector<Expression> tempPoint;
	std::vector<Expression> tempLine1;
	std::vector<Expression> tempLine2;
	std::vector<Expression> Results;

	for(auto & pointpos : exp.m_tail) {
		xpos = pointpos.m_tail[0].head().asNumber() * xScale;
		ypos = pointpos.m_tail[1].head().asNumber() * yScale;
		ypos *= -1;
		// push back for point
		tempPoint.push_back(Expression(Atom(xpos)));
		tempPoint.push_back(Expression(Atom(ypos)));
		Results.push_back(Expression(pointatom, tempPoint).eval(env));
		tempPoint.clear();

		ypos *= -1;
		// push back for line from 0 to point
		tempLine1.push_back(Expression(Atom(xpos)));
		if (0 > negYMinTemp) {
			//negYMinTemp *= -1;
			tempLine1.push_back(Expression(Atom(negYMinTemp * -1)));
			//negYMinTemp *= -1;
		}
		else {
			tempLine1.push_back(Expression(Atom(xax)));
		}
		tempPoint.push_back(Expression(pointatom, tempLine1));

		tempLine2.push_back(Expression(Atom(xpos)));
		tempLine2.push_back(Expression(Atom(ypos)));
		tempPoint.push_back(Expression(pointatom, tempLine2));

		Results.push_back(Expression(lineatom, tempPoint).eval(env));

		tempPoint.clear();
		tempLine1.clear();
		tempLine2.clear();
	}

	// Line 3 created and evaluated
	// Y-Axis
	// (make-line (make-point 0 10) (make-point 0 -10))
	if (orig > xMinTemp && orig < xMaxTemp) {
		std::vector<Expression> line3;
		std::vector<Expression> line3p1;
		std::vector<Expression> line3p2;
		// create point (make-point 0 10)
		line3p1.push_back(origPos); // Push back the point 1 values
		line3p1.push_back(yMaxPos);
		// create point (make-point 0 -10)
		line3p2.push_back(origPos); // Push back the point 2 values
		line3p2.push_back(yMinPos);
		// push back the points
		line3.push_back(Expression(pointatom, line3p1)); // Push back the line of points
		line3.push_back(Expression(pointatom, line3p2));
		// eval the y axis line
		Expression line3pos = Expression(lineatom, line3).eval(env); // Eval the line
		Results.push_back(line3pos);
	}

	// Line 4 created and evaluated
	// X-Axis
	// (make-line (make-point -10 0) (make-point 10 0))
	if (orig > yMinTemp && orig < yMaxTemp) {
		std::vector<Expression> line4;
		std::vector<Expression> line4p1;
		std::vector<Expression> line4p2;
		// Create point (make-point -10 0)
		line4p1.push_back(xMinPos); // Push back the point 1 values
		line4p1.push_back(origPos);
		// create point (make-point 10 0)
		line4p2.push_back(xMaxPos); // Push back the point 2 values
		line4p2.push_back(origPos);
		// push back the points
		line4.push_back(Expression(pointatom, line4p1)); // Push back the line of points
		line4.push_back(Expression(pointatom, line4p2));
		// eval the x-axis line
		Expression line4pos = Expression(lineatom, line4).eval(env); // Eval the line
		Results.push_back(line4pos);
	}

	// Line 5 created and evaluated
	// (make-line (make-point -10 10) (make-point 10 10))
	std::vector<Expression> line5;
	std::vector<Expression> line5p1;
	std::vector<Expression> line5p2;
	line5p1.push_back(xMinPos); // Push back the point 1 values
	line5p1.push_back(yMaxPos);
	line5p2.push_back(xMaxPos); // Push back the point 2 values
	line5p2.push_back(yMaxPos);
	line5.push_back(Expression(pointatom, line5p1)); // Push back the line of points
	line5.push_back(Expression(pointatom, line5p2));
	Expression line5pos = Expression(lineatom, line5).eval(env); // Eval the line

	// Line 6 created and evaluated
	// (make-line (make-point -10 -10) (make-point 10 -10))
	std::vector<Expression> line6;
	std::vector<Expression> line6p1;
	std::vector<Expression> line6p2;
	line6p1.push_back(xMinPos); // Push back the point 1 values
	line6p1.push_back(yMinPos);
	line6p2.push_back(xMaxPos); // Push back the point 2 values
	line6p2.push_back(yMinPos);
	line6.push_back(Expression(pointatom, line6p1)); // Push back the line of points
	line6.push_back(Expression(pointatom, line6p2));
	Expression line6pos = Expression(lineatom, line6).eval(env); // Eval the line

	// Line 7 created and evaluated
	// (make-line (make-point -10 10) (make-point -10 -10))
	std::vector<Expression> line7;
	std::vector<Expression> line7p1;
	std::vector<Expression> line7p2;
	line7p1.push_back(xMinPos); // Push back the point 1 values
	line7p1.push_back(yMaxPos);
	line7p2.push_back(xMinPos); // Push back the point 2 values
	line7p2.push_back(yMinPos);
	line7.push_back(Expression(pointatom, line7p1)); // Push back the line of points
	line7.push_back(Expression(pointatom, line7p2));
	Expression line7pos = Expression(lineatom, line7).eval(env); // Eval the line

	// Line 8 created and evaluated
	// (make-line (make-point 10 10) (make-point 10 -10))
	std::vector<Expression> line8;
	std::vector<Expression> line8p1;
	std::vector<Expression> line8p2;
	line8p1.push_back(xMaxPos); // Push back the point 1 values
	line8p1.push_back(yMaxPos);
	line8p2.push_back(xMaxPos); // Push back the point 2 values
	line8p2.push_back(yMinPos);
	line8.push_back(Expression(pointatom, line8p1)); // Push back the line of points
	line8.push_back(Expression(pointatom, line8p2));
	Expression line8pos = Expression(lineatom, line8).eval(env); // Eval the line

	 // Push all the necessary expressions onto the result
	// vector of expressions
	Results.push_back(line5pos);
	Results.push_back(line6pos);
	Results.push_back(line7pos);
	Results.push_back(line8pos);

	// Iterate through the lists to add max and min x's and y's to the vector
	tempXMin = "\"" + tempXMin + "\"";
	tempXMax = "\"" + tempXMax + "\"";
	tempYMin = "\"" + tempYMin + "\"";
	tempYMax = "\"" + tempYMax + "\"";
	Results.push_back(Expression(tempXMin));
	Results.push_back(Expression(tempXMax));
	Results.push_back(Expression(tempYMin));
	Results.push_back(Expression(tempYMax));

	// Iterate through m_tail[1] and add all variables to the results array
	for (auto h = last.tailConstBegin(); h != last.tailConstEnd(); ++h) {
		Results.push_back(Expression((*h).m_tail[1].head().asString()));
	}

	// Returns the vector of expressions as an expression
	/*Expression FinalResults = Expression(Results);
	FinalResults.head().setDiscrete();
	return FinalResults;*/
	return Expression(Results);
}


// this is a simple recursive version. the iterative version is more
// difficult with the last data structure used (no parent pointer).
// this limits the practical depth of our AST
Expression Expression::eval(Environment & env){
	if (global_status_flag > 0) {
		return Expression(Atom("Error: interpreter kernel interrupted"));
	}
	if(m_tail.empty()){
		if (m_head.isSymbol() && (m_head.asSymbol() == "list")) {
			return Expression(m_tail);
		}
		return handle_lookup(m_head, env);
	}
	// handle begin special-form
	else if(m_head.isSymbol() && m_head.asSymbol() == "begin"){
		return handle_begin(env);
	}
	// handle define special-form
	else if(m_head.isSymbol() && m_head.asSymbol() == "define"){
		return handle_define(env);
	}
	// handle lambda special-form
	else if (m_head.isSymbol() && m_head.asSymbol() == "lambda") {
		return handle_lambda(env);
	}
	// handle appl special-form
	else if (m_head.isSymbol() && m_head.asSymbol() == "apply") {
		return handle_apply(env);
	}
	else if (m_head.isSymbol() && m_head.asSymbol() == "map") {
		return handle_map(env);
	}
	else if (m_head.isSymbol() && m_head.asSymbol() == "set-property") {
		return handle_set(env);
	}
	else if (m_head.isSymbol() && m_head.asSymbol() == "get-property") {
		return handle_get(env);
	}
	else if (m_head.isSymbol() && m_head.asSymbol() == "discrete-plot") {
		return handle_discrete(env);
	}
	// else attempt to treat as procedure
	else{ 
		std::vector<Expression> results;
		for(Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it){
		results.push_back(it->eval(env));
		}
		return apply(m_head, results, env);
	}
}

std::ostream & operator<<(std::ostream & out, const Expression & exp){
	Environment env;
	if (!exp.isHeadList() && exp.head().isNone()) {
		out << "NONE";
	}
	else {
		if (!exp.isHeadComplex()) {
			out << "(";
		}

		// If the expression head is a procedure and is not lambda, add a space to output
		if (env.is_proc(exp.head()) && exp.isHeadSymbol() && (exp.head().asSymbol() != "lambda")) {
			out << exp.head();
			out << " ";
		}
		else {
			out << exp.head();
		}

		for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e) {
			out << *e;
			// add correct spacing for lists and lambda
			if ((e + 1) != exp.tailConstEnd()) {
				out << " ";
			}
		}

		if (!exp.isHeadComplex()) {
			out << ")";
		}
	}
  return out;
}

std::string Expression::transferString() const noexcept{
	Environment env;
	std::string text;

	if (!this->isHeadList() && this->head().isNone()) {
		text += "NONE";
	}
	else {
		if (!this->isHeadComplex() && !this->isHeadList()) {
			text += "(";
		}

			text += this->head().asString();

		for (auto e = this->tailConstBegin(); e != this->tailConstEnd(); ++e) {
			// add correct spacing for lists and lambda
			text += (*e).transferString();
		}

		if (!this->isHeadComplex() && !this->isHeadList()) {
			text += ")";
		}
	}
	return text;
}

bool Expression::isPoint() const noexcept {
	Expression exp(Atom("\"point\""));
	for (auto &e : propmap) {
		if (e.first == "\"object-name\"") {
			if (e.second == exp) {
				return true;
			}
		}
	}
	return false;
}

bool Expression::isLine() const noexcept{
	Expression exp(Atom("\"line\""));
	if ((propmap.find("\"object-name\"") != propmap.end()) && propmap.at("\"object-name\"") == exp) {
		return true;
	}
	return false;
}

bool Expression::isText() const noexcept{
	Expression exp(Atom("\"text\""));
	if ((propmap.find("\"object-name\"") != propmap.end()) && propmap.at("\"object-name\"") == exp) {
		return true;
	}
	return false;
}

double Expression::pointTail0() const noexcept{
	return m_tail[0].head().asNumber();
}

double Expression::pointTail1() const noexcept{
	return m_tail[1].head().asNumber();
}

Expression Expression::req() const noexcept {
	Expression point(Atom("\"point\""));
	Expression line(Atom("\"line\""));
	Expression text(Atom("\"text\""));
	Expression exp;
	if ((propmap.find("\"object-name\"") != propmap.end()) && propmap.at("\"object-name\"") == point) {
		return propmap.at("\"size\"");
	}
	if ((propmap.find("\"object-name\"") != propmap.end()) && propmap.at("\"object-name\"") == line) {
		return propmap.at("\"thickness\"");
	}
	if ((propmap.find("\"object-name\"") != propmap.end()) && propmap.at("\"object-name\"") == text) {
		return propmap.at("\"text-scale\"");
		/*if (propmap.at("\"text-scale\"").head().asNumber() > 0) {
			return propmap.at("\"text-scale\"").head().asNumber();
		}*/
		/*else {
			throw SemanticError("Error during notebook: scale is not positive non-zero number");
		}*/
	}
	return exp;
}

Expression Expression::textReq() const noexcept{
		return propmap.at("\"position\"");
}

double Expression::textRotReq() const noexcept {
	// need to convert from radian to degree
	return propmap.at("\"text-rotation\"").head().asNumber();
}

double Expression::lineTail0x() const noexcept {
	return m_tail[0].m_tail[0].head().asNumber();
}

double Expression::lineTail0y() const noexcept {
	return m_tail[0].m_tail[1].head().asNumber();
}

double Expression::lineTail1x() const noexcept {
	return m_tail[1].m_tail[0].head().asNumber();
}

double Expression::lineTail1y() const noexcept {
	return m_tail[1].m_tail[1].head().asNumber();
}

bool Expression::operator==(const Expression & exp) const noexcept{

  bool result = (m_head == exp.m_head);

  result = result && (m_tail.size() == exp.m_tail.size());

  if(result){
    for(auto lefte = m_tail.begin(), righte = exp.m_tail.begin();
	(lefte != m_tail.end()) && (righte != exp.m_tail.end());
	++lefte, ++righte){
      result = result && (*lefte == *righte);
    }
  }

  return result;
}

bool operator!=(const Expression & left, const Expression & right) noexcept{

  return !(left == right);
}


std::vector<Expression> Expression::getTail() const noexcept {
	std::vector<Expression> tailpos;
	for (auto f = this->tailConstBegin(); f != this->tailConstEnd(); f++) {
		tailpos.push_back(*f);
	}
	return tailpos;
}