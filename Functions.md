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
	
24. send
25. recv
26. chdir
27. bind
28. connect
29. getaddrinfo
30. freeaddrinfo
31. setsockopt
32. getsockname
33. getprotobyname
34. fcntl
35. close
36. read
37. write
38. waitpid
39. kill
40. signal
41. access
42. stat
43. open
44. opendir
45. readdir
46. closedir