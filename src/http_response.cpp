#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <sys/socket.h>
#include <dirent.h>
#include "utils.hpp"
#include "webserv.hpp"
#include "parse_config.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "string_utils.hpp"
#include "cgi_handler.hpp"
#include "http_handler.hpp"

std::string	getDirHTML(std::string const path)
{
	std::string output("<html>\n<body>\n<h1>Index:</h1>\n<ul>\n");
	DIR* dir = opendir(path.c_str());
	if (!dir)
		throw(http_request::http_error("Error opening dir!", 500));

	struct dirent* entry;
	while ((entry = readdir(dir)) !=NULL)
	{
		std::string name = entry->d_name;
		if (name == "." || name == "..")
			continue;
		
		std::string fullPath = path + "/" + name;
		struct stat info;
		if (stat(fullPath.c_str(), &info) == 0 && (S_ISDIR(info.st_mode) || S_ISREG(info.st_mode)))
			output = output + "	<li><a href=\"" + name + "\">"+name+"</a></li>\n";
	}
	closedir(dir);
	output = output + "</ul>\n</body>\n</html>";
	return (output);
}


http_response getResponseGetURL(std::string requested_page, Server& serv)
{
	http_response output(200, "");
	std::cout<< "requested url: " << requested_page << std::endl;

    Route rte = serv.config.getOneRoute(requested_page);


	std::string file_to_serve = return_path_routed(requested_page, serv.config);
	if (isDirectory(file_to_serve))
	{
		if (!endsWith(requested_page, "/"))
			return (getRedirect(requested_page + "/", 301));
		std::cout << file_to_serve << " Is directory!\n";
		if ( !rte.getAutoindex())
		{
			std::cout<<"no auto index!\n";
			file_to_serve = appendRoute( rte, rte.getIndex());
		}
		else
		{
			output.body = getDirHTML(file_to_serve);
			output.headers["Content-Type"] = "text/html; charset=utf-8";
			return (output.setLength());
		}
	}
	
	std::cout<< "HTML to serve: " << file_to_serve << std::endl;
	output.body = read_file(file_to_serve);
	if (endsWith(file_to_serve, ".html"))
		output.headers["Content-Type"] = "text/html; charset=utf-8";
	return (output.setLength());
}

void	send_to_fd(std::string str, int fd, Server& serv)
{
	int	dup_fd = dup(fd);
	if (dup_fd == -1)
		throw(http_request::http_error("send error!", 500));
	connection&	cgi_connection = serv.epoll.connections[dup_fd];
	cgi_connection.type = connection::WriteFD;
	cgi_connection.body = str;
	cgi_connection.fd = dup_fd;
	try
	{
		add_connection_to_epoll(dup_fd, &cgi_connection, serv.epoll.myepoll, EPOLLOUT);
	}
	catch(...)
	{
		serv.epoll.connections.erase(fd);
		throw(http_request::http_error("send error!", 500));
	}
}

void	send_HTTP_Message(int fd, http_response response, Server& serv)
{
	std::ostringstream ss;
	ss << "HTTP/1.1 "<< response.code <<" " <<  get_http_nb_phrase(response.code) << "\r\n";
	for(std::map<std::string, std::string>::const_iterator it = response.headers.begin(); it!= response.headers.end(); ++it)
	{
		ss << it->first << ": " << it->second << "\r\n";
	}
	ss << "\r\n";
	ss << response.body;

	//std::cout << "Sent response:\n" << ss.str() << std::endl;
	send_to_fd(ss.str(), fd, serv);
}

void	safe_send_Error(int fd, const http_request::http_error& error, Server &serv, bool connection_close)
{
	int index = 0;
	int code = error.get_nb();
	http_response response(code, "");
	if (connection_close)
	{
		response.headers["Connection"] = "close";
	}
	while (index < 10)
	{
		try{
			std::string error_page_path = serv.config.getErrorPagePath(code);
			if (error_page_path != "")
				response.body = read_file(serv.config.getErrorPagePath(code));
			response.setLength();
			send_HTTP_Message(fd, response, serv);
			return;
		}
		catch(const http_request::http_error& e)
		{
			code = e.get_nb();
		}
		index ++;
	}
	send_HTTP_Message(fd, response, serv);
}

void	getURL(int fd, http_request message, Server& serv)
{
	if (endsWith(message.url, ".py") && serv.config.getOneRoute(message.url).getCgi())
	{
		std::cout << "CGI!!!!\n";
		start_CGI(return_path_routed(message.url, serv.config), message, fd, serv);
	}
	else
	{
		std::cout << "Boring usual request... \n";
		send_HTTP_Message(fd, getResponseGetURL(message.url, serv), serv);
	}
}



void	sendPOSTstatus(int fd, int status, Server& serv)
{
	if (status == 0)
		send_HTTP_Message(fd, http_response(201, ""), serv);
	else
		throw(std::runtime_error("status not handled!"));
}
void	sendDELETEstatus(int fd, int status, Server& serv)
{
	if (status == 0)
		send_HTTP_Message(fd, http_response(204, ""), serv);
	else
		throw(std::runtime_error("status not handled!"));
}