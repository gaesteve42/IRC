# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/30 15:28:46 by gaesteve          #+#    #+#              #
#    Updated: 2025/04/02 11:51:32 by gaesteve         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= ircserv
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -Iincludes

SRCS_DIR	= srcs/
OBJS_DIR	= objs/

SRCS		= main.cpp \
			  Server.cpp \
			  User.cpp \
			  Channel.cpp \
			  Parsing.cpp

SRCS_PATH	= $(addprefix $(SRCS_DIR), $(SRCS))
OBJS		= $(addprefix $(OBJS_DIR), $(SRCS:.cpp=.o))

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJS_DIR)%.o: $(SRCS_DIR)%.cpp | $(OBJS_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
