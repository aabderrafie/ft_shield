
CC = c++
CFLAGS = -Wall -Wextra -std=c++11 -O3 -pthread

NAME = shield

src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)

all: $(obj)
	$(CC) $(CFLAGS) -o $(NAME) $(obj) 

clean:
	rm -f $(obj)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re