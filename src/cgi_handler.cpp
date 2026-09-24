#include <string>
#include <stdio.h>

void	start_CGI(std::string path, std::string message_body, int fd)
{
	pid_t p = fork();
	if (p<0)
		throw(std::runtimeerror("Fork failed!"));
}