#include "webserv.hpp"
#include "parse_config.hpp"
#include <csignal>
#include <cerrno>
#include <sys/wait.h>
#include "parse_config.hpp"
#include <sys/socket.h>
#include "epoll_utils.hpp"


void reap_children(int)
{
	int status;

	while (waitpid(-1, &status, WNOHANG) > 0)
	{

	}
}



void start_server(Server& server)
{
	server.config =  parse_server("config.conf");
	start_epoll(server);
	if (signal(SIGCHLD, reap_children) == SIG_ERR)
		throw std::runtime_error("Could not install SIGCHLD handler!");

	std::cout << "My socket: " << server.epoll.mysocket << std::endl;
}

int main(int argc, char **argv, char **envp)
{
	(void)argc;
	(void)argv;
	Server	server;

	server.envp = envp;
	start_server(server);
	std::cout<< "Listening at http://[::1]:"<< server.config.getPort() <<"/\n";


	while (true)
	{
		epoll_handle(server.epoll);
	}
}