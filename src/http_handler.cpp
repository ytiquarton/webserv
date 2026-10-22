#include "http_request.hpp"
#include "http_response.hpp"
#include <fcntl.h>

std::string get_target_path(std::string url, Server serv){
	std::string path = return_path_routed(url, serv.config);

	if (path.empty())
		throw(http_request::http_error("Unable to root path", 404));
	return (path);
}

bool 	is_allowed(std::string allowed, std::string method){
	size_t pos = allowed.find(method);

	if (pos == std::string::npos)
		return false;
	if (pos == 0 && (allowed[pos + method.length()] == ' ' || allowed[pos + method.length()] == '\t'))
		return true;
	else if (allowed[pos - 1] == ' ' || allowed[pos - 1] == '\t'
		&& (allowed[pos + method.length()] == ' ' || allowed[pos + method.length()] == '\t'))
		return true;
	return false;

}

int	create_file(std::string url, std::string content, Server& serv)
{
	Route route = serv.config.getOneRoute(url);
	std::string path = get_target_path(url, serv);
	struct stat info;
	bool existed;
	int fd;
	size_t written;

	if (!is_allowed(route.getAllowMethods(), "POST"))
		throw (http_request::http_error("Method is not allowed", 405));
	existed = stat(path.c_str(), &info) == 0;
	fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW, 0644);
	if (fd == -1)
		throw (http_request::http_error("Could not open file to POST", errno));
	while (written < content.size()){
		ssize_t count = write(fd, content.data() + written, content.size() - written);
		if (count < 0 && errno == EINTR)
			continue;
		if (count <= 0)
		{
			close(fd);
			throw (http_request::http_error("Can't write in file", errno));

		}
		written += static_cast<std::size_t>(count);
	}
	if (close(fd) == -1)
		throw(http_request::http_error("Can't close file", errno));
	return (existed ? 200 : 201);
	
}

void	delete_file(std::string url, Server& serv)
{
	Route route = serv.config.getOneRoute(url);
	std::string path = get_target_path(url, serv);
	struct stat info;

	if (!is_allowed(route.getAllowMethods(), "DELETE"))
		throw (http_request::http_error("Method is not allowed", 405));
	

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
		try{
			serv.config.getOneRoute(message.url);
			get_target_path(message.url, serv);
		}
		catch(const std::runtime_error&){
			throw (http_request::http_error("Unable to get route", 404));
		}
		catch(const http_request::http_error&){
			throw (http_request::http_error("Unable to root path", 404));
		}
		if (message.method == http_request::POST)
		{
			create_file(message.url, message.message_body, serv);
			sendPOSTstatus(fd, 0, serv);
		}
		else if (message.method == http_request::DELETE)
		{
			delete_file(message.url, serv);
			sendDELETEstatus(fd, 0, serv);
		}
		else
			throw("Method not implemented yet!");
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
