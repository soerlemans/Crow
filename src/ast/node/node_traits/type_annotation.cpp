#include "type_annotation.hpp"


using namespace ast::node::node_traits;

TypeAnnotation::TypeAnnotation(const std::string_view t_type): m_type{t_type}
{}

auto TypeAnnotation::type() const -> std::string_view
{
  return m_type;
}
