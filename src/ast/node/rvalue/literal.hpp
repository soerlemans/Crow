#ifndef CROW_AST_NODE_RVALUE_LITERAL_HPP
#define CROW_AST_NODE_RVALUE_LITERAL_HPP

// Includes:
#include "../node_interface.hpp"

// Local Includes:
#include "fdecl.hpp"


namespace ast::node::rvalue {
// Classes:
template<typename T>
class Literal : public NodeInterface {
  private:
  T m_value;

  public:
  // Methods:
  Literal(const T t_value): m_value{t_value}
  {}

  auto get() -> T
  {
    return m_value;
  }

  MAKE_ARCHIVEABLE(Literal)
  {
    t_archive(CEREAL_NVP(m_value));
  }

  AST_VISITOR_MAKE_VISITABLE(visitor::NodeVisitor);

  virtual ~Literal() = default;
};

} // namespace ast::node::rvalue

// Cereal type registration:
REGISTER_ARCHIVEABLE_TYPE(ast::node::rvalue, Integer);
REGISTER_ARCHIVEABLE_TYPE(ast::node::rvalue, Float);
REGISTER_ARCHIVEABLE_TYPE(ast::node::rvalue, String);
REGISTER_ARCHIVEABLE_TYPE(ast::node::rvalue, Boolean);

#endif // CROW_AST_NODE_RVALUE_LITERAL_HPP
