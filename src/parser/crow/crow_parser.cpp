#include "crow_parser.hpp"

// Includes:
#include "../../ast/node/include.hpp"
#include "../../debug/trace.hpp"
#include <exception>
#include <string>


// Using statements:
using namespace parser::crow;

using namespace token;

using namespace ast::node;
using namespace ast::node::control;
using namespace ast::node::functions;
using namespace ast::node::lvalue;
using namespace ast::node::operators;
using namespace ast::node::packaging;
using namespace ast::node::rvalue;
using namespace ast::node::typing;

// Aliases:
namespace nt = ast::node::node_traits;

// Methods:
CrowParser::CrowParser(TokenStream t_tokenstream)
  : PrattParser{std::move(t_tokenstream)}
{}

auto CrowParser::newline_opt() -> void
{
  DBG_TRACE_FN(VERBOSE);

  while(!eos() && next_if(TokenType::NEWLINE)) {
    DBG_TRACE_PRINT(INFO, "Found 'NEWLINE'");
  }
}

auto CrowParser::terminator() -> void
{
  DBG_TRACE_FN(VERBOSE);

  const auto is_terminator{[this] {
    return next_if(TokenType::SEMICOLON, TokenType::NEWLINE);
  }};

  if(!is_terminator()) {
    syntax_error("Expected at least one 'TERMINATOR'");
  }

  while(!eos()) {
    if(is_terminator()) {
      DBG_TRACE_PRINT(INFO, "Found 'TERMINATOR'");
    } else {
      break;
    }
  }
}

auto CrowParser::expr_opt() -> NodePtr
{
  DBG_TRACE_FN(VERBOSE);

  return expr();
}

auto CrowParser::decl_expr() -> NodePtr
{
  DBG_TRACE_FN(VERBOSE);
  NodePtr node;

  if(next_if(TokenType::LET)) {
    PARSER_FOUND(TokenType::LET);
    const auto id{expect(TokenType::IDENTIFIER)};

    if(next_if(TokenType::COLON)) {
      expect(TokenType::IDENTIFIER);

      // TODO: Implement type inference
    }

    NodePtr expr_ptr;
    if(next_if(TokenType::ASSIGNMENT)) {
      newline_opt();
      expr_ptr = expr();
    }

    node = make_node<Let>(id.str(), std::move(expr_ptr));
  }

  return node;
}

auto CrowParser::eval_expr() -> NodePair
{
  DBG_TRACE_FN(VERBOSE);
  NodePair pair;

  if(auto ptr{decl_expr()}; ptr) {
    expect(TokenType::SEMICOLON);

    pair = {std::move(ptr), expr()};
  } else if(auto ptr{expr()}; ptr) {
    if(next_if(TokenType::SEMICOLON)) {
      pair = {std::move(ptr), expr()};
    } else {
      pair.second = std::move(ptr);
    }
  }

  return pair;
}

auto CrowParser::expr_statement() -> NodePtr
{
  DBG_TRACE_FN(VERBOSE);
  NodePtr node;

  if(auto ptr{expr()}; ptr) {
    terminator();
    node = std::move(ptr);
  }

  return node;
}

auto CrowParser::multiple_expr_list() -> NodeListPtr
{
  DBG_TRACE_FN(VERBOSE);
  auto nodes{make_node<List>()};

  if(auto ptr{expr()}; ptr) {
    DBG_TRACE_PRINT(INFO, "Found 'EXPR");

    nodes->push_back(std::move(ptr));
  }

  while(!eos()) {
    if(next_if(TokenType::COMMA)) {
      newline_opt();
      if(auto ptr{expr()}; ptr) {
        DBG_TRACE_PRINT(INFO, "Found ',' 'EXPR'");

        nodes->push_back(std::move(ptr));
      } else {
        syntax_error("Expected another expression after ','");
      }
    } else {
      break;
    }
  }

  return nodes;
}

auto CrowParser::expr_list() -> NodeListPtr
{
  DBG_TRACE_FN(VERBOSE);
  auto nodes{expr_list_opt()};

  if(nodes->empty()) {
    syntax_error("Expected a list of expressions");
  }

  return nodes;
}

