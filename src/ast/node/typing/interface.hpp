#ifndef CROW_AST_NODE_TYPING_INTERFACE_HPP
#define CROW_AST_NODE_TYPING_INTERFACE_HPP


// Includes:
#include "../node_traits/include.hpp"

// Local Includes:
#include "fdecl.hpp"


namespace ast::node::typing {
// Using Statements:
using node_traits::Identifier;

// Classes:
class Interface : public Identifier {
  private:
  NodeListPtr m_methods;

  public:
  Interface(std::string_view t_identifier, NodeListPtr&& t_methods);

  virtual auto methods() -> NodeListPtr&;

  MAKE_TRAITS_ARCHIVEABLE(Interface, Identifier)
  MAKE_VISITABLE(visitor::NodeVisitor);

  virtual ~Interface() = default;
};
} // namespace ast::node::typing

// Cereal type registration:
REGISTER_ARCHIVEABLE_TYPE(ast::node::typing, Interface);

#endif // CROW_AST_NODE_TYPING_INTERFACE_HPP
