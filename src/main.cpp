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



void start_server(serverdata *data)
{
	parse_server("config.conf");
	start_epoll(data);
	if (signal(SIGCHLD, reap_children) == SIG_ERR)
		throw std::runtime_error("Could not install SIGCHLD handler!");

	std::cout << "My socket: " << data->mysocket << std::endl;
}

int main(int argc, char **argv, char **envp)
{
	serverdata data;

	if (argc != 2)
		throw(std::runtime_error("Veuillez specifier le port!\n"));
	if (getaddrinfo("::1", argv[1], 0, &data.myaddr))
		throw(std::runtime_error("Error !"));
	start_server(&data);
	std::cout<< "Listening at http://[::1]:"<< argv[1] <<"/\n";


	
	while (true)
	{
		epoll_handle(&data);
	}
}