-include source.mk

NAME		=	libft.a

CC			=	cc
AR			=	ar
ARFLAGS		=	rcs
CFLAGS		=	-Wall -Wextra -Werror -I .

SRCS		=	$(_LIBFT_SRCS)
BONUS		=	$(_LIBFT_BONUS)

OBJS		=	$(SRCS:.c=.o)
BONUS_OBJS	=	$(BONUS:.c=.o)

GREEN	=	\033[1;32m
YELLOW	=	\033[1;33m
BLUE	=	\033[1;34m
CYAN	=	\033[1;36m
RESET	=	\033[0m
UP		=	\033[A
CUT		=	\033[K

ifeq ($(MAKECMDGOALS),bonus)
TOTAL_FILES	=	$(words $(BONUS))
else
TOTAL_FILES	=	$(words $(SRCS))
endif

.DEFAULT_GOAL := all

%.o: %.c
	@if [ ! -f .count ]; then echo 0 > .count; fi
	@COUNT=$$(($$(cat .count) + 1)); \
	echo $$COUNT > .count; \
	PERCENT=$$(($$COUNT * 100 / $(TOTAL_FILES))); \
	printf "$(CUT)$(RESET)[$(YELLOW)%3d%%$(RESET)] 🎖️ Training Stormtroopers:$(RESET) %s\n" $$PERCENT $(notdir $<)
	@$(CC) $(CFLAGS) -c $< -o $@
	@printf "$(UP)"

$(NAME): $(OBJS)
	@printf "$(RESET)[$(GREEN)DONE$(RESET)] ⚔️ $(CYAN)Imperial Academy training complete.$(RESET)$(CUT)\n"
	@$(AR) $(ARFLAGS) $(NAME) $(OBJS)
	@rm -f .count
	@printf "$(RESET)[$(GREEN)DONE$(RESET)] 🛡️ $(BLUE)$(NAME) is ready for your command.$(RESET)$(CUT)\n"

all: $(NAME)

bonus: $(NAME) $(BONUS_OBJS)
	@$(AR) $(ARFLAGS) $(NAME) $(BONUS_OBJS)
	@rm -f .count
	@printf "$(RESET)[$(GREEN)DONE$(RESET)] 🎖️ $(BLUE)$(NAME) bonus recruits enlisted.$(RESET)$(CUT)\n"

clean:
	@rm -f $(OBJS) $(BONUS_OBJS) .count

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all bonus clean fclean re
