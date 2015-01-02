#pragma once

#include "indexer.h"
#include "searcher.h"
#include <core/query.h>

#include <sstream>
#include <unistd.h>
#include <zmq.hpp>

#include <distributed/proto/protocol.pb.h>

namespace NCodesearch {

class TCoreServer {
public:
    TCoreServer(TIndexer& indexer, const char* proxyHost, uint16_t port = 6731)
        : Indexer(indexer)
        , ZmqContext(1)
        , ZmqSocket(ZmqContext, ZMQ_REP)
    {
        char addr[32];
        snprintf(addr, 32, "tcp://%s:%d", proxyHost, port);
        ZmqSocket.connect(addr);
    }

    void Start() {
        while (true) {
            zmq::message_t request;
            ZmqSocket.recv(&request);
            HandleRequest(request);
        }
    }

private:
    void HandleRequest(const zmq::message_t& request) {
        NProtocol::TSearchRequest req;
        std::string data((const char*)request.data(), request.size());
        req.ParseFromString(data);
        string queryStr = req.filter();
        string regexp = req.regexp();

        if (queryStr == "--")
            queryStr = regexp;
        std::cout << "Got request: " << queryStr << " ||| " << regexp << "\n";

        TQueryTreeNode* query = TQueryFactory::Parse(queryStr.c_str());
        if (!query) {
            cout << "Query compilation error!\n";
            return;
        }
        std::ostringstream output;
        TSearchConfig config;
        config.MaxFileSize = req.maxfilesize();
        std::shared_ptr<TSearcher> searcher = Indexer.GetSearcher();
        searcher->Search(query, config, output, regexp.c_str());
        TQueryFactory::Free(query); // TODO: Smart pointer with overrided delete.

        const string& result = output.str();
        zmq::message_t reply(result.size());
        memcpy((void *)reply.data(), result.data(), result.size());
        ZmqSocket.send(reply);
    }

private:
    TIndexer& Indexer;

    zmq::context_t ZmqContext;
    zmq::socket_t ZmqSocket;
};

} // NCodesearch

