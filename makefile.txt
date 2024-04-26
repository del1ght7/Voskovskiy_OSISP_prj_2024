CC = gcc
NAME = myrescue
CFLAGS = -std=c11 -pedantic -W -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable

.PHONY: clean
all: $(NAME)
$(NAME): pfr.c analyze_com_line.h rescue.h logger.h
	$(CC) $(CFLAGS) pfr.c analyze_com_line.h rescue.h logger.h -lrt -pthread -o $(NAME)
clean:
	rm -rf $(NAME)
