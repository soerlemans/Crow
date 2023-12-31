#include "unary_prefix.hpp"


using namespace ast::node::operators;
using namespace ast::node::node_traits;

UnaryPrefix::UnaryPrefix(UnaryPrefixOp t_op, NodePtr&& t_left)
  : UnaryOperator{std::move(t_left)}, m_op{t_op}
{}

auto UnaryPrefix::op() const -> UnaryPrefixOp
{
  return m_op;
}
