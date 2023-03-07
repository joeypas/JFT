#include "Client.h"

using namespace boost::asio;
using namespace boost::asio::ip;

namespace jft {
Client::Client(io_context& io_context, const std::string& address) : socket_(io_context) {
    tcp::resolver resolver(io_context);
    tcp::resolver::query query(address, "5511");
    auto endpoints = resolver.resolve(query);
    connect(socket_, endpoints);
}

void Client::retFile(const std::string& filename) {
    std::string message = "RETR " + filename + "\r\n";
    socket_.write_some(buffer(message));

    streambuf buf;
    size_t code = read_until(socket_, buf, "\r\n");

    buf.commit(code);

    std::istream is(&buf);
    std::string res;
    is >> res;
    
    if (res.substr(0, 3) == "150") {
        const size_t rSize = sizeof("---EOF--");
        const size_t bufSize = 512000;
        streambuf fileBuf;
        boost::system::error_code ec;
        bool finished = false;

        
        read_until(socket_, fileBuf, "---EOF---");

        std::ofstream f("(1)" + filename, std::ios::binary);

        while (fileBuf.size() > bufSize){
            f.write(buffer_cast<const char*>(fileBuf.data()), bufSize);
            fileBuf.consume(bufSize);
        }
        f.write(buffer_cast<const char*>(fileBuf.data()), fileBuf.size() - rSize);

        
        f.close();

    } else {
        std::cout << res << std::endl;
    }
}

void Client::retDir(const std::string& dirname) {
    std::string message = "DIR " + dirname + "\r\n";
    socket_.write_some(buffer(message));

    streambuf buf;
    size_t code = read_until(socket_, buf, "\r");

    const char* res = buffer_cast<const char*>(buf.data());

    std::cout << res;

}
}

