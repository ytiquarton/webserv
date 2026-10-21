#pragma once
#include <string>
#include "webserv.hpp"
#include "http_request.hpp"
void	start_CGI(std::string path, http_request request, int fd, Server& serv);
