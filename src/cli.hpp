#ifndef CROW_CLI_HPP
#define CROW_CLI_HPP

// STL Includes:
#include <filesystem>
#include <sstream>
#include <vector>

// Library Includes:
#include <CLI/App.hpp>
#include <CLI/CLI.hpp>
#include <CLI/Error.hpp>
#include <CLI/Validators.hpp>
#include <rang.hpp>

// Local Includes:
#include "banner.hpp"
#include "definitions.hpp"

// Aliases:
namespace fs = std::filesystem;

// Globals:
struct Settings {
  std::vector<fs::path> m_paths;
};

// External globals:
extern Settings settings;

// Classes:
class BannerFormatter : public CLI::Formatter {
  public:
  BannerFormatter(): Formatter()
  {}

  //! Add banner to beginning of the description.
  auto make_description(const CLI::App* t_app) const -> std::string override
  {
    std::stringstream ss;

    ss << make_banner();
    ss << CLI::Formatter::make_description(t_app);

    return ss.str();
  }
};

// Functions:
auto parse_args(CLI::App& t_app, const int t_argc, char* t_argv[]) -> void;

#endif // CROW_CLI_HPP