auto CrowParser::expr_list_opt() -> NodeListPtr
{
  DBG_TRACE_FN(VERBOSE);
  auto nodes{make_node<List>()};

  if(auto ptr{expr()}; ptr) {
    nodes->push_back(std::move(ptr));
  } else if(auto ptr{multiple_expr_list()}; ptr) {
    nodes = std::move(ptr);
  }

  return nodes;
}

// Result statement:
auto CrowParser::assignment() -> NodePtr
{
  DBG_TRACE_FN(VERBOSE);
  NodePtr node;

  if(auto lhs{lvalue()}; lhs) {
    const auto token{get_token()};
    const auto lambda{[&](AssignmentOp t_op) {
      newline_opt();
      if(auto rhs{expr()}; rhs) {
        node = make_node<Assignment>(t_op, std::move(lhs), std::move(rhs));
      }
    }};

    if(next_if(TokenType::MUL_ASSIGN)) {
      PARSER_FOUND(TokenType::MUL_ASSIGN);
      lambda(AssignmentOp::MULTIPLY);

    } else if(next_if(TokenType::DIV_ASSIGN)) {
      PARSER_FOUND(TokenType::DIV_ASSIGN);
      lambda(AssignmentOp::DIVIDE);

    } else if(next_if(TokenType::MOD_ASSIGN)) {
      PARSER_FOUND(TokenType::MOD_ASSIGN);
      lambda(AssignmentOp::MODULO);

    } else if(next_if(TokenType::ADD_ASSIGN)) {
      PARSER_FOUND(TokenType::ADD_ASSIGN);
      lambda(AssignmentOp::ADD);

    } else if(next_if(TokenType::SUB_ASSIGN)) {
      PARSER_FOUND(TokenType::SUB_ASSIGN);
      lambda(AssignmentOp::SUBTRACT);

    } else if(next_if(TokenType::ASSIGNMENT)) {
      PARSER_FOUND(TokenType::ASSIGNMENT);
      lambda(AssignmentOp::REGULAR);
    }
  }

  return node;
}

auto CrowParser::result_statement() -> n::NodePtr
{
  DBG_TRACE_FN(VERBOSE);
  NodePtr node;

  if(auto ptr{decl_expr()}; ptr) {
    node = std::move(ptr);
  } else if(auto ptr{assignment()}; ptr) {
    node = std::move(ptr);
  } else if(auto ptr{precrement()}; ptr) {
    node = std::move(ptr);
  }

  // Terminate result statement
  if(node) {
    terminator();
  }

  return node;
}

auto CrowParser::jump_statement() -> NodePtr
{
  DBG_TRACE_FN(VERBOSE);
  NodePtr node;

  if(next_if(TokenType::CONTINUE)) {
    terminator();
    node = make_node<Continue>();
  } else if(next_if(TokenType::BREAK)) {
    terminator();
    node = make_node<Break>();
  } else if(next_if(TokenType::DEFER)) {
    auto body_ptr{make_node<List>()};
    if(auto ptr{expr_statement()}; ptr) {
      body_ptr->push_back(std::move(ptr));
    } else if(auto ptr{body()}; ptr) {
      body_ptr = std::move(ptr);
    }

    node = make_node<Defer>(std::move(body_ptr));
  } else if(next_if(TokenType::RETURN)) {
    auto expr_ptr{expr_opt()};
    terminator();

    node = make_node<Return>(std::move(expr_ptr));
  }

  return node;
}

auto CrowParser::loop_statement() -> NodePtr
{
  DBG_TRACE_FN(VERBOSE);
  NodePtr node;

  if(next_if(TokenType::LOOP)) {
    auto [init, cond] = eval_expr();

    NodePtr expr_ptr;
    if(next_if(TokenType::SEMICOLON)) {
      expr_ptr = expr();
    }

    auto body_ptr{body()};

    node = make_node<Loop>(std::move(init), std::move(cond),
                           std::move(expr_ptr), std::move(body_ptr));
  }

  return node;
}

