#ifndef CROW_CROW_AST_NODE_NODE_TRAITS_OP_HPP
#define CROW_CROW_AST_NODE_NODE_TRAITS_OP_HPP

// Includes:
#include "../include.hpp"

// Local Includes:
#include "fdecl.hpp"

namespace ast::node::node_traits {
template<typename T>
class Op : virtual public NodeInterface {
  private:
  T m_op;

  public:
  Op(const T t_op): m_op{t_op}
  {}

  auto op() const -> T
  {
    return m_op;
  }

  /*!
   */
  virtual auto op2str() const -> std::string_view = 0;

  AST_ARCHIVE_MAKE_ARCHIVEABLE(Op)
  {
    t_archive(CEREAL_NVP(m_op));
  }

  AST_VISITOR_VISITABLE_PURE_ACCEPT(visitor::NodeVisitor);

  virtual ~Op() = default;
};
} // namespace ast::node::node_traits

#endif // CROW_CROW_AST_NODE_NODE_TRAITS_OP_HPP
