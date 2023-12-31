#include "decrement.hpp"


using namespace ast::node::operators;
using namespace ast::node::node_traits;

Decrement::Decrement(NodePtr&& t_left, bool t_prefix)
  : UnaryOperator{std::move(t_left)}, m_prefix{t_prefix}
{}

auto Decrement::prefix() const -> bool
{
  return m_prefix;
}
