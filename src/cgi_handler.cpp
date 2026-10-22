#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "http_request.hpp"
#include "webserv.hpp"
#include "parse_config.hpp"
#include "string_utils.hpp"

std::string	rebase_to_script_dir(std::string path)
{
	std::string::size_type pos = path.find_last_of('/');
	if (pos == std::string::npos)
		return (path);
	if (pos == 0)
	{
		if (chdir("/") == -1)
		{
			throw(std::runtime_error("chdir failed!"));
		}
		return (path.substr(1));
	}
	if (chdir(path.substr(0, pos).c_str()) == -1)
	{
		throw(std::runtime_error("chdir failed!"));
	}
	return (path.substr(pos + 1));
}

std::vector<std::string> &add_var(std::vector<std::string> &env, std::string var)
{
	std::vector<std::string>::iterator it = env.begin();
	std::string key = var.substr(0, var.find("="));
	bool	found = false;
	while(it != env.end())
	{
		if ((*it).find('=') == std::string::npos)
			throw(std::runtime_error("invalid env!"));
		if ((*it).substr(0,(*it).find('=')) == key)
		{
			*it = var;
			found = true;
		}
		it++;
	}
	if (found == false)
		env.push_back(var);
	return (env);
}

std::vector<std::string> get_CGI_env(http_request request, Server serv)
{
	char **temp = serv.envp;
	std::vector<std::string> newEnv;

	while(temp && *temp)
	{
		newEnv.push_back(*temp);
		temp++;
	}
	// https://www.rfc-editor.org/rfc/rfc3875.html#section-4.1
	add_var(newEnv, "GATEWAY_INTERFACE=CGI/1.1");
	add_var(newEnv, "SERVER_SOFTWARE=webserv/1.0");
	add_var(newEnv, "SERVER_NAME=localhost");
	add_var(newEnv, "SERVER_PORT="+int_to_string(serv.config.getPort()));
	add_var(newEnv, "SERVER_PROTOCOL="+request.version);
	add_var(newEnv, "REQUEST_METHOD="+http_request::r_methods_map.find(request.method)->second);
	add_var(newEnv, "SCRIPT_NAME="+request.url);
	add_var(newEnv, "QUERY_STRING="+request.query);
	add_var(newEnv, "REMOTE_ADDR="+request.client_IP);
	if(request.headers.find("content-type")!=request.headers.end())
		add_var(newEnv, "CONTENT_TYPE="+request.headers["content-type"]);
	add_var(newEnv, "CONTENT_LENGTH="+ int_to_string(request.message_body.size()));
	return (newEnv);
}

void	start_CGI(std::string path, http_request request, int fd, Server& serv)
{
	if (access(path.c_str(), X_OK) == -1)
		throw(http_request::http_error("Inmpossible to access CGI file!", 500));
	int	pipe_fds[2];
	if (pipe(pipe_fds) == -1)
		throw std::runtime_error("Error creating pipes!");
	pid_t p = fork();
	if (p < 0)
	{
		close(pipe_fds[0]);
		close(pipe_fds[1]);
		throw(std::runtime_error("Fork failed!"));
	}
	if (p > 0)
	{
		close(pipe_fds[0]);
		try_nonblock_fd(pipe_fds[1]);
		connection&	cgi_connection = serv.epoll.connections[pipe_fds[1]];
		cgi_connection.type = connection::WriteFD;
		cgi_connection.body = request.message_body;
		cgi_connection.fd = pipe_fds[1];
		try
		{
			add_connection_to_epoll(pipe_fds[1], &cgi_connection, serv.epoll.myepoll, EPOLLOUT);
		}
		catch(...)
		{
			serv.epoll.connections.erase(pipe_fds[1]);
			throw(http_request::http_error("CGI error!", 500));
		}
		return;
	}
	close(pipe_fds[1]);
	if (dup2(pipe_fds[0], 0) == -1)
		throw(std::runtime_error("Child Error!"));
	if (dup2(fd, 1) == -1)
		throw(std::runtime_error("Child Error!"));
	close(pipe_fds[0]);
	close(fd);

	std::vector<std::string> env_str;
	std::vector<char *>	env_char_ptr;
	std::vector<char *> argv;
	try{
		path = rebase_to_script_dir(path);
		argv.push_back((char *)path.c_str());
		argv.push_back(0);

		env_str = get_CGI_env(request, serv);
		env_char_ptr = to_char_str_tab(env_str);
	}
	catch (...)
	{
		throw(std::runtime_error("Child Error!"));
	}

	execve(path.c_str(), &argv[0], &env_char_ptr[0]);
	throw(std::runtime_error("Child Error!"));
}
