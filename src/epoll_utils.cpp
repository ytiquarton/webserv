#include <sys/epoll.h>
#include "epoll_utils.hpp"
#include <iostream>
#include <unistd.h>
#include "http_request.hpp"
#include "http_handler.hpp"
#include <sys/socket.h>
#include "webserv.hpp"
#include "string_utils.hpp"
#include <fcntl.h>
#include "http_response.hpp"
#include <exception>
#include <cerrno>


void	add_connection_to_epoll(int fd, connection *_connection, int epoll_fd, unsigned int events)
{
	epoll_event	event;

	event.events = events;
	event.data.ptr = _connection;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		close(fd);
		throw(std::runtime_error("Epoll error!"));
	}
}

void start_epoll(Server& server)
{
	std::cout << " Starting epoll... " << std::endl;
	ServerEpoll& epoll_data = server.epoll;
	std::cout << " Port : " << server.config.getPort() << std::endl;
	if (getaddrinfo("::1", int_to_string(server.config.getPort()).c_str(), 0, &server.epoll.myaddr))
		throw(std::runtime_error("Address error !"));
	epoll_data.mysocket = socket(AF_INET6, SOCK_STREAM, 0);
	if (epoll_data.mysocket == -1)
	{
		freeaddrinfo(epoll_data.myaddr);
		throw(std::runtime_error("socket error!"));
	}
	try
	{
		try_nonblock_fd(epoll_data.mysocket);
	}
	catch(const std::exception& e)
	{
		freeaddrinfo(epoll_data.myaddr);
		throw;
	}
	
	int reuse = 1;
	if(setsockopt(epoll_data.mysocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)))
	{
		close(epoll_data.mysocket);
		freeaddrinfo(epoll_data.myaddr);
		throw(std::runtime_error("sockopt error!"));
	}
	if (bind(epoll_data.mysocket, epoll_data.myaddr->ai_addr, epoll_data.myaddr->ai_addrlen) == -1)
	{
		close(epoll_data.mysocket);
		freeaddrinfo(epoll_data.myaddr);
		throw(std::runtime_error("bind error!"));
	}
	freeaddrinfo(epoll_data.myaddr);
	if (listen(epoll_data.mysocket, 1000) == -1)
	{
		close(epoll_data.mysocket);
		throw(std::runtime_error("epoll listen error!"));
	}

	epoll_data.myepoll = epoll_create(10000);
	if (epoll_data.myepoll == -1)
	{
		close(epoll_data.mysocket);
		throw (std::runtime_error("epoll_create failed!"));
	}
	server.epoll.connections[epoll_data.mysocket].type = connection::Epoll;
	server.epoll.connections[epoll_data.mysocket].fd = epoll_data.mysocket;
	try
	{
		add_connection_to_epoll(epoll_data.mysocket, &server.epoll.connections[epoll_data.mysocket], epoll_data.myepoll, EPOLLIN);
	}
	catch(const std::exception& e)
	{
		server.epoll.connections.erase(epoll_data.mysocket);
		close(epoll_data.myepoll);
		throw;
	}
	
	epoll_data.events = new epoll_event();
}


void	log_event(epoll_event	*event)
{
	std::cout << "Event! EPOLLIN: " << (event->events & EPOLLIN) << " EPOLLHUP: " <<  (event->events & EPOLLHUP) << std::endl;
	std::cout << "fd: " << (*static_cast<connection*>(event->data.ptr)).fd << std::endl;
}

void	close_and_remove_fd(int epoll, int fd, std::map<int, connection>& connections)
{
	epoll_ctl(epoll, EPOLL_CTL_DEL, fd, NULL);
	close(fd);
	connections.erase(fd);
}

int	read_connection(connection& _connection)
{
	char		*buffer;
	std::string	output;

	buffer = new char[4096];
	ssize_t	len;

	len = read(_connection.fd, buffer, 4096);
	if (len <= 0)
		return (delete[] buffer, 0);
	_connection.buffer.append(buffer, static_cast<size_t>(len));

	delete[] buffer;
	return (1);
}

std::string	address_to_string(unsigned const char *input)
{
	std::ostringstream	ip;

	ip << std::hex;

	for (int i = 0; i < 16; i += 2)
	{
		ip << (((unsigned int)input[i] << 8) | (unsigned int)input [i+1]);
		if (i!= 14)
			ip << ":";
	}
	return (ip.str());
}

