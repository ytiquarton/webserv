#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdexcept>

void	start_CGI(std::string path, std::string message_body, int fd, char* envp)
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
		int index(0);
		close(pipe_fds[0]);
		while (index < message_body.size())
		{
			index += write(pipe_fds[1], message_body.c_str(), message_body.size());
		}
		close(pipe_fds[1]);
		return;
	}
	close(pipe_fds[1]);
	if (dup2(pipe_fds[0], 0) == -1 || dup2(fd, 1) == -1)
		throw(std::runtime_error("CGI child fail!"));
	close(pipe_fds[0]);
	close(fd);
	execve(path.c_str(), 0, 0);
	throw(std::runtime_error("CGI child fail!"));
}