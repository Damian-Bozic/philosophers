# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dbozic <marvin@42.fr>                      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/09/11 18:36:52 by dbozic            #+#    #+#              #
#    Updated: 2024/09/11 18:36:54 by dbozic           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = cc
FLAGS = -Wall -Wextra -Werror
MAIN = philosophers.c
SOURCES = helper_functions.c init_struct.c philosopher_thread.c \
		  death_tracker_thread.c 
OBJECTS = $(SOURCES:%.c=%.o)
NAME = philosophers.a
PROGRAM = philosophers

all: $(PROGRAM)

$(NAME): $(OBJECTS)
	ar rcs $(NAME) $(OBJECTS)

$(PROGRAM): $(NAME)
	$(CC) $(FLAGS) $(MAIN) $(NAME) -o $(PROGRAM)

%.o: %.c
	$(CC) $(FLAGS) -I. -o $@ -c $<

clean:
	rm -f $(OBJECTS)

fclean: clean
	rm -f $(NAME) $(PROGRAM)

cc:
	$(CC) $(FLAGS) $(MAIN) $(NAME) -o $(PROGRAM)

re: fclean all

full: all clean
	clear