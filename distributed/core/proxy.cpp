#include <iostream>
#include <zmq.hpp>

int main(int argc, char *argv[]) {
    // TODO: Configurable ports.

    zmq::context_t context(1);

    zmq::socket_t frontend(context, ZMQ_ROUTER);
    zmq::socket_t backend(context, ZMQ_DEALER);
    frontend.bind("tcp://*:6730");
    backend.bind("tcp://*:6731");

    std::cerr << "Codesearch proxy started.\n";
    zmq_device(ZMQ_QUEUE, frontend, backend);

    return 0;
}

