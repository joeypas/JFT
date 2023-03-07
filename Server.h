#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <memory>

using namespace boost::asio;
using namespace boost::asio::ip;

namespace jft {
class Server {
public:
    Server(io_context& io_context, const tcp::endpoint& endpoint);

private:
    void startAccept(io_context& io_context);

    void startRead(const std::shared_ptr<tcp::socket>& socket);

    void processReq(const std::string& message, const std::shared_ptr<tcp::socket>& socket);

    tcp::acceptor acceptor_;
    streambuf request_;
    io_context* ioio;
};
}


#endif