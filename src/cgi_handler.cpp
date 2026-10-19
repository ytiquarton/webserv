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

char	**get_CGI_env(http_request request, Server serv)
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
}

void	start_CGI(std::string path, http_request request, int fd, Server serv)
{
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
		unsigned int index(0);
		close(pipe_fds[0]);
		while (index < request.message_body.size())
		{
			index += write(pipe_fds[1], request.message_body.c_str(), request.message_body.size());
		}
		close(pipe_fds[1]);
		return;
	}
	close(pipe_fds[1]);
	if (dup2(pipe_fds[0], 0) == -1)
		throw(std::runtime_error("CGI child fail!"));
	if (dup2(fd, 1) == -1)
		throw(std::runtime_error("CGI child fail!"));
	close(pipe_fds[0]);
	close(fd);
	std::vector<char *> argv;
	argv.push_back((char *)path.c_str());
	argv.push_back(0);

	

	execve(path.c_str(), &argv[0], get_CGI_env(request, serv));
	std::cerr << "Child fail?\n\n";
	throw(std::runtime_error("CGI child fail!"));
}