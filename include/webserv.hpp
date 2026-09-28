#pragma once

#include <string>
#include <netdb.h>

struct serverdata
{
	int mysocket;
	int	clientsocket;
	int	myepoll;
	struct addrinfo *myaddr;
	epoll_event	*events;
	std::map<int, connection>	connections;
};
