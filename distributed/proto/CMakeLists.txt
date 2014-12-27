
COMPILE_PROTOBUF(metadata)
COMPILE_PROTOBUF(protocol)

add_library (distributed-proto metadata.pb.h metadata.pb.cc protocol.pb.h protocol.pb.cc)

