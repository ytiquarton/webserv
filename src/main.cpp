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
#include "parse_http.cpp"

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

std::string	full_read_fd(int fd)
{
	char		*buffer;
	std::string	output;

	buffer = new char[4096];
	ssize_t	len;
	while (true)
	{
		std::cout<< "Reading input...\n";
		len = read(fd, buffer, 4096);
		std::cout<< "Reading End...\n";
		if (len == 0)
			break;
		else if(len < 0)
		{
			if (errno == EINTR)
				continue;
			break;
		}
		output.append(buffer, static_cast<size_t>(len));
	}
	std::cout<< "Exit...\n";
	delete[] buffer;
	return(output);
}

void	handle_event(epoll_event	*event, int	mysocket, int myepoll)
{
	int	newsocket;
	http_message message;
	

	if (event->events & EPOLLIN)
	{
		if (event->data.fd == mysocket)
		{
			newsocket = accept(mysocket, 0 ,0);
			std::cout << "Incoming connection!" << std::endl;
			add_socket_to_epoll(newsocket, myepoll);
		}
		else
		{
			message = parse_message(full_read_fd(event->data.fd));
			std::cout << "Received : \"";
			std::cout << message.content_str;
			std::cout << "\" from fd: " << event->data.fd << std::endl;
		}
	}
	else if (event->events & EPOLLHUP)
	{
		std::cout << "Connection closed fd: " << event->data.fd << std::endl;
		close(event->data.fd);
	}
}


int main(int argc, char **argv)
{
	int mysocket;
	int	clientsocket;
	int	myepoll;
	int numbEvents;
	struct addrinfo *myaddr;
	epoll_event	*events;
	char	*buffer = new char[1001]();

	(void)argc;
	if (getaddrinfo("::1", argv[1], 0, &myaddr))
		std::cout << "Error !";

	mysocket = socket(AF_INET6, SOCK_STREAM, 0);
	bind(mysocket, myaddr->ai_addr, myaddr->ai_addrlen);
	listen(mysocket, 1000);
	myepoll = epoll_create(10000);
	add_socket_to_epoll(mysocket, myepoll);
	events = new epoll_event();

	std::cout << "My socket: " << mysocket << std::endl;
	while (true)
	{
		std::cout << "Listening the socket....\n";
		numbEvents = epoll_wait(myepoll, events, 1, -1);
		(void)numbEvents;
		log_event(events);
		handle_event(events, mysocket, myepoll);
	}
	std::cout << "Event! EPOLLIN: " << (events->events & EPOLLIN) << " EPOLLHUP: " <<  (events->events & EPOLLHUP) << std::endl;
	std::cout << "fd: " << events->data.fd << std::endl;
	clientsocket = accept(mysocket, 0, 0);
	std::cout << "Client socket: " << clientsocket << std::endl;
	add_socket_to_epoll(clientsocket, myepoll);

	read(clientsocket, buffer, 1000);
	std::cout << "Buffer: " << buffer << std::endl;
	std::cout << "Listening the socket....\n";
	epoll_wait(myepoll, events, 1, -1);
	std::cout << "Event! EPOLLIN: " << (events->events & EPOLLIN) << " EPOLLHUP: " <<  (events->events & EPOLLHUP) << std::endl;
	std::cout << "fd: " << events->data.fd << std::endl;
	free (events);
}