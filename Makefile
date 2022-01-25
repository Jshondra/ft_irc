NAME		=	ircserv

SRC			=	main	Server	Client	Channels	\
				privmsg		req_client	handler	authorization	\
				resp_serv

SRCS		=	$(addsuffix .cpp, $(addprefix srcs/, $(SRC)))

OBJS		=	$(SRCS:.cpp=.o)

CC			=	c++

FLAGS		=	-Wall -Wextra -Werror -std=c++98 -I includes/

HEADER		=	includes/Server.hpp includes/Client.hpp includes/Channels.hpp

RM			=	rm -f

OFF_COLOR	=	\033[0m
RED			=	\033[0;31m
GREEN		=	\033[0;32m

.PHONY		:	all clean fclean re

.cpp.o		:
			@c++ $(FLAGS) -c $< -o ${<:.cpp=.o}

all			:	$(NAME)

$(NAME)		: 	$(OBJS) $(HEADER)
			@$(CC) $(FLAGS) $(OBJS) -o $(NAME)
			@echo "$(GREEN)Done!$(OFF_COLOR)"

clean		:
			@$(RM) $(OBJS)
			@echo "$(RED)🤷 Object files are absent! 🤷$(OFF_COLOR)"

fclean		:	clean
			@$(RM) $(NAME)
			@echo "${RED}🗑 Deleted!${OFF_COLOR}"

re			:	fclean all
