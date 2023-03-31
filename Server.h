#ifndef SERVER_H
#define SERVER_H

#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/signal_set.hpp>
#include <asio/write.hpp>
#include <asio/streambuf.hpp>
#include <asio/read_until.hpp>
#include <iostream>
#include <string>
#include <memory>

using namespace asio;
using namespace asio::ip;
using asio::awaitable;

namespace jft {
class Server {
public:
    Server(io_context& io_context, const tcp::endpoint& endpoint);

private:
    awaitable<void> startAccept(io_context& io_context);

    awaitable<void> startRead(tcp::socket socket);

    awaitable<void> processReq(const std::string message, tcp::socket socket);

    tcp::endpoint _endpoint;
    streambuf request_;
    io_context* ioio;
};
}


#endif