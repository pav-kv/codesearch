all: metadata.pb.h metadata.pb.cc

metadata.pb.h metadata.pb.cc: metadata.proto
	protoc --cpp_out=. metadata.proto

clean:
	rm *.pb.h *.pb.cc

