# Configure the compiler settings.

# Functions:
function(COMPILE_FLAGS T_TARGET)


	target_compile_options(${T_TARGET} PRIVATE
		-Wall
		-Wextra
		-pedantic
	)
endfunction()


# Set some compiler options:
set_target_properties(
	${TARGET_CROW}
	${TARGET_CROW_LIB}
	PROPERTIES
	CXX_STANDARD 23
	CXX_COMPILER "g++"
	C_COMPILER "gcc"
)

# Define DEBUG macro if build type is Debug
target_compile_definitions(${TARGET_CROW} PRIVATE
  $<$<CONFIG:Debug>:DEBUG>
)

target_compile_definitions(${TARGET_CROW_LIB} PRIVATE
  $<$<CONFIG:Debug>:DEBUG>
)

# Compiler flags:
target_compile_options(${TARGET_CROW} PRIVATE
  -Wall
  -Wextra
  -pedantic
)

target_compile_options(${TARGET_CROW_LIB} PRIVATE
  -Wall
  -Wextra
  -pedantic
)

#add_compile_options(
#  -Wall
#  -Wextra
#  -pedantic
#)

# Set the flags for debugging
set(CMAKE_CXX_FLAGS_DEBUG
	"-g3 -gdwarf"
)

# Make it possible for tests to include sources from the project root.
if(DEFINED BUILD_CROW_TESTS)
target_include_directories(
	${TARGET_CROW_TESTS}
	PUBLIC ${CMAKE_SOURCE_DIR}/src
)
endif(DEFINED BUILD_CROW_TESTS)
