#pragma once
#include <string>
#include <netdb.h>
#include <sys/epoll.h>
#include <map>

class Server;

class connection
{
	public:
	bool		is_cgi_fd;
	std::string buffer;
	int			fd;
	std::string	client_address;
	size_t		index;
	std::string body;
};

struct ServerEpoll
{
	int mysocket;
	int	clientsocket;
	int	myepoll;
	struct addrinfo *myaddr;
	epoll_event	*events;
	std::map<int, connection>	connections;
};


void epoll_handle(ServerEpoll& data, Server& serv);
void start_epoll(Server& data);
void	try_nonblock_fd(int fd);
void	add_connection_to_epoll(int socket_fd, connection *_connection, int epoll_fd, unsigned int events);
