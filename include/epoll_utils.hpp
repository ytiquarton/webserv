#pragma once
#include <string>
#include <netdb.h>
#include <sys/epoll.h>
#include <map>

class Server;

class connection
{
	public:
	std::string buffer;
	int			fd;
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


void epoll_handle(ServerEpoll& data);
void start_epoll(Server& data);