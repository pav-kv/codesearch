#include <zmq.hpp>

int main(int argc, char *argv[]) {
    zmq::context_t context(1);

    zmq::socket_t frontend(context, ZMQ_ROUTER);
    frontend.bind("tcp://*:6730");

    zmq::socket_t backend(context, ZMQ_DEALER);
    backend.bind("tcp://*:6731");

    zmq_device(ZMQ_QUEUE, frontend, backend);

    return 0;
}

