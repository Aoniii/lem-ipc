-include ./source.mk
-include ./libft/source.mk
-include ./ft_printf/source.mk
-include ./parser/source.mk

.DEFAULT_GOAL := all

NAME	=	lemipc

CC		=	gcc
CFLAGS	=	-Wall -Wextra -Werror

BUILD_DIR	=	build
OBJS_DIR	=	$(BUILD_DIR)/objects

SRCS		=	$(addprefix parser/, $(PARSER_SRCS)) $(addprefix src/, $(MAIN_SRCS))
OBJS_PATH	=	$(patsubst %.c,$(OBJS_DIR)/%.o,$(SRCS))

LIBFT_DIR			=	libft
LIBFT_NAME			=	$(LIBFT_DIR)/libft.a
LIBFT_SRCS			=	$(addprefix $(LIBFT_DIR)/, $(_LIBFT_SRCS))
LIBFT_LINK			=	-L $(LIBFT_DIR) -lft

PRINTF_NAME			=	$(BUILD_DIR)/libftprintf.a
PRINTF_DIR			=	ft_printf
PRINTF_SRCS			=	$(addprefix $(PRINTF_DIR)/, $(_PRINTF_SRCS))
PRINTF_OBJS_PATH	=	$(patsubst $(PRINTF_DIR)/%.c,$(OBJS_DIR)/$(PRINTF_DIR)/%.o,$(PRINTF_SRCS))

GET_NEXT_LINE_DIR	=	get_next_line
GET_NEXT_LINE_SRCS	=	$(addprefix $(GET_NEXT_LINE_DIR)/, get_next_line_bonus.c get_next_line_utils_bonus.c)
SRCS				+=	$(GET_NEXT_LINE_SRCS)

PARSER_DIR	=	parser

RED			=	\033[1;31m
GREEN		=	\033[1;32m
YELLOW		=	\033[1;33m
BLUE		=	\033[1;34m
CYAN		=	\033[1;36m
RESET		=	\033[0m
UP			=	\033[A
CUT			=	\033[K

TOTAL_FILES_LIBFT	=	$(words $(LIBFT_SRCS))
TOTAL_FILES_PRINTF	=	$(words $(PRINTF_SRCS))
TOTAL_FILES			=	$(words $(SRCS))

$(LIBFT_NAME): $(LIBFT_SRCS)
	@$(MAKE) -C $(LIBFT_DIR) --no-print-directory

$(OBJS_DIR)/$(PRINTF_DIR)/%.o: $(PRINTF_DIR)/%.c
	@mkdir -p $(@D)
	@if [ ! -f .count2 ]; then echo 0 > .count2; fi
	@COUNT=$$(($$(cat .count2) + 1)); \
	echo $$COUNT > .count2; \
	PERCENT=$$(($$COUNT * 100 / $(TOTAL_FILES_PRINTF))); \
	printf "$(CUT)$(RESET)[$(YELLOW)%3d%%$(RESET)] 📡 Imperial Comms Calibration: %s\n" $$PERCENT $(notdir $<)
	@$(CC) $(CFLAGS) -I $(PRINTF_DIR)/includes -c $< -o $@
	@printf "$(UP)"

$(PRINTF_NAME): $(PRINTF_OBJS_PATH)
	@printf "$(RESET)[$(GREEN)DONE$(RESET)] 📟 $(CYAN)HoloNet uplink established.$(RESET)$(CUT)\n"
	@mkdir -p $(@D)
	@ar rcs $(PRINTF_NAME) $(PRINTF_OBJS_PATH)
	@rm -f .count2
	@printf "$(RESET)[$(GREEN)DONE$(RESET)] 🦾 $(BLUE)$(notdir $(PRINTF_NAME)) are now operational.$(RESET)$(CUT)\n"

$(OBJS_DIR)/%.o: %.c
	@mkdir -p $(@D)
	@if [ ! -f .count3 ]; then echo 0 > .count3; fi
	@COUNT=$$(($$(cat .count3) + 1)); \
	echo $$COUNT > .count3; \
	PERCENT=$$(($$COUNT * 100 / $(TOTAL_FILES))); \
	printf "$(CUT)$(RESET)[$(YELLOW)%3d%%$(RESET)] 🏗️ Constructing Death Star: %s\n" $$PERCENT $(notdir $<)
	@$(CC) $(CFLAGS) -I include -I $(LIBFT_DIR) -I $(PARSER_DIR) -I $(GET_NEXT_LINE_DIR) -c $< -o $@
	@printf "$(UP)"

$(NAME): $(LIBFT_NAME) $(PRINTF_NAME) $(OBJS_PATH)
	@printf "$(RESET)[$(GREEN)DONE$(RESET)] 🌩️ $(CYAN)Commence primary ignition!$(RESET)$(CUT)\n"
	@$(CC) $(CFLAGS) $(OBJS_PATH) $(LIBFT_NAME) $(PRINTF_NAME) -o $(NAME) -lncursesw
	@rm -f .count2 .count3
	@printf "$(RESET)[$(GREEN)DONE$(RESET)] 🛰️ $(BLUE)$(NAME) is fully operational!$(RESET)$(CUT)\n"

all:
	@if $(MAKE) -q $(NAME) --no-print-directory; then \
		printf "$(RESET)[$(GREEN)DONE$(RESET)] 🛰️ $(BLUE)The Death Star is already at full power, My Lord.$(RESET)\n"; \
	else \
		$(MAKE) $(NAME) --no-print-directory; \
	fi

clean:
	@$(MAKE) -C $(LIBFT_DIR) clean --no-print-directory
	@rm -f .count2 .count3
	@rm -rf $(OBJS_DIR)
	@printf "$(RESET)[$(GREEN)DONE$(RESET)] 🧹 $(YELLOW)Sweeping the sector for rebel scum.$(RESET)\n"

fclean: clean
	@$(MAKE) -C $(LIBFT_DIR) fclean --no-print-directory
	@rm -rf $(BUILD_DIR) $(NAME)
	@printf "$(RESET)[$(GREEN)DONE$(RESET)] 💥 $(RED)The planet has been obliterated.$(RESET)\n"

re: fclean all

.PHONY: all clean fclean re