auto CrowParser::branch_statement(const TokenType t_type) -> NodePtr
{
  DBG_TRACE_FN(VERBOSE);
  NodePtr node;

  if(next_if(t_type)) {
    auto [init, cond] = eval_expr();
    if(!cond) {
      syntax_error("Expected a conditional");
    }

    auto then_ptr{body()};

    NodeListPtr alt_ptr;
    if(next_if(TokenType::ELSE)) {
      alt_ptr = body();
    } else if(auto ptr{branch_statement(TokenType::ELIF)}; ptr) {
      // Elifs are structured in the AST as just an if in an else branch
      alt_ptr = make_node<List>();
      alt_ptr->push_back(std::move(ptr));
    }

    node = make_node<If>(std::move(init), std::move(cond), std::move(then_ptr),
                         std::move(alt_ptr));
  }

  return node;
}

auto CrowParser::if_statement() -> NodePtr
{
  DBG_TRACE_FN(VERBOSE);
  NodePtr node;

  return branch_statement(TokenType::IF);
}

auto CrowParser::elif_statement() -> NodePtr
{
  DBG_TRACE_FN(VERBOSE);

  return branch_statement(TokenType::ELIF);
}

auto CrowParser::statement() -> NodePtr
{
  DBG_TRACE_FN(VERBOSE);
  NodePtr node;

  if(auto ptr{decl_expr()}; ptr) {
    terminator();
    node = std::move(ptr);
  } else if(auto ptr{result_statement()}; ptr) {
    node = std::move(ptr);
  } else if(auto ptr{if_statement()}; ptr) {
    node = std::move(ptr);
  } else if(auto ptr{loop_statement()}; ptr) {
    node = std::move(ptr);
  } else if(auto ptr{jump_statement()}; ptr) {
    node = std::move(ptr);
  } else if(auto ptr{body()}; ptr) {
    node = std::move(ptr);
  }

  return node;
}

auto CrowParser::statement_list() -> NodeListPtr
{
  DBG_TRACE_FN(VERBOSE);
  auto nodes{make_node<List>()};

  while(!eos()) {
    if(auto ptr{statement()}; ptr) {
      nodes->push_back(std::move(ptr));
    } else {
      break;
    }

    newline_opt();
  }

  return nodes;
}

// Body:
/*! The body() should never be optional, or else we will consume newlines
 * unintentionally
 */
auto CrowParser::body() -> NodeListPtr
{
  DBG_TRACE_FN(VERBOSE);
  NodeListPtr nodes;

  // After a list of newlines an accolade most occur
  if(after_newlines(TokenType::ACCOLADE_OPEN)) {
    expect(TokenType::ACCOLADE_OPEN);

    newline_opt();
    if(auto ptr{statement_list()}; ptr) {
      nodes = std::move(ptr);
    }

    expect(TokenType::ACCOLADE_CLOSE);
  }

  return nodes;
}

// Interface:
auto CrowParser::method_decl() -> NodePtr
{
  DBG_TRACE_FN(VERBOSE);
  NodePtr node;

  if(next_if(TokenType::FUNCTION)) {
    const auto id{expect(TokenType::IDENTIFIER).str()};

    // NodeListPtr params;
    auto params{parens([this] {
      return this->param_list_opt();
    })};

    const auto type{return_type()};

    terminator();

    node = make_node<MethodDecl>(id, std::move(params), type);
  }

  return node;
}

auto CrowParser::method_decl_list() -> NodeListPtr
{
  DBG_TRACE_FN(VERBOSE);
  auto nodes{make_node<List>()};

  while(!eos()) {
    if(auto ptr{method_decl()}; ptr) {
      nodes->push_back(std::move(ptr));
    } else {
      break;
    }
  }

  return nodes;
}

auto CrowParser::interface_def() -> NodePtr
{
  DBG_TRACE_FN(VERBOSE);
  NodePtr node;

  if(next_if(TokenType::INTERFACE)) {
    const auto id{expect(TokenType::IDENTIFIER).str()};
    newline_opt();

    auto methods{accolades([this] {
      newline_opt();

      return method_decl_list();
    })};

    node = make_node<Interface>(id, std::move(methods));
  }

  return node;
}

