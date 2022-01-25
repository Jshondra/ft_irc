NAME		=	ircserv

SRC			=	main	Server	Client	Server_cmd	Channels	\
				privmsg		request		utils	away	auth	\
				response

SRCS		=	$(addsuffix .cpp, $(addprefix srcs/, $(SRC)))

OBJS		=	$(SRCS:.cpp=.o)

CC			=	c++

FLAGS		=	-Wall -Wextra -Werror -std=c++98 -I includes/

HEADER		=	includes/Server.hpp includes/Client.hpp includes/Channels.hpp

.cpp.o		:
			c++ $(FLAGS) -c $< -o ${<:.cpp=.o}

all			:	$(NAME)

$(NAME)		: 	$(OBJS) $(HEADER)
			$(CC) $(FLAGS) $(OBJS) -o $(NAME)

clean		:
			rm -f $(OBJS)

fclean		:	clean
			rm -f $(NAME)

re			:	fclean all

.PHONY		:	all clean fclean re