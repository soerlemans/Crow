# Configure clang-tidy.

if(DEFINED CROW_CLANG_TIDY)
find_program(
	CLANG_TIDY_PATH
	NAMES clang-tidy
	REQUIRED
)

set(
	CLANG_TIDY_COMMAND
	${CLANG_TIDY_PATH};
	--header-filter=src/.*;
	--extra-arg=-std=c++2b;
)

set_target_properties(
	${TARGET_CROW_LIB}
	PROPERTIES
	CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
      )

endif()
