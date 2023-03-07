#include "Server.h"

int main() {
    try {
        io_context io_context;
        tcp::endpoint endpoint(tcp::v4(), 5511);
        jft::Server server(io_context, endpoint);
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    
    return 0;
}