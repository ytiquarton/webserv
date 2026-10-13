#include "http_request.hpp"
#include "http_response.hpp"

void	create_file(std::string path, std::string content)
{
	(void)path;
	(void)content;
}

void	delete_file(std::string path)
{
	(void)path;
}

void	handle_http_message(int fd, http_request message, Server& serv)
{
	if (message.method == http_request::GET)
	{
		getURL(fd, message.url, serv);
		return;
	}
	else if (message.method == http_request::POST)
	{
		create_file(message.url, message.message_body);
		sendPOSTstatus(fd, 0);
	}
	else if (message.method == http_request::DELETE)
	{
		delete_file(message.url);
		sendDELETEstatus(fd, 0);
	}
	throw("Method not implemented yet!");
}