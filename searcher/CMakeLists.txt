
add_executable (searcher main.cpp) # TODO: add query.h
target_link_libraries (searcher core-search-searcher)
target_link_libraries (searcher -lboost_program_options)

LINK_TARGET(searcher ${CMAKE_CURRENT_SOURCE_DIR}/searcher)
LINK_TARGET(searcher ${PROJECT_BINARY_DIR}/bin/searcher)

