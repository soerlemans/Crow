#ifndef CROW_CROW_AST_NODE_NODE_TRAITS_PARAMS_HPP
#define CROW_CROW_AST_NODE_NODE_TRAITS_PARAMS_HPP

// Includes:
#include "../include.hpp"

// Local Includes:
#include "fdecl.hpp"

namespace ast::node::node_traits {
class Params : virtual public NodeInterface {
  private:
  NodeListPtr m_params;

  public:
  Params(NodeListPtr&& t_params);

  auto params() -> NodeListPtr&;

  AST_ARCHIVE_MAKE_ARCHIVEABLE(Params)
  {
    t_archive(CEREAL_NVP(m_params));
  }

  AST_VISITOR_VISITABLE_PURE_ACCEPT(visitor::NodeVisitor);

  virtual ~Params() = default;
};
} // namespace ast::node::node_traits

// Cereal register type:
REGISTER_ARCHIVEABLE_TYPE(ast::node::node_traits, Params);

#endif // CROW_CROW_AST_NODE_NODE_TRAITS_PARAMS_HPP
