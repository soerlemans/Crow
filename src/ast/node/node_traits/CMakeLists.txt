# Add source files
target_sources(${PROJECT_NAME} PRIVATE
	body.cpp
	condition.cpp
	decl_expr.cpp
	expr.cpp
	identifier.cpp
	init_expr.cpp
	node_position.cpp
	params.cpp
	type_annotation.cpp
	then.cpp
	alt.cpp
	unary_operator.cpp
	binary_operator.cpp
)

# Add subdirectories
add_subdirectory(typing)
