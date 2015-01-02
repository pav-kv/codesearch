
add_executable (cscore main.cpp indexer.h searcher.h searcher.cpp server.h $<TARGET_OBJECTS:util-lister>)
target_link_libraries (cscore core-index-writer distributed-proto)
target_link_libraries (cscore -lzmq -lprotobuf -lboost_filesystem)

LINK_TARGET(cscore ${CMAKE_CURRENT_SOURCE_DIR}/cscore)
LINK_TARGET(cscore ${PROJECT_BINARY_DIR}/bin/cscore)

