#ifndef CROW_CROW_AST_NODE_NODE_TRAITS_UNARY_OPERATOR_HPP
#define CROW_CROW_AST_NODE_NODE_TRAITS_UNARY_OPERATOR_HPP

// Includes:
#include "../include.hpp"

// Local Includes:
#include "fdecl.hpp"

namespace ast::node::node_traits {
class UnaryOperator : virtual public NodeInterface {
  protected:
  NodePtr m_left;

  public:
  explicit UnaryOperator(NodePtr&& t_left);

  virtual auto left() -> NodePtr&;

  AST_ARCHIVE_MAKE_ARCHIVEABLE(UnaryOperator)
  {
    t_archive(CEREAL_NVP(m_left));
  }

  AST_VISITOR_VISITABLE_PURE_ACCEPT(visitor::NodeVisitor);

  virtual ~UnaryOperator() = default;
};
} // namespace ast::node::node_traits

// Cereal type registration:
REGISTER_ARCHIVEABLE_TYPE(ast::node::node_traits, UnaryOperator);

#endif // CROW_CROW_AST_NODE_NODE_TRAITS_UNARY_OPERATOR_HPP
