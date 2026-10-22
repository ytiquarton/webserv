#include "http_request.hpp"
#include "http_response.hpp"
#include <fcntl.h>
#include "cgi_handler.hpp"
#include "http_response.hpp"


http_response	getRedirect(const std::string& newURL, int code = 301)
{
	http_response	output(code, "");
	output.headers["Location"] = newURL;
	return (output.setLength());
}

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

bool 	is_allowed(std::string allowed, std::string method)
{
	size_t pos = allowed.find(method);

	if (pos == std::string::npos)
		return false;
	if (pos == 0 && (allowed[pos + method.length()] == ' ' || allowed[pos + method.length()] == '\t'))
		return true;
	else if ((allowed[pos - 1] == ' ' || allowed[pos - 1] == '\t')
		&& (allowed[pos + method.length()] == ' ' || allowed[pos + method.length()] == '\t' || allowed[pos + method.length()] == ';'))
		return true;
	return false;

}

int	create_file(std::string content, std::string path)
{
	struct stat info;
	bool existed;
	int fd;
	size_t written = 0;

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

void	delete_file(std::string path)
{
	struct stat info;

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
	Route	rte;
	std::string path_r;
	try
	{
		try{
			rte = serv.config.getOneRoute(message.url);
			path_r = get_target_path(message.url, serv);
		}
		catch(const std::runtime_error&){
			throw (http_request::http_error("Unable to get route", 404));
		}
		catch(const http_request::http_error&){
			throw (http_request::http_error("Unable to root path", 404));
		}
		if (!is_allowed(rte.getAllowMethods(),  http_request::r_methods_map.find(message.method)->second))
			throw (http_request::http_error("Method is not allowed", 405));
		
		std::pair<int, std::string> redir = rte.getRedir();
		if (redir.first != 0)
		{
			send_HTTP_Message(fd, getRedirect(redir.second, redir.first), serv);
			return;
		}
		if (message.method == http_request::GET)
		{
			getURL(fd, message, serv);
		}
		else if (message.method == http_request::POST)
		{
			if (endsWith(message.url, ".py") && rte.getCgi())
				start_CGI(return_path_routed(message.url, serv.config), message, fd, serv);
			else{
				create_file(message.message_body, path_r);
				sendPOSTstatus(fd, 0, serv);
			}
		}
		else if (message.method == http_request::DELETE)
		{
			delete_file(path_r);
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
