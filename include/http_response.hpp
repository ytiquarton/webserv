#pragma once
#include <string>
#include "webserv.hpp"
#include "string_utils.hpp"

class http_response
{
	public:
		int									code;
		std::map<std::string, std::string>	headers;
		std::string							body;
		http_response(): code(), headers(), body()
		{
			setLength();
		}
		http_response(int _code, std::string _body): code(_code), headers(), body(_body)
		{
			setLength();
		}
		http_response &setLength()
		{
			headers["Content-Length"] = int_to_string(body.size());
			return (*this);
		}
};
void	getURL(int fd, http_request mesage, Server& serv);
void	sendDELETEstatus(int fd, int status, Server& serv);
void	sendPOSTstatus(int fd, int status, Server& serv);
void	sendError(int fd, int code, Server &serv);
void	safe_send_Error(int fd, const http_request::http_error& error, Server &serv, bool connection_close = false);
void	send_HTTP_Message(int fd, http_response response, Server& serv);