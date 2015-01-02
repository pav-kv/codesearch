
add_executable (csproxy proxy.cpp)
target_link_libraries (csproxy -lzmq)

LINK_TARGET(csproxy ${CMAKE_CURRENT_SOURCE_DIR}/csproxy)
LINK_TARGET(csproxy ${PROJECT_BINARY_DIR}/bin/csproxy)

