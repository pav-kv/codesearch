
add_executable (csproxy proxy.cpp)
target_link_libraries (csproxy -lzmq)

add_executable (cscore main.cpp indexer.h searcher.h searcher.cpp server.h $<TARGET_OBJECTS:lister>)
target_link_libraries (cscore writer distributed-proto)
target_link_libraries (cscore -lzmq -lprotobuf -lboost_filesystem)

