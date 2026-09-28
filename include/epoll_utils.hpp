#pragma once
#include <string>

class connection
{
	public:
	std::string buffer;
	int			fd;
};

void epoll_handle(serverdata *data);
void start_epoll(serverdata *data);