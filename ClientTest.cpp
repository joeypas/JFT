#include "Client.h"

int main() {
    try{
        io_context io_context;
        jft::Client client(io_context, "localhost");
        client.retDir(".");
        client.retFile("Xcode_14.2.xip");
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
