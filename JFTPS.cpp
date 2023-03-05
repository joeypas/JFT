#include "JFTPS.h"
#include <boost/filesystem.hpp>

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::filesystem;

JFTPS::JFTPS(io_context& io_context, const tcp::endpoint& endpoint) : acceptor_(io_context, endpoint) {
    startAccept(io_context);
}

void JFTPS::startAccept(io_context& io_context) {
    auto socket = std::make_shared<tcp::socket>(io_context);
    acceptor_.async_accept(*socket, [this, socket, &io_context](const boost::system::error_code& error) {
        if (!error) {
            std::cout << "New Connection: " << socket->remote_endpoint() << std::endl;
            startRead(socket);
        }
        startAccept(io_context);
    });
}

void JFTPS::startRead(const std::shared_ptr<tcp::socket>& socket) {
    async_read_until(*socket, request_, "\r\n", [this, socket](const boost::system::error_code& error, std::size_t bytes) {
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

void JFTPS::processReq(const std::string& message, const std::shared_ptr<tcp::socket>& socket) {
    std::istringstream iss(message);
    std::string command;
    iss >> command;

    if (command == "RETR") {
        std::string filename;
        iss >> filename;

        std::ifstream file(filename, std::ios::binary);
        if (file.is_open()) {
            std::string res = "150\r\n";

            char buf[16000];
            while (!file.eof()) {
                file.read(buf, sizeof(buf));
                std::streamsize bytes = file.gcount();
                socket->write_some(buffer(buf, bytes));
            }
            file.close();

            res = "226\r\n";
            socket->write_some(buffer(res));
        }
        else {
            std::string res = "550 Not Found\r\n";
            socket->write_some(buffer(res));
        }
    }
    else if (command == "DIR") {
        std::string dir;
        iss >> dir;

        path p(dir);
        recursive_directory_iterator end_itr;

        for (recursive_directory_iterator itr(p); itr != end_itr; ++itr) {
            const path fp(*itr);
            const std::string message = fp.string() + "\n";
            socket->write_some(buffer(message));
        }
        std::string res = "226\r\n";
        socket->write_some(buffer(res));
    }
    else {
        std::string res = "502 Command not found";
        socket->write_some(buffer(res));
    }
    startRead(socket);
}