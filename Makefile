NAME=game
NCURSES = $(shell ncursesw6-config --cflags --libs)

all: $(NAME)

$(NAME): $(NAME).c
	gcc -Wall -Werror -std=gnu11  -o $(NAME) main.c $(NAME).c helpers.c -pthread $(NCURSES) -fcf-protection=branch

run:
	./$(NAME)
		
clean:
	rm -rf $(NAME) $(NAME)
