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
	try
	{
		if (message.method == http_request::GET)
		{
			getURL(fd, message, serv);
			return;
		}
		else if (message.method == http_request::POST)
		{
			create_file(message.url, message.message_body);
			sendPOSTstatus(fd, 0, serv);
		}
		else if (message.method == http_request::DELETE)
		{
			delete_file(message.url);
			sendDELETEstatus(fd, 0, serv);
		}
		else
			throw(std::runtime_error("Method not implemented yet!"));
	}
	catch(const http_request::http_error& e)
	{
		safe_send_Error(fd, e, serv);
	}
	catch (std::exception& e)
	{
		if (std::string(e.what()) == "Child Error!")
			throw(std::runtime_error("Child Error!"));
		http_request::http_error error(e.what(), 500);
		safe_send_Error(fd, error, serv);
	}
	
}