// Struct:
auto CrowParser::member_decl() -> NodePtr
{
  DBG_TRACE_FN(VERBOSE);
  NodePtr node;

  const auto token{get_token()};
  if(next_if(TokenType::IDENTIFIER)) {
    const auto id{token.str()};
    expect(TokenType::COLON);

    const auto type{expect(TokenType::IDENTIFIER).str()};
    terminator();

    node = make_node<MemberDecl>(id, type);
  }

  return node;
}

auto CrowParser::member_decl_list() -> NodeListPtr
{
  DBG_TRACE_FN(VERBOSE);
  auto nodes{make_node<List>()};

  while(!eos()) {
    if(auto ptr{member_decl()}; ptr) {
      nodes->push_back(std::move(ptr));
    } else {
      break;
    }
  }

  return nodes;
}

auto CrowParser::struct_def() -> NodePtr
{
  DBG_TRACE_FN(VERBOSE);
  NodePtr node;

  if(next_if(TokenType::STRUCT)) {
    PARSER_FOUND(TokenType::STRUCT);

    const auto id{expect(TokenType::IDENTIFIER).str()};
    newline_opt();

    auto members{accolades([this] {
      newline_opt();

      return member_decl_list();
    })};

    node = make_node<Struct>(id, std::move(members));
  }

  return node;
}

auto CrowParser::type_def() -> NodePtr
{
  DBG_TRACE_FN(VERBOSE);
  NodePtr node;

  if(auto ptr{interface_def()}; ptr) {
    node = std::move(ptr);
  } else if(auto ptr{struct_def()}; ptr) {
    node = std::move(ptr);
  }

  return node;
}

// Function:
auto CrowParser::param_list() -> NodeListPtr
{
  DBG_TRACE_FN(VERBOSE);
  auto nodes{param_list_opt()};

  if(nodes->empty()) {
    syntax_error("Expected list of parameters");
  }

  return nodes;
}

auto CrowParser::param_list_opt() -> NodeListPtr
{
  DBG_TRACE_FN(VERBOSE);
  auto nodes{make_node<List>()};

  if(check(TokenType::IDENTIFIER)) {
    PARSER_FOUND(TokenType::IDENTIFIER);

    const auto id{expect(TokenType::IDENTIFIER).str()};
    expect(TokenType::COLON);
    const auto type{expect(TokenType::IDENTIFIER).str()};

    nodes->push_back(make_node<Variable>(id, type));
  }

  while(!eos()) {
    if(next_if(TokenType::COMMA)) {
      const auto id{expect(TokenType::IDENTIFIER).str()};
      PARSER_FOUND(TokenType::COMMA, "'IDENTIFIER'");

      expect(TokenType::COLON);
      const auto type{expect(TokenType::IDENTIFIER).str()};

      nodes->push_back(make_node<Variable>(id, type));
    } else {
      break;
    }
  }

  return nodes;
}

// TODO: Create a StrOpt type
auto CrowParser::return_type() -> std::string
{
  DBG_TRACE_FN(VERBOSE);
  const auto type{return_type_opt()};

  if(type.empty()) {
    syntax_error("Expected a return type!");
  }

  return type;
}

auto CrowParser::return_type_opt() -> std::string
{
  DBG_TRACE_FN(VERBOSE);
  std::string type;

  if(after_newlines(TokenType::ARROW)) {
    expect(TokenType::ARROW);
    type = expect(TokenType::IDENTIFIER).str();
  }

  return type;
}

auto CrowParser::lambda() -> n::NodePtr
{
  DBG_TRACE_FN(VERBOSE);
  NodePtr node;

  if(next_if(TokenType::FUNCTION)) {
    auto params{parens([this] {
      return this->param_list_opt();
    })};
  }

  return node;
}

auto CrowParser::function() -> NodePtr
{
  DBG_TRACE_FN(VERBOSE);
  NodePtr node;

  if(next_if(TokenType::FUNCTION)) {
    const auto tt_id{expect(TokenType::IDENTIFIER)};
    const auto id{tt_id.str()};

    // NodeListPtr params;
    auto params{parens([this] {
      return this->param_list_opt();
    })};

    const auto type{return_type_opt()};

    auto body_ptr{body()};
    if(!body_ptr) {
      syntax_error("Expected a function body");
    }

    node =
      make_node<Function>(id, std::move(params), type, std::move(body_ptr));
  }

  return node;
}

