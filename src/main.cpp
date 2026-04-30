#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <iostream>
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>

void	add_socket_to_epoll(int socket_fd, int epoll_fd)
{
	epoll_event	event;

	event.events = EPOLLIN | EPOLLRDHUP;
	event.data.fd = socket_fd;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event);
}

void	log_events(epoll_event *events, int	len)
{
	if (!events)
		return ;
	log_event(events);
	log_events(events->data->)
}

void	log_event(epoll_event	*event)
{
	std::cout << "Event! EPOLLIN: " << (event->events & EPOLLIN) << " EPOLLHUP: " <<  (event->events & EPOLLHUP) << std::endl;
	std::cout << "fd: " << event->data.fd << std::endl;
}

int main()
{
	int mysocket;
	int	clientsocket;
	int	myepoll;
	struct addrinfo *myaddr;
	epoll_event	*events;
	char	*buffer = new char[1001]();

	if (getaddrinfo("::1", "8081", 0, &myaddr))
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
		epoll_wait(myepoll, events, 1, -1);
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