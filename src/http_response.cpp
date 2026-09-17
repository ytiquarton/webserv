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

void	sendHTMLPage(int fd, std::string requested_page)
{

	
	std::ostringstream ss;
	ss << get_HTML(requested_page);
	std::string	html_string(ss.str());
	ss.str("");

	ss << "HTTP/1.1 200 OK\r\n";
    ss << "Content-Type: text/html; charset=utf-8\r\n";
    ss << "Content-Length: " << html_string.size() << "\r\n";
    ss << "Connection: close\r\n";
    ss << "\r\n";
    ss << html_string;

	std::string content = ss.str();
	std::size_t	totalSent = 0;
	while( totalSent < content.size())
	{
		ssize_t sent = send(
			fd,
			content.data() + totalSent,
			content.size() - totalSent,
			0
		);
		if ( sent <= 0)
			break;
		totalSent += static_cast<std::size_t>(sent);
	}
}
