
add_library (lister OBJECT lister.h lister.cpp)

add_executable (indexer main.cpp $<TARGET_OBJECTS:lister>)
target_link_libraries (indexer writer)

# TODO: make a macro
add_custom_command (
    TARGET indexer POST_BUILD
    COMMAND ln --symbolic --force ${CMAKE_CURRENT_BINARY_DIR}/indexer ${PROJECT_BINARY_DIR}/bin/
)

