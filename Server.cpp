#include "Server.h"
#include <boost/filesystem.hpp>
#include <fstream>


using asio::ip::tcp;
using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;
namespace this_coro = asio::this_coro;
namespace fs = boost::filesystem;

#if defined(ASIO_ENABLE_HANDLER_TRACKING)
# define use_awaitable \
  asio::use_awaitable_t(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif 

namespace jft {
Server::Server(io_context& io_context, const tcp::endpoint& endpoint) : _endpoint(endpoint) {
    ioio = &io_context;
    co_spawn(*ioio, startAccept(*ioio), detached);
}

awaitable<void> Server::startAccept(io_context& io_context) {
    //auto socket = std::make_shared<tcp::socket>(io_context);
    /*
    acceptor_.async_accept(*socket, [this, socket, &io_context](const boost::system::error_code error) {
        if (!error) {
            std::cout << "New Connection: " << socket->remote_endpoint() << std::endl;
            startRead(socket);
        }
        startAccept(io_context);
    });
    */
   auto executer = co_await this_coro::executor;
   tcp::acceptor acceptor(executer, _endpoint);
   for (;;) {
        tcp::socket socket = co_await acceptor.async_accept(use_awaitable);

        std::cout << "New Connection: " << socket.remote_endpoint() << std::endl;
        co_spawn(executer, startRead(std::move(socket)), detached);
   }

}

awaitable<void> Server::startRead(tcp::socket socket) {
    /*
    async_read_until(socket, request_, "\r\n", [&](const boost::system::error_code error, std::size_t bytes) {
        if (!error) {
            std::string message;
            std::istream stream(&request_);
            std::getline(stream, message);
            processReq(message, socket);
        } else {
            std::cout << "ERR: " << error.message() << std::endl;
        }
    });
    */

   try {
    char req[1024];
    for (;;) {
        size_t n = co_await socket.async_read_some(asio::buffer(req), use_awaitable);
        std::string message = req;

        co_await processReq(message, std::move(socket));
    }
   } catch (std::exception &e) {
    std::cout << "ERR: " << e.what() << std::endl;
   }
}

awaitable<void> Server::processReq(const std::string message, tcp::socket socket) {
    std::istringstream iss(message);
    std::string command;
    iss >> command;

    std::cout << command << std::endl;

    if (command == "RETR") {
        std::string filename;
        iss >> filename;
        std::cout << filename << std::endl;
        std::stringstream convert;

        uintmax_t fsize = fs::file_size(filename);
        convert << fsize;
        std::string size = convert.str() + "\r\n";

        std::ifstream file(filename, std::ios::binary);
        if (file.is_open()) {
            std::string res = "150\r\n";
            socket.async_write_some(buffer(res), use_awaitable);
            std::cout << "im here" << std::endl;

            socket.async_write_some(buffer(size), use_awaitable);

            // 8MB BUFFERS!!!
            char buf[8000];
            while (!file.eof()) {
                file.read(buf, sizeof(buf));
                std::streamsize bytes = file.gcount();
                socket.async_write_some(buffer(buf, bytes), use_awaitable);
            }
            file.close();

            res = "---EOF---";
            socket.async_write_some(buffer(res), use_awaitable);
        }
        else {
            std::string res = "550 Not Found\r\n";
            socket.async_write_some(buffer(res), use_awaitable);
        }
    }
    else if (command == "DIR") {
        std::string dir;
        iss >> dir;

        fs::path p(dir);

        for (fs::directory_entry& x : fs::directory_iterator(p)) {
            auto fp(x.path());
            const std::string message = fp.string() + "\n";
            socket.write_some(buffer(message));
        }
        std::string res = "\r";
        socket.write_some(buffer(res));
    }
    else {
        std::string res = "502 Command not found";
        socket.write_some(buffer(res));
    }

    //startRead(std::move(socket)), detached);
}
}

