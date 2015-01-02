
add_executable (cscore main.cpp indexer.h searcher.h searcher.cpp server.h $<TARGET_OBJECTS:util-lister>)
target_link_libraries (cscore core-index-writer distributed-proto)
target_link_libraries (cscore -lzmq -lprotobuf -lboost_filesystem)

add_executable (csproxy proxy.cpp)
target_link_libraries (csproxy -lzmq)

LINK_TARGET(cscore ${CMAKE_CURRENT_SOURCE_DIR}/cscore)
LINK_TARGET(cscore ${PROJECT_BINARY_DIR}/bin/cscore)

LINK_TARGET(csproxy ${CMAKE_CURRENT_SOURCE_DIR}/csproxy)
LINK_TARGET(csproxy ${PROJECT_BINARY_DIR}/bin/csproxy)

