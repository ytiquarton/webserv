#pragma once
#include <string>
#include "webserv.hpp"
void	sendHTMLPage(int fd, std::string requested_page, Server& serv);
void	sendDELETEstatus(int fd, int status);
void	sendPOSTstatus(int fd, int status);