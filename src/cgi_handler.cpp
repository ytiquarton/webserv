#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdexcept>
#include <vector>
#include <iostream>
#include "http_request.hpp"

char	**get_CGI_env(char **originalEnv, http_request request)
{

}

void	start_CGI(std::string path, http_request request, int fd)
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
		while (index < request.body.size())
		{
			index += write(pipe_fds[1], request.body.c_str(), request.body.size());
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

	
	std::vector<char *> envp;
	envp.push_back("REQUEST_METHOD"+request.method);

	execve(path.c_str(), &argv[0], &envp[0]);
	std::cerr << "Child fail?\n\n";
	throw(std::runtime_error("CGI child fail!"));
}