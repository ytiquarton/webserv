#include "http_request.hpp"
#include "http_response.hpp"
#include <fcntl.h>

void throw_file_error(int error){
	if (error == ENOENT || error == ENOTDIR)
		throw (http_request::http_error("File not found", 404));
	if (error == EACCES || error == EPERM || error == EISDIR || error == ELOOP)
		throw (http_request::http_error("Permission denied", 403));
	throw (http_request::http_error("File operation failed", 500));
}

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

int	create_file(std::string content, Route route, std::string path)
{
	struct stat info;
	bool existed;
	int fd;
	size_t written;

	if (!is_allowed(route.getAllowMethods(), "POST"))
		throw (http_request::http_error("Method is not allowed", 405));
	existed = stat(path.c_str(), &info) == 0;
	fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW, 0644);
	if (fd == -1)
		throw_file_error(errno);
	while (written < content.size()){
		ssize_t count = write(fd, content.data() + written, content.size() - written);
		if (count < 0 && errno == EINTR)
			continue;
		if (count <= 0)
		{
			close(fd);
			throw_file_error(errno);

		}
		written += static_cast<std::size_t>(count);
	}
	if (close(fd) == -1)
		throw_file_error(errno);
	return (existed ? 200 : 201);
}

void	delete_file(Route route, std::string path)
{
	struct stat info;

	if (!is_allowed(route.getAllowMethods(), "DELETE"))
		throw (http_request::http_error("Method is not allowed", 405));
	if (stat(path.c_str(), &info) == -1)
		throw_file_error(errno);
	int fd = open(path.c_str(), O_WRONLY | O_TRUNC | O_NOFOLLOW);
	if (fd == -1)
		throw_file_error(errno);
	if (close(fd) == -1)
		throw_file_error(errno);
}

void	handle_http_message(int fd, http_request message, Server& serv)
{
	if (message.method == http_request::GET)
	{
		getURL(fd, message, serv);
		return;
	}
	Route route;
	std::string path_r;
	try{
		route = serv.config.getOneRoute(message.url);
		path_r = get_target_path(message.url, serv);
	}
	catch(const std::runtime_error&){
		throw (http_request::http_error("Unable to get route", 404));
	}
	catch(const http_request::http_error&){
		throw (http_request::http_error("Unable to root path", 404));
	}
	if (message.method == http_request::POST)
	{
		create_file(message.message_body, route, path_r);
		sendPOSTstatus(fd, 0);
	}
	else if (message.method == http_request::DELETE)
	{
		delete_file(route, path_r);
		sendDELETEstatus(fd, 0);
	}
	throw("Method not implemented yet!");
}