#pragma once

#include <string>
#include <iostream>
#include <ostream>
#include <sys/stat.h>
#include <unistd.h>
#include "parse_config.hpp"

class ServerConfig;
class Route;
std::string return_path_routed(std::string path, ServerConfig& server);


bool existDir(const std::string& path);


std::string return_path_routed(std::string path, ServerConfig& server);
bool isDirectory(const std::string& path);
std::string appendRoute(Route target_route, std::string path);
std::string get_http_nb_phrase(int code);
std::string read_file(std::string path);
