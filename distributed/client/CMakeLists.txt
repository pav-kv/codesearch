
add_executable (csclient main.cpp)
target_link_libraries (csclient distributed-proto)
target_link_libraries (csclient -lzmq -lprotobuf)

LINK_TARGET(csclient ${CMAKE_CURRENT_SOURCE_DIR}/csclient)
LINK_TARGET(csclient ${PROJECT_BINARY_DIR}/bin/csclient)

