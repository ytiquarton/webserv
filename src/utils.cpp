#include "utils.hpp"

std::string return_path_routed(std::string path, const ServerConfig& server) {
    Route curr = server.getOneRoute(path);
    return curr.getRedirPath();
}
