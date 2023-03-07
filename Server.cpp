#include "Server.h"
#include <boost/filesystem.hpp>
#include <fstream>

using namespace boost::asio;
using namespace boost::asio::ip;
namespace fs = boost::filesystem;

namespace jft {
Server::Server(io_context& io_context, const tcp::endpoint& endpoint) : acceptor_(io_context, endpoint) {
    ioio = &io_context;
    startAccept(io_context);
}

void Server::startAccept(io_context& io_context) {
    auto socket = std::make_shared<tcp::socket>(io_context);
    acceptor_.async_accept(*socket, [this, socket, &io_context](const boost::system::error_code error) {
        if (!error) {
            std::cout << "New Connection: " << socket->remote_endpoint() << std::endl;
            startRead(socket);
        }
        startAccept(io_context);
    });
}

void Server::startRead(const std::shared_ptr<tcp::socket>& socket) {
    async_read_until(*socket, request_, "\r\n", [this, socket](const boost::system::error_code error, std::size_t bytes) {
        if (!error) {
            std::string message;
            std::istream stream(&request_);
            std::getline(stream, message);
            processReq(message, socket);
        } else {
            std::cout << "ERR: " << error.message() << std::endl;
        }
    });
}

void Server::processReq(const std::string& message, const std::shared_ptr<tcp::socket>& socket) {
    std::istringstream iss(message);
    std::string command;
    iss >> command;

    if (command == "RETR") {
        std::string filename;
        iss >> filename;
        std::stringstream convert;

        uintmax_t fsize = fs::file_size(filename);
        convert << fsize;
        std::string size = convert.str() + "\r\n";

        std::ifstream file(filename, std::ios::binary);
        if (file.is_open()) {
            std::string res = "150\r\n";
            socket->write_some(buffer(res));

            socket->write_some(buffer(size));

            // 8MB BUFFERS!!!
            char buf[8000000];
            while (!file.eof()) {
                file.read(buf, sizeof(buf));
                std::streamsize bytes = file.gcount();
                socket->write_some(buffer(buf, bytes));
            }
            file.close();

            res = "---EOF---";
            socket->write_some(buffer(res));
            ioio->stop();
            delete ioio;
        }
        else {
            std::string res = "550 Not Found\r\n";
            socket->write_some(buffer(res));
        }
    }
    else if (command == "DIR") {
        std::string dir;
        iss >> dir;

        fs::path p(dir);

        for (fs::directory_entry& x : fs::directory_iterator(p)) {
            auto fp(x.path());
            const std::string message = fp.string() + "\n";
            socket->write_some(buffer(message));
        }
        std::string res = "\r";
        socket->write_some(buffer(res));
    }
    else {
        std::string res = "502 Command not found";
        socket->write_some(buffer(res));
    }
    startRead(socket);
}
}

