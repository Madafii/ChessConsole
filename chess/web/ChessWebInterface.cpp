#include "ChessWebInterface.h"
#include "httplib.h"

ChessWebInterface::ChessWebInterface() {
    httplib::Server svr;

    auto ret = svr.set_mount_point("/", "./www");
    if (!ret) {
        std::cerr << "the specified mount point does not exist" << std::endl;
    }

    std::cout << "starting server" << std::endl;
    if (!svr.listen("localhost", 8080)) {
        std::cerr << "Error: not listening to server" << std::endl;
    }
}
