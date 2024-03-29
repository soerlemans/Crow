#include "text_position.hpp"

// STL Includes:
#include <sstream>

namespace container {
// Methods:
TextPosition::TextPosition(): m_lineno{0}, m_columno{0}
{}

// Its important to note that lines are zero indexed
TextPosition::TextPosition(const std::string_view t_source,
                           const std::string_view t_line, std::size_t t_lineno,
                           std::size_t t_columnno)
  : m_source{t_source},
    m_line{t_line},
    m_lineno{t_lineno},
    m_columno{t_columnno}
{}

// Friend methods:
auto operator<<(std::ostream& t_os, const TextPosition& t_fp) -> std::ostream&
{
  std::stringstream ss;
  // To print the correct line number we have to increment by one since it is
  // Zero indexed
  ss << "Line(" << t_fp.m_lineno + 1 << "): ";
  const auto offset{ss.str().size() + t_fp.m_columno};

  t_os << "Source: \"" << t_fp.m_source << "\"\n";
  t_os << ss.str() << t_fp.m_line;
  t_os << std::string(offset, ' ') << "^";

  return t_os;
}
} // namespace container
