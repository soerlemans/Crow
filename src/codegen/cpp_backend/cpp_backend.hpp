#ifndef CROW_CODEGEN_CPP_BACKEND_CPP_BACKEND_HPP
#define CROW_CODEGEN_CPP_BACKEND_CPP_BACKEND_HPP

// STL Includes:
#include <filesystem>
#include <format>
#include <fstream>
#include <memory>

// Includes:
#include "../../ast/visitor/node_visitor.hpp"

namespace codegen::cpp_backend {
// Using statements:
using namespace ast;

// Using Declarations:
using ast::node::NodePtr;
using ast::visitor::Any;

// Namespace aliases:
namespace fs = std::filesystem;

// Classes:
/*!
 * @brief Tree walk codegenerator generating safe C++ code.
 * The generated C++ is stored in a cross platform location.
 * This generated C++ code could later be compiled with libclang.
 * Or any compiler of choice.
 */
class CppBackend : public ast::visitor::NodeVisitor {
  private:
  std::ofstream m_ofs;

  protected:
  template<typename... Args>
  auto write(const std::string_view t_fmt, Args&&... t_args) -> void
  {
    const auto fmt_args{std::make_format_args(std::forward<Args>(t_args)...)};

    m_ofs << std::vformat(t_fmt, fmt_args);
    m_ofs << '\n';
  }

  public:
  CppBackend() = default;

  // Control:
  auto visit(node::control::If* t_if) -> Any override;
  auto visit(node::control::Loop* t_loop) -> Any override;
  auto visit(node::control::Continue* t_continue) -> Any override;
  auto visit(node::control::Break* t_break) -> Any override;
  auto visit(node::control::Return* t_return) -> Any override;

  // Function:
  auto visit(node::function::Parameter* t_param) -> Any override;
  auto visit(node::function::Function* t_fn) -> Any override;
  auto visit(node::function::Call* t_fn_call) -> Any override;
  auto visit(node::function::ReturnType* t_rt) -> Any override;

  // Lvalue:
  auto visit(node::lvalue::Let* t_let) -> Any override;
  auto visit(node::lvalue::Var* t_var) -> Any override;
  auto visit(node::lvalue::Variable* t_var) -> Any override;

  // Operators:
  auto visit(node::operators::Arithmetic* t_arith) -> Any override;
  auto visit(node::operators::Assignment* t_assign) -> Any override;
  auto visit(node::operators::Comparison* t_comp) -> Any override;

  auto visit(node::operators::Increment* t_inc) -> Any override;
  auto visit(node::operators::Decrement* t_dec) -> Any override;

  auto visit(node::operators::UnaryPrefix* t_up) -> Any override;

  // Logical:
  auto visit(node::operators::Not* t_not) -> Any override;
  auto visit(node::operators::And* t_and) -> Any override;
  auto visit(node::operators::Or* t_or) -> Any override;

  auto visit(node::operators::Ternary* t_ternary) -> Any override;

  // Packaging:
  auto visit(node::packaging::Import* t_import) -> Any override;
  auto visit(node::packaging::ModuleDecl* t_mod) -> Any override;

  // Rvalue:
  auto visit(node::rvalue::Float* t_float) -> Any override;
  auto visit(node::rvalue::Integer* t_int) -> Any override;
  auto visit(node::rvalue::String* t_str) -> Any override;
  auto visit(node::rvalue::Boolean* t_bool) -> Any override;

  // Typing:
  auto visit(ast::node::typing::MethodDecl* t_md) -> Any override;
  auto visit(ast::node::typing::Interface* t_ifc) -> Any override;
  auto visit(ast::node::typing::MemberDecl* t_md) -> Any override;
  auto visit(ast::node::typing::Struct* t_struct) -> Any override;
  auto visit(ast::node::typing::Impl* t_impl) -> Any override;
  auto visit(ast::node::typing::DotExpr* t_dot_expr) -> Any override;

  // Util:
  auto codegen(NodePtr t_ast) -> void;
  auto compile(fs::path t_path) -> void;

  virtual ~CppBackend() = default;
};
} // namespace codegen::cpp_backend

#endif // CROW_CODEGEN_CPP_BACKEND_CPP_BACKEND_HPP
