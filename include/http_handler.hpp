#pragma once
#include "http_request.hpp"
#include "http_response.hpp"
void	handle_http_message(int fd, http_request message, Server& serv);
http_response	getRedirect(const std::string& newURL, int code = 301);
