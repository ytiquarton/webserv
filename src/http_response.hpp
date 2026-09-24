#pragma once
#include <string>
void	sendHTMLPage(int fd, std::string requested_page);
void	sendDELETEstatus(int fd, int status);
void	sendPOSTstatus(int fd, int status);