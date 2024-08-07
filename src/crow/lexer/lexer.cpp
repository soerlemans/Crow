#include "lexer.hpp"

// STL Includes:
#include <cctype>
#include <iomanip>
#include <ios>
#include <sstream>
#include <stdexcept>
#include <utility>

// Absolute Includes:
#include "crow/debug/log.hpp"
#include "crow/exception/syntax_error.hpp"


// Macros:
#define LOG_TOKEN(t_msg, t_str) DBG_INFO(t_msg, std::quoted(std::string{t_str}))

namespace lexer {
// Using Statements:
using token::TokenType;

// Protected Methods:
auto Lexer::text_position() const -> TextPosition
{
  return m_text->position();
}

auto Lexer::syntax_error(const std::string_view t_msg) const -> void
{
  using exception::SyntaxError;

  throw SyntaxError{std::string{t_msg}, text_position()};
}

// Public methods:
Lexer::Lexer(TextStreamPtr t_text): m_text{t_text}
{}

auto Lexer::is_keyword(const std::string_view t_identifier) -> TokenTypeOpt
{
  using namespace token::reserved::keywords;

  TokenTypeOpt opt;

  if(const auto iter{g_keywords.find(t_identifier)}; iter != g_keywords.end()) {
    LOG_TOKEN("KEYWORD: ", iter->first);
    opt = iter->second;
  }

  return opt;
}

auto Lexer::identifier() -> Token
{
  using namespace token::reserved::symbols;

  Token token;
  auto position{text_position()};
  std::stringstream ss;

  auto is_valid_id{[&](const char t_char) -> bool {
    return std::isalnum(t_char) || t_char == '_';
  }};

  // Extract identifier
  if(std::isalpha(m_text->character())) {
    ss << m_text->character();
    while(!m_text->eos()) {
      if(const auto ch{m_text->peek()}; ch && is_valid_id(ch.value())) {
        m_text->next();
        ss << m_text->character();
      } else {
        break;
      }
    }
  }

  // Verify if it is a keyword or not
  if(const auto opt{is_keyword(ss.str())}; opt) {
    token = Token{opt.value(), position};
  } else {
    LOG_TOKEN("IDENTIFIER: ", ss.str());
    token = Token{TokenType::IDENTIFIER, ss.str(), position};
  }

  return token;
}

// Helper functions for literal_numeric
auto Lexer::is_hex_literal() -> bool
{
  bool is_hex{false};

  if(m_text->character() == '0' && m_text->peek() == 'x') {
    m_text->next(); // Discard '0'
    m_text->next(); // Discard 'x'

    is_hex = true;
  }

  return is_hex;
}

auto Lexer::handle_hex() -> Token
{
  auto position{text_position()};

  std::stringstream ss;

  // Extract hex
  if(std::isxdigit(m_text->character())) {
    ss << m_text->character();
    while(!m_text->eos()) {
      if(const auto ch{m_text->peek()}; ch) {
        if(std::isxdigit(ch.value())) {
          m_text->next();
          ss << m_text->character();
        } else {
          break;
        }
      }
    }
  }

  LOG_TOKEN("HEX: ", ss.str());
  const int number{(int)std::stoul(ss.str(), nullptr, 16)};

  return Token(TokenType::INTEGER, number, position);
}

// t_str and t_dot have default arguments
// t_str contains is for if you already have part of a string to continue on
// t_dot indicates if there is already a dot in the string
auto Lexer::handle_float(const std::string_view t_str) -> Token
{
  using namespace token::reserved::symbols;

  auto position{text_position()};

  std::stringstream ss;
  ss << t_str;

  while(!m_text->eos()) {
    const auto ch{m_text->peek()};
    if(ch) {
      if(std::isdigit(ch.value())) {
        m_text->next();
        ss << m_text->character();
      } else if(ch.value() == g_dot.m_identifier) {
        syntax_error("Cant have a second '.' in a float literal.");
      } else {
        break;
      }
    }
  }

  LOG_TOKEN("FLOAT: ", ss.str());
  const double number{std::stod(ss.str())};

  return Token{TokenType::FLOAT, number, position};
}

auto Lexer::handle_integer() -> Token
{
  using namespace token::reserved::symbols;

  auto position{text_position()};

  std::stringstream ss;

  // Extract integer.
  if(std::isdigit(m_text->character())) {
    ss << m_text->character();
    while(!m_text->eos()) {
      if(const auto ch{m_text->peek()}; ch) {
        if(std::isdigit(ch.value())) {
          m_text->next();
          ss << m_text->character();
        } else if(ch.value() == g_dot.m_identifier) {
          m_text->next();
          ss << m_text->character();

          return handle_float(ss.str());
        } else {
          break;
        }
      }
    }
  }

  LOG_TOKEN("INTEGER: ", ss.str());
  const int number{std::stoi(ss.str())};

  return Token{TokenType::INTEGER, number, position};
}

auto Lexer::literal_numeric() -> Token
{
  Token token;

  // Just next to the apropiate numeric literal handle function
  if(is_hex_literal()) {
    token = handle_hex();
  } else {
    // handle_integer() may also next to handle_float()
    token = handle_integer();
  }

  return token;
}

auto Lexer::literal_string() -> Token
{
  using namespace token::reserved::symbols::none;

  const auto position{text_position()};
  std::stringstream ss;

  bool quit{false};
  while(!quit && !m_text->eos()) {
    if(const auto ch{m_text->peek()}; ch) {
      m_text->next();
      switch(ch.value()) {
        case g_double_quote:
          quit = true;
          break;

        case g_backslash:
          ss << m_text->character();
          m_text->next();
          [[fallthrough]];

        default:
          ss << m_text->character();
          break;
      }
    }
  }

  LOG_TOKEN("STRING: ", ss.str());
  return Token{TokenType::STRING, ss.str(), position};
}

auto Lexer::is_multi_symbol() -> TokenTypeOpt
{
  using namespace token::reserved::symbols;

  TokenTypeOpt opt;

  std::stringstream ss;
  const char character{m_text->character()};

  ss << character;

  if(const auto ch{m_text->peek()}; ch) {
    ss << ch.value();

    if(const auto iter{g_multi_symbols.find(ss.str())};
       iter != g_multi_symbols.end()) {
      LOG_TOKEN("MULTI SYMBOL: ", iter->first);
      opt = iter->second;
      m_text->next();
    }
  }

  return opt;
}

auto Lexer::is_single_symbol() -> TokenTypeOpt
{
  using namespace token::reserved::symbols;

  TokenTypeOpt opt;
  const auto ch{m_text->character()};

  if(const auto iter{g_single_symbols.find(ch)};
     iter != g_single_symbols.end()) {
    LOG_TOKEN("SINGLE SYMBOL: ", iter->first);
    opt = iter->second;
  }

  return opt;
}

auto Lexer::symbol() -> Token
{
  const auto position{text_position()};
  TokenTypeOpt opt;

  // First check for multi symbol
  opt = is_multi_symbol();

  // Then check for single symbol
  if(!opt) {
    opt = is_single_symbol();
  }

  // Throw if it is neither
  if(!opt) {
    std::stringstream ss;
    ss << "Symbol encountered is not valid: " << m_text->character() << '\n';
    syntax_error(ss.str());
  }

  // Add the symbol if we recognize it
  return Token{opt.value(), position};
}

// TODO: refactor this
auto Lexer::tokenize() -> TokenStream
{
  using namespace token::reserved::symbols;
  using namespace token::reserved::symbols::none;

  while(!m_text->eos()) {
    const auto ch{m_text->character()};

    if(std::isspace(ch)) {
      // Just ignore whitespace, but do not ignore newlines
      if(ch == g_newline.m_identifier) {
        DBG_INFO("NEWLINE");
        m_ts.emplace_back(TokenType::NEWLINE, text_position());
      }

      // TODO: change the comment character to // and /*
    } else if(ch == '#') {
      // '#' are used for comments.
      // If we just skip to the next line we ignore the \n at the end, so we
      // Must add a NEWLINE explicitly!
      DBG_INFO("INSERTING NEWLINE");
      m_ts.emplace_back(TokenType::NEWLINE, text_position());

      // Skip to next line
      m_text->next_line();
      continue;
    } else if(std::isalpha(ch)) {
      m_ts.push_back(identifier());
    } else if(std::isdigit(ch)) {
      m_ts.push_back(literal_numeric());
    } else if(ch == g_double_quote) {
      m_ts.push_back(literal_string());
    } else {
      m_ts.push_back(symbol());
    }

    m_text->next();
  }

  return m_ts;
}
} // namespace lexer
