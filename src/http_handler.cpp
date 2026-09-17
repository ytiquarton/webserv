#include "http_message.hpp"
#include "http_response.hpp"

void	handle_http_message(int fd, http_message message)
{
	if (message.method != http_message::GET)
		throw("Method not implemented yet!");
	sendHTMLPage(fd, message.url);
}