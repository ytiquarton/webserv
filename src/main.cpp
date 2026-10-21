#include "webserv.hpp"
#include "parse_config.hpp"
#include <csignal>
#include <cerrno>
#include <sys/wait.h>
#include "parse_config.hpp"
#include <sys/socket.h>
#include "epoll_utils.hpp"
#include <cerrno>


void reap_children(int)
{
	int status;

	int	no = errno;
	while (waitpid(-1, &status, WNOHANG) > 0)
	{

	}
	errno = no;
}



void start_server(Server& server, std::string confpath)
{
	server.config =  parse_server(confpath);
	start_epoll(server);
	if (signal(SIGCHLD, reap_children) == SIG_ERR)
	{
		close_all_connections(server.epoll.myepoll, server.epoll.connections);
		close(server.epoll.myepoll);
		delete server.epoll.events;
		throw std::runtime_error("Could not install SIGCHLD handler!");
	}
	std::cout << "My socket: " << server.epoll.mysocket << std::endl;
}

int main(int argc, char **argv, char **envp)
{
	(void)argc;
	(void)argv;
	Server	server;

	server.envp = envp;
	if (argc > 1)
		start_server(server, argv[1]);
	else
		start_server(server, "config.conf");
	std::cout<< "Listening at http://[::1]:"<< server.config.getPort() <<"/\n";

	try{
		while (true)
		{
			epoll_handle(server.epoll, server);
		}
	}
	catch(std::exception &e)
	{
		close_all_connections(server.epoll.myepoll, server.epoll.connections);
		close(server.epoll.myepoll);
		delete server.epoll.events;
		throw;
	}
}