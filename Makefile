# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ezeppa <ezeppa@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/03/03 00:00:00 by ezeppa            #+#    #+#              #
#    Updated: 2026/03/03 00:00:00 by vbonnard           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ================================ VARIABLES ================================= #

NAME		= ircserv

CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98
INCLUDES	= -I include -I include/bnf

# ================================== SOURCES ================================= #

SRC_DIR		= src
OBJ_DIR		= obj

SRCS		= src/main.cpp \
			  src/Server.cpp \
			  src/Client.cpp \
			  src/Channel.cpp \
			  src/Logger.cpp \
			  src/commands/CommandManager.cpp \
			  src/commands/cmd_auth.cpp \
			  src/commands/cmd_channel.cpp \
			  src/commands/cmd_mode.cpp \
			  src/commands/cmd_ops.cpp \
			  src/commands/cmd_privmsg.cpp \
			  src/bnf/AST.cpp \
			  src/bnf/Arena.cpp \
			  src/bnf/BNFParser.cpp \
			  src/bnf/BNFTokenizer.cpp \
			  src/bnf/DataExtractor.cpp \
			  src/bnf/Expression.cpp \
			  src/bnf/ExpressionInterner.cpp \
			  src/bnf/ExtractedData.cpp \
			  src/bnf/Grammar.cpp

OBJS		= $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# ================================== COLORS ================================== #

RED			= \033[0;31m
GREEN		= \033[0;32m
YELLOW		= \033[0;33m
BLUE		= \033[0;34m
CYAN		= \033[0;36m
RESET		= \033[0m

# =================================== RULES ================================== #

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(CYAN)Linking $(NAME)...$(RESET)"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)$(NAME) compiled successfully$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "$(BLUE)Compiling $<...$(RESET)"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@echo "$(GREEN)Clean completed$(RESET)"

fclean: clean
	@echo "$(RED)Removing $(NAME)...$(RESET)"
	@rm -f $(NAME)
	@echo "$(GREEN)Full clean completed$(RESET)"

re: fclean all

.PHONY: all clean fclean re