auto CrowParser::import_expr(Import& t_import) -> bool
{
  DBG_TRACE_FN(VERBOSE);
  bool is_import_expr{true};

  const auto token{get_token()};
  if(next_if(TokenType::STRING)) {
    auto str{token.str()};
    DBG_TRACE_PRINT(INFO, "Found: ", std::quoted(str));

    t_import.add_import(std::move(str));
  } else if(next_if(TokenType::IDENTIFIER)) {
    auto id{token.str()};

    expect(TokenType::ASSIGNMENT);
    auto str{expect(TokenType::STRING).str()};

    DBG_TRACE_PRINT(INFO, "Found alias: ", id, " = ", std::quoted(str));

    t_import.add_import({str, id});
  } else {
    is_import_expr = false;
  }

  return is_import_expr;
}

auto CrowParser::import_list(Import& t_import) -> void
{
  DBG_TRACE_FN(VERBOSE);

  if(!import_expr(t_import)) {
    syntax_error("Expected at least one import expression!");
  }

  while(!eos()) {
    if(check(TokenType::SEMICOLON)) {
      terminator();

      if(!import_expr(t_import)) {
        syntax_error("Expected another import expression!");
      }
    } else if(next_if(TokenType::NEWLINE)) {
      newline_opt();

      if(!import_expr(t_import)) {
        break;
      }
    } else {
      break;
    }
  }
}

auto CrowParser::import_() -> NodePtr
{
  DBG_TRACE_FN(VERBOSE);
  NodePtr node;

  if(next_if(TokenType::IMPORT)) {
    PARSER_FOUND(TokenType::IMPORT);

    auto import_ptr{make_node<Import>()};
    if(check(TokenType::STRING)) {
      PARSER_FOUND(TokenType::STRING);
      const auto id{get_token()};
      next();

      import_ptr->add_import(id.str());
    } else if(next_if(TokenType::PAREN_OPEN)) {
      newline_opt();
      import_list(*import_ptr);
      expect(TokenType::PAREN_CLOSE);
    } else {
      syntax_error("Expected string or an import list");
    }

    node = std::move(import_ptr);
  }

  return node;
}

auto CrowParser::module_() -> NodePtr
{
  DBG_TRACE_FN(VERBOSE);
  NodePtr node;

  if(next_if(TokenType::MODULE)) {
    PARSER_FOUND(TokenType::MODULE);
    const auto id{expect(TokenType::IDENTIFIER)};

    node = make_node<ModuleDecl>(id.str());
  }

  return node;
}

auto CrowParser::item() -> NodePtr
{
  DBG_TRACE_FN(VERBOSE);
  NodePtr node;

  if(auto ptr{module_()}; ptr) {
    node = std::move(ptr);
  } else if(auto ptr{import_()}; ptr) {
    node = std::move(ptr);
  } else if(auto ptr{type_def()}; ptr) {
    node = std::move(ptr);
  } else if(auto ptr{decl_expr()}; ptr) {
    node = std::move(ptr);
  } else if(auto ptr{function()}; ptr) {
    node = std::move(ptr);
  }

  return node;
}

// item list exists out of an item followed by a terminator
// Till there are are no more items
auto CrowParser::item_list() -> NodeListPtr
{
  DBG_TRACE_FN(VERBOSE);
  auto nodes{make_node<List>()};

  while(!eos()) {
    // Remove newlines before items
    newline_opt();
    if(eos()) {
      break;
    }

    if(auto ptr{item()}; ptr) {
      nodes->push_back(std::move(ptr));
    } else {
      break;
    }
  }

  return nodes;
}

auto CrowParser::program() -> NodeListPtr
{
  DBG_TRACE_FN(VERBOSE);

  return item_list();
}

auto CrowParser::parse() -> NodePtr
{
  return program();
}
