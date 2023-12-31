#ifndef CROW_AST_NODE_LVALUE_LET_HPP
#define CROW_AST_NODE_LVALUE_LET_HPP

// Includes:
#include "../node_traits/include.hpp"

// Local Includes:
#include "fdecl.hpp"


namespace ast::node::lvalue {
// Aliases:
namespace nt = node_traits;
namespace ct = container;

// Classes:
class Let : public nt::DeclExpr {
  public:
  Let(ct::TextPosition t_pos, std::string_view t_identifier,
      std::string_view t_str, NodePtr&& t_init);

  MAKE_VISITABLE(visitor::NodeVisitor);

  ~Let() override = default;
};
} // namespace ast::node::lvalue

#endif // CROW_AST_NODE_LVALUE_LET_HPP
