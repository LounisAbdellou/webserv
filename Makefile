CC = c++
NAME = webserv
CFLAGS = -Wall -Wextra -Werror -Iincludes -std=c++98

SRC_DIR = srcs
OBJ_DIR = objs

SRCS = $(addprefix $(SRC_DIR)/, main.cpp Webserv.cpp Parser.cpp Server.cpp Client.cpp Location.cpp Validator.cpp Request.cpp Response.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
