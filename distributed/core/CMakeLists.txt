
add_executable (csproxy proxy.cpp)
target_link_libraries (csproxy -lzmq)

add_executable (cscore main.cpp indexer.h searcher.h searcher.cpp server.h $<TARGET_OBJECTS:lister>)
target_link_libraries (cscore writer distributed-proto)
target_link_libraries (cscore -lzmq -lprotobuf -lboost_filesystem)

# TODO: make a macro
add_custom_command (
    TARGET csproxy POST_BUILD
    COMMAND ln --symbolic --force ${CMAKE_CURRENT_BINARY_DIR}/csproxy ${PROJECT_BINARY_DIR}/bin/
)
add_custom_command (
    TARGET cscore POST_BUILD
    COMMAND ln --symbolic --force ${CMAKE_CURRENT_BINARY_DIR}/cscore ${PROJECT_BINARY_DIR}/bin/
)

