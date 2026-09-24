#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <sys/socket.h>


std::string get_HTML(std::string requested_page)
{
	std::cout<< "requested url: " << requested_page << std::endl;
	std::ifstream HTML_File("HTML_Example.html");
	if (!HTML_File)
		throw (std::runtime_error("Cannot open file!"));
		
	std::ostringstream ss;
	ss << HTML_File.rdbuf();
	return (ss.str());
}

void	sendHTML(std::string str, int fd)
{
	std::size_t	totalSent = 0;
	while( totalSent < str.size())
	{
		ssize_t sent = send(
			fd,
			str.data() + totalSent,
			str.size() - totalSent,
			0
		);
		if ( sent <= 0)
			break;
		totalSent += static_cast<std::size_t>(sent);
	}
}

void	sendHTMLPage(int fd, std::string requested_page)
{
	std::string	html_string(get_HTML(requested_page));

	std::ostringstream ss;
	ss << "HTTP/1.1 200 OK\r\n";
    ss << "Content-Type: text/html; charset=utf-8\r\n";
    ss << "Content-Length: " << html_string.size() << "\r\n";
    ss << "Connection: close\r\n";
    ss << "\r\n";
    ss << html_string;

	std::string content = ss.str();
	sendHTML(content, fd);
}


void	sendPOSTstatus(int fd, int status)
{
	if (status == 0)
		sendHTML("HTTP/1.1 201 Created\r\nContent-Length: 0\r\n\r\n", fd);
	else
		throw(std::runtime_error("status not handled!"));
}
void	sendDELETEstatus(int fd, int status)
{
	if (status == 0)
		sendHTML("HTTP/1.1 204 No Content\r\n\r\n", fd);
	else
		throw(std::runtime_error("status not handled!"));
}