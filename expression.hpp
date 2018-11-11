/*! \file expression.hpp
Defines the Expression type and assiciated functions.
 */
#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <vector>
#include <map>

#include "token.hpp"
#include "atom.hpp"

// forward declare Environment
class Environment;

/*! \class Expression
\brief An expression is a tree of Atoms.

An expression is an atom called the head followed by a (possibly empty) 
list of expressions called the tail.
 */
class Expression {
public:

  typedef std::vector<Expression>::const_iterator ConstIteratorType;

  /// Default construct and Expression, whose type in NoneType
  Expression();

  /*! Construct an Expression with given Atom as head an empty tail
    \param atom the atom to make the head
  */
  Expression(const Atom & a);

  /// deep-copy construct an expression (recursive)
  Expression(const Expression & a);

  /// deep-copy construct of vector expression
  Expression(const std::vector<Expression> & a);

  /// deep-copy constructor of atom and vector expression
  Expression(const Atom & a, const std::vector<Expression> & exp);

  /// deep-copy assign an expression  (recursive)
  Expression & operator=(const Expression & a);

  /// return a reference to the head Atom
  Atom & head();

  /// return a const-reference to the head Atom
  const Atom & head() const;

  /// append Atom to tail of the expression
  void append(const Atom & a);

  /// return a pointer to the last expression in the tail, or nullptr
  Expression * tail();

  /// return a const-iterator to the beginning of tail
  ConstIteratorType tailConstBegin() const noexcept;

  /// return a const-iterator to the tail end
  ConstIteratorType tailConstEnd() const noexcept;

  /// convienience member to determine if head atom is a number
  bool isHeadNumber() const noexcept;

  /// convienience member to determine if head atom is a symbol
  bool isHeadSymbol() const noexcept;

  /// convenience member to determine if head atom is a complex
  bool isHeadComplex() const noexcept;

  /// convenience member to determine if head atom is a list
  bool isHeadList() const noexcept;

  /// convenience member to determine if head atom is a lambda
  bool isHeadLambda() const noexcept;

  /// convenience member to determine f head atom is a string
  bool isHeadString() const noexcept;

  /// Evaluate expression using a post-order traversal (recursive)
  Expression eval(Environment & env);

  /// equality comparison for two expressions (recursive)
  bool operator==(const Expression & exp) const noexcept;

  std::string transferString() const noexcept;
  
  bool isPoint() const noexcept;
  bool isLine() const noexcept;
  bool isText() const noexcept;

  double pointTail0() const noexcept;
  double pointTail1() const noexcept;
  double req() const noexcept;
  Expression textReq() const noexcept;
  double textRotReq() const noexcept;
  double lineTail0x() const noexcept;
  double lineTail0y() const noexcept;
  double lineTail1x() const noexcept;
  double lineTail1y() const noexcept;

private:

  // the head of the expression
  Atom m_head;

  // the tail list is expressed as a vector for access efficiency
  // and cache coherence, at the cost of wasted memory.
  std::vector<Expression> m_tail;

  // the property map
  std::map<std::string, Expression> propmap;

  // convenience typedef
  typedef std::vector<Expression>::iterator IteratorType;
  typedef std::vector<Expression>::iterator ListType;
  
  // internal helper methods
  Expression handle_lookup(const Atom & head, const Environment & env);
  Expression handle_define(Environment & env);
  Expression handle_begin(Environment & env);
  Expression handle_lambda(Environment & env);
  Expression handle_apply(Environment & env);
  Expression handle_map(Environment & env);
  Expression handle_set(Environment & env);
  Expression handle_get(Environment & env);
  Expression handle_discrete(Environment & env);

};

/// Render expression to output stream
std::ostream & operator<<(std::ostream & out, const Expression & exp);

/// inequality comparison for two expressions (recursive)
bool operator!=(const Expression & left, const Expression & right) noexcept;
  
#endif
