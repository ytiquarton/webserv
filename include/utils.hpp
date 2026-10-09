#include <string>
#include <iostream>
#include <ostream>
#include "parse_config.hpp"

std::string return_path_routed(std::string path, ServerConfig& server);
bool isDirectory(const std::string& path);
std::string appendRoute(Route target_route, std::string path);
