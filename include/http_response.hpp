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

void	getURL(int fd, std::string requested_page, Server& serv);
void	sendDELETEstatus(int fd, int status);
void	sendPOSTstatus(int fd, int status);
void	sendError(int fd, int code, Server &serv);