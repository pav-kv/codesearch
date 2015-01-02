
add_executable (indexer main.cpp $<TARGET_OBJECTS:util-lister>)
target_link_libraries (indexer core-index-writer)
target_link_libraries (indexer -lboost_program_options)

LINK_TARGET(indexer ${CMAKE_CURRENT_SOURCE_DIR}/indexer)
LINK_TARGET(indexer ${PROJECT_BINARY_DIR}/bin/indexer)

