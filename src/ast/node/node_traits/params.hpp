#ifndef CROW_AST_NODE_NODE_TRAITS_PARAMS_HPP
#define CROW_AST_NODE_NODE_TRAITS_PARAMS_HPP

// Includes:
#include "../node_interface.hpp"

// Local Includes:
#include "fdecl.hpp"


namespace ast::node::node_traits {
class Params : virtual public NodeInterface {
  protected:
  NodeListPtr m_params;

  public:
  Params(NodeListPtr&& t_body);

  virtual auto params() -> NodeListPtr&;

  auto accept(visitor::NodeVisitor* t_visitor) -> void override = 0;

  ~Params() override = default;
};
} // namespace ast::node::node_traits

#endif // CROW_AST_NODE_NODE_TRAITS_PARAMS_HPP
