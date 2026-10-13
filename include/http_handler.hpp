#pragma once
#include "http_request.hpp"

void	handle_http_message(int fd, http_request message, Server& serv);
