CC = c++
NAME = webserv
CFLAGS = -Wall -Wextra -Werror -Iincludes -std=c++98 -MMD

SRC_DIR = srcs
OBJ_DIR = objs

SRCS = $(addprefix $(SRC_DIR)/, main.cpp AHttpMessage.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

-include $(OBJS:%.o=%.d)

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
