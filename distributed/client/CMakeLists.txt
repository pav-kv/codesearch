
add_executable (csclient main.cpp)
target_link_libraries (csclient distributed-proto)
target_link_libraries (csclient -lzmq -lprotobuf)

