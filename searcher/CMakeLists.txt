
add_library (searcherlib OBJECT searcher.h searcher.cpp)
add_executable (searcher main.cpp query.h $<TARGET_OBJECTS:searcherlib>)
target_link_libraries (searcher -lboost_program_options)

LINK_TARGET(searcher ${CMAKE_CURRENT_SOURCE_DIR}/searcher)
LINK_TARGET(searcher ${PROJECT_BINARY_DIR}/bin/searcher)

