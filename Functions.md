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

22. accept
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