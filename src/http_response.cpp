#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <sys/socket.h>
#include "utils.hpp"
#include "webserv.hpp"
#include "parse_config.hpp"
#include "http_message.hpp"

std::string get_http_nb_phrase(int code)
{
	switch (code) {
		case 200:	return "OK";
		case 201:	return "Created";
		case 204:	return "No Content";
		case 301:	return "Moved Permanently";
		case 302:	return "Found";
		case 400:	return "Bad Request";
		case 403:	return "Forbidden";
		case 404:	return "Not Found";
		case 405:	return "Method Not Allowed";
		case 408:	return "Request Timeout";
		case 411:	return "Length Required";
		case 413:	return "Content Too Large";
		case 414:	return "URI Too Long";
		case 500:	return "Internal Server Error";
		case 501:	return "Not Implemented";
		case 502:	return "Bad Gateway";
		case 504:	return "Gateway Timeout";
		case 505:	return "HTTP Version Not Supported";
		default:	return "Unknown";
	}
}

std::string read_file(std::string path)
{
	std::ifstream file(path.c_str());
	if (!file)
		throw (http_message::http_error("File not found!", 404));

	std::ostringstream ss;
	ss << file.rdbuf();
	return (ss.str());
}

std::string get_HTML(std::string requested_page, Server& serv)
{
	std::cout<< "requested url: " << requested_page << std::endl;

    Route rte = serv.config.getOneRoute(requested_page);
	if (rte.getAllowMethods().find("GET") == std::string::npos)
		throw (http_message::http_error("Method not allowed!", 405));

	std::string file_to_serve;
	if (isDirectory(requested_page))
		file_to_serve = appendRoute( rte, rte.getIndex());
	else
	{
		file_to_serve = return_path_routed(requested_page, serv.config);
	}
	std::cout<< "File to serve: " << file_to_serve << std::endl;
	return (read_file(file_to_serve));
}

void	send_to_fd(std::string str, int fd)
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

void	send_HTTP_Message(int fd, std::string body, int http_nb)
{
	std::ostringstream ss;
		ss << "HTTP/1.1 "<< http_nb <<" " <<  get_http_nb_phrase(http_nb) << "\r\n";
		ss << "Content-Type: text/html; charset=utf-8\r\n";
		ss << "Content-Length: " << body.size() << "\r\n";
		ss << "Connection: close\r\n";
		ss << "\r\n";
		ss << body;

		send_to_fd(ss.str(), fd);
}

void	safe_send_Error(int fd, const http_message::http_error& error, Server &serv)
{
	int index = 0;
	int code = error.get_nb();
	while (index < 10)
	{
		try{
			send_HTTP_Message(fd, read_file(serv.config.getErrorPagePath(code)), code);
			return;
		}
		catch(const http_message::http_error& e)
		{
			code = e.get_nb();
		}
		index ++;
	}
	send_HTTP_Message(fd, "", code);
}

void	sendHTMLPage(int fd, std::string requested_page, Server& serv, int depth=0)
{
	std::ostringstream ss;
	try
	{
		send_HTTP_Message(fd, get_HTML(requested_page, serv), 200);
	}
	catch(const http_message::http_error& e)
	{
		safe_send_Error(fd, e, serv);
	}
	catch (std::exception& e)
	{
		throw(e);
	}
}



void	sendPOSTstatus(int fd, int status)
{
	if (status == 0)
		send_HTTP_Message(fd, "", 201);
	else
		throw(std::runtime_error("status not handled!"));
}
void	sendDELETEstatus(int fd, int status)
{
	if (status == 0)
		send_HTTP_Message(fd, "", 204);
	else
		throw(std::runtime_error("status not handled!"));
}