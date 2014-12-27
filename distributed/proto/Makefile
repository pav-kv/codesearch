all: metadata.pb.h metadata.pb.cc protocol.pb.h protocol.pb.cc

metadata.pb.h metadata.pb.cc: metadata.proto
	protoc --cpp_out=. metadata.proto

protocol.pb.h protocol.pb.cc: protocol.proto
	protoc --cpp_out=. protocol.proto

clean:
	rm *.pb.h *.pb.cc

