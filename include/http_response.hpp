#pragma once
#include <string>
#include "webserv.hpp"
void	sendHTMLPage(int fd, std::string requested_page, Server& serv, int  http_nb = 200, int depth=0);
void	sendDELETEstatus(int fd, int status);
void	sendPOSTstatus(int fd, int status);
void	sendError(int fd, int code, Server &serv);