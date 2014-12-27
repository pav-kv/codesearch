
add_library (searcherlib OBJECT searcher.h searcher.cpp)
add_executable (searcher main.cpp query.h $<TARGET_OBJECTS:searcherlib>)

# TODO: make a macro
add_custom_command (
    TARGET searcher POST_BUILD
    COMMAND ln --symbolic --force ${CMAKE_CURRENT_BINARY_DIR}/searcher ${PROJECT_BINARY_DIR}/bin/
)