void	close_all_connections(int epoll, std::map<int, connection>& connections)
{
	while (connections.size() > 0)
	{
		close_and_remove_fd(epoll, connections.begin()->first, connections);
	}
}

void	handle_event(epoll_event	*event, int	mysocket, int myepoll, std::map<int, connection>& connections, Server& serv)
{
	int	newsocket;
	http_request message;
	connection& event_connection = *static_cast<connection*>(event->data.ptr);
	
	if (event->events & EPOLLERR)
	{
		if (event_connection.type == connection::Epoll)
		{
			close_all_connections(myepoll, connections);
			throw(std::runtime_error("Main connection broken, server shut down..."));
		}
		close_and_remove_fd(serv.epoll.myepoll, event_connection.fd, serv.epoll.connections);
	}
	else if (event->events & EPOLLIN)
	{
		if (event_connection.type == connection::Epoll)
		{
			sockaddr_in6	clientAddr;
			socklen_t	addrLen = sizeof(clientAddr);
			newsocket = accept(mysocket, (sockaddr *)&clientAddr, &addrLen);
			if (newsocket == -1)
				return;
			try
			{
				try_nonblock_fd(newsocket);
			}
			catch(const std::exception& e)
			{
				return;
			}
			connections[newsocket].client_address = address_to_string((unsigned char *)(&clientAddr.sin6_addr));
			std::cout << "Incoming connection!" << std::endl;
			connections[newsocket].fd = newsocket;
			connections[newsocket].type = connection::Client;
			try
			{
				add_connection_to_epoll(newsocket, &connections[newsocket], myepoll,  EPOLLIN | EPOLLRDHUP);
			}
			catch(const std::exception& e)
			{
				connections.erase(newsocket);
			}
		}
		else
		{
			if (read_connection(event_connection) == 0)
			{
				close_and_remove_fd(serv.epoll.myepoll, event_connection.fd, serv.epoll.connections);
				return;
			}
			while (true)
			{
				try
				{
					message = http_request::parse_message(event_connection.buffer);
				}
				catch(...)
				{
					safe_send_Error(event_connection.fd, http_request::http_error("Error parsing message!", 400), serv, true);
					close_and_remove_fd(serv.epoll.myepoll, event_connection.fd, serv.epoll.connections);
					return;
				}
				
				message.client_IP = event_connection.client_address;
				std::cout << "Received : \"";
				std::cout << event_connection.buffer;
				std::cout << "\" from fd: " << event_connection.fd << std::endl;
				if (!message.incomplete)
				{
					event_connection.buffer = message.left_over;
					handle_http_message(event_connection.fd, message, serv);
				}
				else
				{
					std::cout << "Incomplete message !\n";
					break;
				}
			}
		}
	}
	else if (event->events & EPOLLHUP)
	{
		std::cout << "Connection closed fd: " << event_connection.fd << std::endl;
		close_and_remove_fd(serv.epoll.myepoll, event_connection.fd, serv.epoll.connections);
	}
	else if ((event->events & EPOLLOUT) && event_connection.type == connection::WriteFD)
	{
		if (event_connection.index < event_connection.body.size())
		{
			ssize_t written = write(
				event_connection.fd , event_connection.body.substr(event_connection.index).c_str(),
				event_connection.body.size() - event_connection.index
			);
			if (written > 0)
				event_connection.index += written;
		}
		if (event_connection.index >= event_connection.body.size())
			close_and_remove_fd(serv.epoll.myepoll, event_connection.fd, serv.epoll.connections);
	}
}

void epoll_handle(ServerEpoll& data, Server& serv)
{
	int numbEvents;

	std::cout << "Listening the socket....\n";
	numbEvents = epoll_wait(data.myepoll, data.events, 1, -1);
	if (numbEvents == -1)
	{
		if (errno == EINTR)
			return;
		throw(std::runtime_error("Epoll error!"));
	}
	if(numbEvents == 0)
		return;
	std::cout << "Numb of events: " << numbEvents << std::endl;
	log_event(data.events);
	handle_event(data.events, data.mysocket, data.myepoll, data.connections, serv);
}

void	try_nonblock_fd(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1 || fcntl (fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		close(fd);
		throw(std::runtime_error("Error while turning fd nonblocking!"));
	}
}