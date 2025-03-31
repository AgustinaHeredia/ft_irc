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
	$(SRC_DIR)/server/Config.cpp \
	$(SRC_DIR)/server/Reply.cpp \
	$(SRC_DIR)/channel/Channel.cpp \
	$(SRC_DIR)/clients/Client.cpp \
	$(SRC_DIR)/commands/CommandHandler.cpp \
	$(SRC_DIR)/commands/NICK.cpp \
	$(SRC_DIR)/commands/USER.cpp \
	$(SRC_DIR)/commands/PRIVMSG.cpp \
	$(SRC_DIR)/commands/JOIN.cpp \
	$(SRC_DIR)/commands/PART.cpp \
	$(SRC_DIR)/commands/KICK.cpp \
	$(SRC_DIR)/commands/TOPIC.cpp \
	$(SRC_DIR)/commands/MODE.cpp \
	$(SRC_DIR)/commands/INVITE.cpp \
	$(SRC_DIR)/commands/WHO.cpp \
	$(SRC_DIR)/commands/NOTICE.cpp \
	$(SRC_DIR)/commands/BOT.cpp \
	$(SRC_DIR)/commands/DCC.cpp \
	$(INC_DIR)/utils.cpp 


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