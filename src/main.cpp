#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <iostream>
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include "http_message.hpp"
#include <stdexcept>
#include "webserv.hpp"
#include <map>
#include <stdexcept>
#include "http_response.hpp"

void	add_connection_to_epoll(int socket_fd, connection *_connection, int epoll_fd)
{
	epoll_event	event;

	event.events = EPOLLIN | EPOLLRDHUP;
	event.data.ptr = _connection;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event);
}

void	add_socket_to_epoll(int socket_fd, int epoll_fd)
{
	epoll_event	event;

	event.events = EPOLLIN | EPOLLRDHUP;
	event.data.fd = socket_fd;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event);
}
void	log_event(epoll_event	*event)
{
	std::cout << "Event! EPOLLIN: " << (event->events & EPOLLIN) << " EPOLLHUP: " <<  (event->events & EPOLLHUP) << std::endl;
	std::cout << "fd: " << event->data.fd << std::endl;
}

void	read_connection(connection& _connection, int epoll_fd)
{
	char		*buffer;
	std::string	output;

	buffer = new char[4096];
	ssize_t	len;

	std::cout<< "Reading input...\n";
	len = read(_connection.fd, buffer, 4096);
	if (len<0)
		throw(std::runtime_error("Read fail!"));
	if (len == 0)
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, _connection.fd, NULL);
	std::cout<< "Reading End...\n";
	_connection.buffer.append(buffer, static_cast<size_t>(len));
	
	delete[] buffer;
}

void	handle_event(epoll_event	*event, int	mysocket, int myepoll, std::map<int, connection>& connections)
{
	int	newsocket;
	http_message message;
	

	if (event->events & EPOLLIN)
	{
		if (event->data.fd == mysocket)
		{
			newsocket = accept(mysocket, 0 ,0);
			std::cout << "Incoming connection!" << std::endl;
			connections[newsocket].fd = newsocket;
			add_connection_to_epoll(newsocket, &connections[newsocket], myepoll);
		}
		else
		{
			while (true)
			{
				read_connection(*static_cast<connection*>(event->data.ptr), myepoll);
				message = http_message::parse_message(static_cast<connection*>(event->data.ptr)->buffer);
				std::cout << "Received : \"";
				std::cout << static_cast<connection*>(event->data.ptr)->buffer;
				std::cout << "\" from fd: " << static_cast<connection*>(event->data.ptr)->fd << std::endl;
				if (!message.incomplete)
				{
					static_cast<connection*>(event->data.ptr)->buffer = message.left_over;
					sendHTMLPage(static_cast<connection*>(event->data.ptr)->fd);
				}
				else
				{
					std::cout << "Incomplete message !";
					break;
				}
			}
		}
	}
	else if (event->events & EPOLLHUP)
	{
		std::cout << "Connection closed fd: " << event->data.fd << std::endl;
		close(event->data.fd);
	}
}

void start_server(serverdata *data)
{

	data->mysocket = socket(AF_INET6, SOCK_STREAM, 0);
	bind(data->mysocket, data->myaddr->ai_addr, data->myaddr->ai_addrlen);
	listen(data->mysocket, 1000);
	data->myepoll = epoll_create(10000);
	add_socket_to_epoll(data->mysocket, data->myepoll);
	data->events = new epoll_event();

	std::cout << "My socket: " << data->mysocket << std::endl;
}

int main(int argc, char **argv)
{
	serverdata data;
	int numbEvents;

	if (argc != 2)
		throw(std::runtime_error("Veuillez specifier le port!\n"));
	if (getaddrinfo("::1", argv[1], 0, &data.myaddr))
		throw(std::runtime_error("Error !"));
	start_server(&data);
	std::cout<< "Listening at http://[::1]:"<< argv[1] <<"/\n";


	
	while (true)
	{
		std::cout << "Listening the socket....\n";
		numbEvents = epoll_wait(data.myepoll, data.events, 1, -1);
		(void)numbEvents;
		log_event(data.events);
		handle_event(data.events, data.mysocket, data.myepoll, data.connections);
	}
}