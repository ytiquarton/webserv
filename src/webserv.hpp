#pragma once

#include <string>

class connection
{
	public:
	std::string buffer;
	int			fd;
};

struct serverdata
{
	int mysocket;
	int	clientsocket;
	int	myepoll;
	struct addrinfo *myaddr;
	epoll_event	*events;
	std::map<int, connection>	connections;
};
