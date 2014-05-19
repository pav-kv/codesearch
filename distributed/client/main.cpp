#include "../proto/protocol.pb.h"
using namespace NCodesearch::NProtocol;

#include <iostream>
#include <sstream>
#include <string>
#include <zmq.hpp>

int main(int argc, char* argv[]) {
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REQ);

    std::string host = "localhost";
    if (argc >= 2)
        host = argv[1];
    uint16_t port = 6730;
    if (argc >= 3)
        port = atoi(argv[2]);

    std::cout << "Connecting to codesearch server ..." << std::endl;
    std::ostringstream addr;
    addr << "tcp://" << host << ":" << port;
    socket.connect(addr.str().c_str());
    std::cout << "Connected to " << addr.str() << "\n";

    TSearchRequest req;
    std::string queryStr, regexp;
    getline(std::cin, regexp);
    getline(std::cin, queryStr);
    req.set_regexp(regexp);
    req.set_filter(queryStr);
    std::string data;
    req.SerializeToString(&data);

    zmq::message_t request(data.size());
    memcpy((void *)request.data(), &data[0], data.size());
    std::cout << "Sending request ..." << std::endl;
    socket.send(request);
    std::cout << "Sent. Waiting for reply ...\n";

    zmq::message_t reply;
    socket.recv(&reply);
    std::cout << "Result:\n";
    std::cout.write((const char*)reply.data(), reply.size());
    std::cout << std::endl;

    return 0;
}

