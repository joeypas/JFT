#ifndef CLIENT_H
#define CLIENT_H

#include <boost/asio.hpp>
#include <string>
#include <fstream>
#include <iostream>

using namespace boost::asio;
using namespace boost::asio::ip;

namespace jft {
class Client {
public:
    Client(io_context& io_context, const std::string& address);

    void retFile(const std::string &filename);
    void retDir(const std::string &dirname);

private:
    tcp::socket socket_;
};
}

#endif