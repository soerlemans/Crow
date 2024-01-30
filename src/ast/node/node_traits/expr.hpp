#ifndef CROW_AST_NODE_NODE_TRAITS_EXPR_HPP
#define CROW_AST_NODE_NODE_TRAITS_EXPR_HPP

// Includes:
#include "../node_interface.hpp"

// Local Includes:
#include "fdecl.hpp"


namespace ast::node::node_traits {
class Expr : virtual public NodeInterface {
  protected:
  NodePtr m_expr;

  public:
  Expr(NodePtr&& t_expr);

  virtual auto expr() -> NodePtr&;

  MAKE_ARCHIVEABLE(Expr)
  {
		t_archive(CEREAL_NVP(m_expr));
	}

  VISITABLE_PURE_ACCEPT(visitor::NodeVisitor);

  virtual ~Expr() = default;
};
} // namespace ast::node::node_traits

// Cereal register type:
REGISTER_ARCHIVEABLE_TYPE(ast::node::node_traits, Expr);

#endif // CROW_AST_NODE_NODE_TRAITS_EXPR_HPP
