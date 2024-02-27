#ifndef CROW_VERSION_HPP
#define CROW_VERSION_HPP

// STL Includes:
#include <string_view>


// Macros:
//! Cmake substitutes the version numbers specified in the root CMakeLists.txt.
#define CROW_PROJECT_VERSION_MAJOR @crow_VERSION_MAJOR @
#define CROW_PROJECT_VERSION_MINOR @crow_VERSION_MINOR @
#define CROW_PROJECT_VERSION_PATCH @crow_VERSION_PATCH @

#define CROW_PROJECT_VERSION "0.4.0"

// TODO: Move elsewhere?
//! Absolute path to the root of the crow project.
#define CROW_PROJECT_ROOT_DIRECTORY "/home/hackerman/crow"

//! Absolute path to the source directory of the crow project
#define CROW_PROJECT_SRC_DIRECTORY "/home/hackerman/crow/src"

/*!
 * Convert @ref __FILE__ into a relative path.
 * If this is not possible return the absolute path.
 */
#define CROW_PROJECT_RELATIVE_PATH                                           \
  ([]() consteval {                                                          \
    constexpr std::string_view file{__FILE__};                               \
    constexpr std::string_view src_dir{CROW_PROJECT_SRC_DIRECTORY "/"};      \
                                                                             \
    if constexpr(file.starts_with(src_dir)) {                                \
      constexpr std::string_view relative_path{file.substr(src_dir.size())}; \
                                                                             \
      return relative_path;                                                  \
    }                                                                        \
                                                                             \
    return file;                                                             \
  }())

#endif // CROW_PROJECT_VERSION_HPP