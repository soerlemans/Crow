# Find libraries
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

# Tabulate (Terminal table library):
FetchContent_Declare(
  tabulate
  GIT_REPOSITORY https://github.com/p-ranav/tabulate.git
  GIT_TAG v1.5
  GIT_PROGRESS TRUE)

FetchContent_MakeAvailable(tabulate)

# Pthread support:
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# CLI11 (Terminal flag library):
find_package(CLI11 CONFIG REQUIRED)

# LLVM (Compiler toolchain libraries):
find_package(LLVM 15 CONFIG REQUIRED)

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

llvm_map_components_to_libnames(LLVM_LIBS
	support
	core
	irreader
	codegen
)

# Link libraries:
target_link_libraries(
	${PROJECT_NAME}
	tabulate::tabulate
	Threads::Threads
	CLI11::CLI11
	${LLVM_LIBS}
)
