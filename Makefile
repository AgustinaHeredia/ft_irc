NAME = ircserv
FLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -pedantic -Wshadow -fsanitize=address
CC = c++
SRC_DIR = srcs
OBJ_DIR = obj
INC_DIR = srcs/utils

SRC = $(SRC_DIR)/main.cpp \
	$(SRC_DIR)/server/Server.cpp \
	$(SRC_DIR)/server/ClientManager.cpp \
	$(SRC_DIR)/server/ChannelManager.cpp \
	$(SRC_DIR)/channel/Channel.cpp \
	$(SRC_DIR)/clients/Client.cpp \
	$(SRC_DIR)/comands/CommandHandler.cpp \
	$(SRC_DIR)/comands/NICK.cpp \
	$(SRC_DIR)/comands/USER.cpp \
	$(SRC_DIR)/comands/PRIVMSG.cpp \
	$(SRC_DIR)/comands/JOIN.cpp \
	$(SRC_DIR)/comands/PART.cpp \
	$(SRC_DIR)/comands/KICK.cpp

OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEP = $(OBJ:.o=.d)

# Colors
GREEN = \033[0;32m
YELLOW = \033[0;33m
RED = \033[0;31m
DEFAULT = \033[0m

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -I$(INC_DIR) -c $< -o $@

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(NAME)
	@echo "$(GREEN)Build complete!$(DEFAULT)"

-include $(DEP)

clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(YELLOW)$(NAME) object files removed!$(DEFAULT)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)$(NAME) removed!$(DEFAULT)"

re: fclean all

.PHONY: all clean fclean re