-include ./source.mk
-include ./libft/source.mk
-include ./ft_printf/source.mk

.DEFAULT_GOAL := all

NAME	=	lemipc

CC		=	gcc
CFLAGS	=	-Wall -Wextra -Werror

BUILD_DIR	=	build
OBJS_DIR	=	$(BUILD_DIR)/objects

SRCS		=	$(addprefix parser/, $(PARSER_SRCS)) $(MAIN_SRCS)
OBJS_PATH	=	$(patsubst %.c,$(OBJS_DIR)/%.o,$(SRCS))

LIBFT_NAME		=	$(BUILD_DIR)/libft.a
LIBFT_DIR		=	libft
LIBFT_SRCS		=	$(addprefix $(LIBFT_DIR)/, $(_LIBFT_SRCS))
LIBFT_OBJS_PATH	=	$(patsubst $(LIBFT_DIR)/%.c,$(OBJS_DIR)/$(LIBFT_DIR)/%.o,$(LIBFT_SRCS))

PRINTF_NAME			=	$(BUILD_DIR)/libftprintf.a
PRINTF_DIR			=	ft_printf
PRINTF_SRCS			=	$(addprefix $(PRINTF_DIR)/, $(_PRINTF_SRCS))
PRINTF_OBJS_PATH	=	$(patsubst $(PRINTF_DIR)/%.c,$(OBJS_DIR)/$(PRINTF_DIR)/%.o,$(PRINTF_SRCS))

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

$(OBJS_DIR)/$(LIBFT_DIR)/%.o: $(LIBFT_DIR)/%.c
	@mkdir -p $(@D)
	@if [ ! -f .count1 ]; then echo 0 > .count1; fi
	@COUNT=$$(($$(cat .count1) + 1)); \
	echo $$COUNT > .count1; \
	PERCENT=$$(($$COUNT * 100 / $(TOTAL_FILES_LIBFT))); \
	printf "$(CUT)$(RESET)[$(YELLOW)%3d%%$(RESET)] 🎖️ Training Stormtroopers:$(RESET) %s\n" $$PERCENT $(notdir $<)
	@$(CC) $(CFLAGS) -I $(LIBFT_DIR)/includes -c $< -o $@
	@printf "$(UP)"

$(LIBFT_NAME): $(LIBFT_OBJS_PATH)
	@printf "$(RESET)[$(GREEN)DONE$(RESET)] ⚔️ $(CYAN)Imperial Academy training complete.$(RESET)$(CUT)\n"
	@mkdir -p $(@D)
	@ar rcs $(LIBFT_NAME) $(LIBFT_OBJS_PATH)
	@rm -f .count1
	@printf "$(RESET)[$(GREEN)DONE$(RESET)] 🛡️ $(BLUE)The $(notdir $(LIBFT_NAME)) are ready for your command.$(RESET)$(CUT)\n"

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
	@$(CC) $(CFLAGS) -c $< -o $@
	@printf "$(UP)"


$(NAME): $(LIBFT_NAME) $(PRINTF_NAME) $(OBJS_PATH)
	@printf "$(RESET)[$(GREEN)DONE$(RESET)] 🌩️ $(CYAN)Commence primary ignition!$(RESET)$(CUT)\n"
	@$(CC) $(CFLAGS) $(OBJS_PATH) $(LIBFT_NAME) $(PRINTF_NAME) -o $(NAME)
	@rm -f .count1 .count2 .count3
	@printf "$(RESET)[$(GREEN)DONE$(RESET)] 🛰️ $(BLUE)$(NAME) is fully operational!$(RESET)$(CUT)\n"

all:
	@if $(MAKE) -q $(NAME) --no-print-directory; then \
		printf "$(RESET)[$(GREEN)DONE$(RESET)] 🛰️ $(BLUE)The Death Star is already at full power, My Lord.$(RESET)\n"; \
	else \
		$(MAKE) $(NAME) --no-print-directory; \
	fi

clean:
	@rm -f .count1 .count2 .count3
	@rm -rf $(OBJS_DIR)
	@printf "$(RESET)[$(GREEN)DONE$(RESET)] 🧹 $(YELLOW)Sweeping the sector for rebel scum.$(RESET)\n"

fclean: clean
	@rm -rf $(BUILD_DIR) $(NAME)
	@printf "$(RESET)[$(GREEN)DONE$(RESET)] 💥 $(RED)The planet has been obliterated.$(RESET)\n"

re: fclean all

.PHONY: all clean fclean re
