#include "http_message.hpp"
#include "http_response.hpp"

void	create_file(std::string path, std::string content)
{

}

void	delete_file(std::string path)
{

}

void	handle_http_message(int fd, http_message message)
{
	if (message.method == http_message::GET)
	{
		sendHTMLPage(fd, message.url);
		return;
	}
	else if (message.method == http_message::POST)
	{
		create_file(message.url, message.message_body);
		sendPOSTstatus(fd, 0);
	}
	else if (message.method == http_message::DELETE)
	{
		delete_file(message.url);
		sendDELETEstatus(fd, 0);
	}
	throw("Method not implemented yet!");
}