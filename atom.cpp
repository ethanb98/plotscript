#include "atom.hpp"

#include <sstream>
#include <cctype>
#include <cmath>
#include <limits>

Atom::Atom(): m_type(NoneKind) {}

Atom::Atom(double value){

  setNumber(value);
}

Atom::Atom(const Token & token): Atom(){
  
  // is token a number?
  double temp;
  std::istringstream iss(token.asString());
  if(iss >> temp){
    // check for trailing characters if >> succeeds
    if(iss.rdbuf()->in_avail() == 0){
      setNumber(temp);
    }
  }
  // else if it is not a digit, and begins with quotes "
  else if (!std::isdigit(token.asString()[0]) && (token.asString()[0] == '"')) {
	  // Set it as a string
	  setString(token.asString());
  }
  else{ // else assume symbol
    // make sure does not start with number
    if(!std::isdigit(token.asString()[0])){
      setSymbol(token.asString());
    }
  }
}

Atom::Atom(const std::string & value): Atom() {
	if (value[0] == '"') {
		setString(value);
	 }
	else {
		setSymbol(value);
	}
}

Atom::Atom(std::complex<double> value)
{
	setComplex(value);
}

Atom::Atom(const Atom & x): Atom(){
  if(x.isNumber()){
    setNumber(x.numberValue);
  }
  else if(x.isSymbol()){
    setSymbol(x.stringValue);
  }
  else if (x.isComplex()) {
	  setComplex(x.complexValue);
  }
  else if (x.isString()) {
	  setString(x.stringValue);
  }
}

Atom & Atom::operator=(const Atom & x){

  if(this != &x){
    if(x.m_type == NoneKind){
      m_type = NoneKind;
    }
    else if(x.m_type == NumberKind){
      setNumber(x.numberValue);
    }
    else if(x.m_type == SymbolKind){
      setSymbol(x.stringValue);
    }
	else if (x.m_type == ComplexKind) {
		setComplex(x.complexValue);
	}
	else if (x.m_type == ListKind) {
		setList();
	}
	else if (x.m_type == LambdaKind) {
		setLambda();
	}
	else if (x.m_type == StringKind) {
		setString(x.stringValue);
	}
	else if (x.m_type == DiscreteKind) {
		setDiscrete();
	}
  }
  return *this;
}
  
Atom::~Atom(){

  // we need to ensure the destructor of the symbol string is called
  if(m_type == SymbolKind || m_type == StringKind){
    stringValue.~basic_string();
  }
}

bool Atom::isNone() const noexcept{
  return m_type == NoneKind;
}

bool Atom::isNumber() const noexcept{
  return m_type == NumberKind;
}

bool Atom::isSymbol() const noexcept{
  return m_type == SymbolKind;
}

bool Atom::isComplex() const noexcept{
	return m_type == ComplexKind;
}

bool Atom::isList() const noexcept {
	return m_type == ListKind;
}

bool Atom::isLambda() const noexcept {
	return m_type == LambdaKind;
}

bool Atom::isString() const noexcept {
	return m_type == StringKind;
}

bool Atom::isDiscrete() const noexcept {
	return m_type == DiscreteKind;
}

void Atom::setNumber(double value){

  m_type = NumberKind;
  numberValue = value;
}

void Atom::setSymbol(const std::string & value){

  // we need to ensure the destructor of the symbol string is called
  if(m_type == SymbolKind){
    stringValue.~basic_string();
  }
    
  m_type = SymbolKind;

  // copy construct in place
  new (&stringValue) std::string(value);
}

void Atom::setComplex(std::complex<double> value){
	m_type = ComplexKind;
	complexValue = value;
}

void Atom::setList(){
	m_type = ListKind;
}

void Atom::setLambda() {
	m_type = LambdaKind;
}

void Atom::setString(const std::string & value) {
	// we need to ensure the destructor of the symbol string is called
	if (m_type == StringKind) {
		stringValue.~basic_string();
	}
	m_type = StringKind;

	// copy construct in place
	new (&stringValue) std::string(value);
}

void Atom::setDiscrete() {
	m_type = DiscreteKind;
}

double Atom::asNumber() const noexcept{

  return (m_type == NumberKind) ? numberValue : 0.0;  
}

std::string Atom::asSymbol() const noexcept{

  std::string result;

  if(m_type == SymbolKind){
    result = stringValue;
  }

  return result;
}

std::complex<double> Atom::asComplex() const noexcept{
	return (m_type == ComplexKind) ? complexValue : (0.0);
}

std::string Atom::asString() const noexcept {
	std::string result;
	std::ostringstream ostring;

	if (m_type == StringKind) {
		ostring << stringValue;
	}
	else if (m_type == ComplexKind) {
		ostring << complexValue;
	}
	else if (m_type == NumberKind) {
		ostring << numberValue;
	}
	result = ostring.str();
	return result;
}

bool Atom::operator==(const Atom & right) const noexcept{
  
  if(m_type != right.m_type) return false;

  switch(m_type){
  case NoneKind:
    if(right.m_type != NoneKind) return false;
    break;
  case NumberKind:
    {
      if(right.m_type != NumberKind) return false;
      double dleft = numberValue;
      double dright = right.numberValue;
      double diff = fabs(dleft - dright);
      if(std::isnan(diff) ||
	 (diff > (std::numeric_limits<double>::epsilon()*2))) return false;
    }
    break;
  case SymbolKind:
    {
      if(right.m_type != SymbolKind) return false;
	  return stringValue == right.stringValue;
    }
    break;
  case ComplexKind:
  {
	  if (right.m_type != ComplexKind) return false;
	  std::complex<double> cleft = complexValue;
	  std::complex<double> cright = right.complexValue;
	  std::complex<double> difference = std::abs(cleft - cright);
	  if(std::isnan(difference.real()) || (std::isnan(difference.imag())) ||
 		  (difference.real() > (std::numeric_limits<double>::epsilon()*2)) ||
		  (difference.imag() > (std::numeric_limits<double>::epsilon()*2))) return false;
  }
  break;
  case ListKind:
  {
	  if (right.m_type != ListKind) return false;
  }
  break;
  case LambdaKind:
  {
	  if (right.m_type != LambdaKind) return false;
  }
  break;
  case StringKind:
  {
	  if (right.m_type != StringKind) return false;
	  return stringValue == right.stringValue;
  }
  break;
  case DiscreteKind:
  {
	  if (right.m_type != DiscreteKind) return false;
  }
  break;
  default:
    return false;
  }

  return true;
}

bool operator!=(const Atom & left, const Atom & right) noexcept{
  
  return !(left == right);
}


std::ostream & operator<<(std::ostream & out, const Atom & a){

  if(a.isNumber()){
    out << a.asNumber();
  }
  if(a.isSymbol()){
    out << a.asSymbol();
  }
  if (a.isComplex()) {
	out << a.asComplex();
  }
  if (a.isString()) {
	  out << a.asString();
  }
  return out;
}
