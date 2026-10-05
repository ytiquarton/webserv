NAME		= webserv

CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -g
RM			= rm -f

SRCS		= $(addprefix src/, cgi_handler.cpp epoll_utils.cpp http_handler.cpp http_message.cpp http_response.cpp main.cpp parse_config.cpp server.cpp string_utils.cpp utils.cpp)
HEADERS		= include
OBJS		= $(patsubst src/%.cpp,obj/%.o,$(SRCS))

all: $(NAME)

$(NAME): $(OBJS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

obj/%.o: src/%.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -I$(HEADERS) -c $< -o $@
	

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re