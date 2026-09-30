#pragma once
#include "parse_config.hpp"
#include "epoll_utils.hpp"

class Server
{
	public:
	ServerConfig config;
	ServerEpoll epoll;
	char **envp;
};