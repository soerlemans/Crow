# Add source files.
target_sources(${TARGET_CROW_LIB} PRIVATE
	crow_parser.cpp
)

# Add subdirectories.
add_subdirectory(context)
