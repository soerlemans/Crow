#ifndef CROW_CHECK_SYMBOL_HELPER_HPP
#define CROW_CHECK_SYMBOL_HELPER_HPP


// Includes:
#include "../ast/visitor/node_visitor.hpp"

// Local Includes:
#include "symbol_data.hpp"


namespace check {
// Using statements:
using namespace ast;

using ast::node::node_traits::typing::NativeTypeOpt;
using visitable::Any;

// Classes:
/*!
 * Helper class for dealing with @ref SymbolData in a @ref NodeVisitor context.
 */
class SymbolHelper : public visitor::NodeVisitor {
  protected:
  template<typename T, typename... Args>
  inline auto define_type_variant(Args&&... t_args) -> SymbolData
  {
    return {std::make_shared<T>(std::forward<Args>(t_args)...)};
  }

  template<typename... Args>
  inline auto define_function(Args&&... t_args) -> SymbolData
  {
    return {define_type_variant<FnType>(std::forward<Args>(t_args)...)};
  }

  template<typename... Args>
  inline auto define_variable(Args&&... t_args) -> SymbolData
  {
    return {define_type_variant<VarType>(std::forward<Args>(t_args)...)};
  }

  auto get_symbol_data(ast::node::NodePtr t_ptr) -> SymbolData;
  auto get_resolved_type(ast::node::NodePtr t_ptr) -> SymbolData;
  auto get_native_type(ast::node::NodePtr t_ptr) -> NativeTypeOpt;
  auto get_type_list(ast::node::NodeListPtr t_list) -> TypeList;

  public:
  SymbolHelper() = default;

  ~SymbolHelper() override = default;
};
} // namespace check

#endif // CROW_CHECK_SYMBOL_HELPER_HPP
