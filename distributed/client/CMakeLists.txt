
add_executable (csclient main.cpp)
target_link_libraries (csclient distributed-proto)
target_link_libraries (csclient -lzmq -lprotobuf)

# TODO: make a macro
add_custom_command (
    TARGET csclient POST_BUILD
    COMMAND ln --symbolic --force ${CMAKE_CURRENT_BINARY_DIR}/csclient ${PROJECT_BINARY_DIR}/bin/
)

