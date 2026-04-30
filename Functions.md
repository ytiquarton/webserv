1. execve
	replace current process with a new one from an executable
2. pipe
	creates a pipe
3. strerror
	gets the string corresponding to an error
4. gai_strerror
	same as strerror but for return of getaddrinfo()
5. errno
	thread local value storing error flag
6. dup
	duplicates a file descriptor
7. dup2
	moves a file descriptor to another
8. fork
	forks duplicates the current process
9. socketpair
	creates connected sockets and returns them as fds, like a pipe but bi-directional
10. htons
	port number converter to make it compatible with the computer
11. htonl
	ip address number converter to make it compatible with the computer
12. ntohs
	port number converter to make it compatible with the computer
13. ntohl
	ip address number converter to make it compatible with the computer
14. select
	waits till one of the fds is ready for read/write and returns them, overriding the give array
15. poll
	like select but better
16. epoll_create
	creates an epoll instance
17. epoll_ctl
	registers an fd in an epoll instance
18. epoll_wait
	waits for readiness of a registered fd
19. kqueue
	equivalent of epoll_create for BSD OSs
20. kevent
	equivalent of epoll_ctl and epoll_wait for BSD OSs
21. socket
	creates a socket that can then be connected for connections with other machines
	it takes as argument the domain which defines the "range" like ipv4, ipv6, local, etc..
	other argument is the "type" which will for example choose between udp, tcp etc...
	final argument is protocol which is usually kept at 0 for the default protocol. it can be for example ip, tcp, udp etc...
22. accept
	is a blocking call on a connection-based socket (dictated by its type on socket creation), that has been bound with bind() and is listenning for connections after a listen()
	the second argument scokaddr is a structure that will be filled with the calling socket
	the third argument contains the size of the socket both the second and third arguments can be null and nothing will therefore be returned through them
	the function returns and fd refferring to the calling socket
	if the socket is non-blocking and there is no pending connection the accept will fail and set a flag
23. listen
	marks the socket for accept()
24. send
	is just write but with socket related flags
25. recv
	is just like read but with socket related flags
26. chdir
	changes the working directory to the one specified as argument
27. bind
	assigns and address to a socket
28. connect
	connects the socket to an address (that can be outside the computer)
29. getaddrinfo
	returns addrinfo structures that can be used for bind and connect. It allost for host/ip search. Service argument corresponds to the port. Node argument is either a numerical network address (IPv4/IPv6) or a hostname.
30. freeaddrinfo
	frees the structure returned by getaddrinfo
31. setsockopt
	sets the option_name option to the socket at a chosen protocol level
32. getsockname
	returns the current adress to which the socket is bound
33. getprotobyname
	returns a protoent structure that matches the protocol name => deprecated, use getaddrinfo() instead
34. fcntl
	performs an operation on the given open file descriptor.
35. close
	closes a fd
36. read
	reads a fd
37. write
	writes in an fd
38. waitpid
	waits for a proccess id to end and returns its output
39. kill
	sends a signal to a process
40. signal
	sets up signal reception with handlers
41. access
	checks whether the calling process can access a path and performs accessibility checks
42. stat
	returns information on a file
43. open
	open a path and returns the fd (can create the file if not present)
44. opendir
	opens a directory stream
45. readdir
	returns a dirent structure referring to the next directory entryy in the directory stream
46. closedir
	closes a directory stream