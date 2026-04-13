-include ./source.mk
-include ./libft/source.mk
-include ./ft_printf/source.mk

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

$(OBJS_DIR)/$(LIBFT_DIR)/%.o: $(LIBFT_DIR)/%.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -I $(LIBFT_DIR)/includes -c $< -o $@

$(LIBFT_NAME): $(LIBFT_OBJS_PATH)
	@mkdir -p $(@D)
	@ar rcs $(LIBFT_NAME) $(LIBFT_OBJS_PATH)

$(OBJS_DIR)/$(PRINTF_DIR)/%.o: $(PRINTF_DIR)/%.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -I $(PRINTF_DIR)/includes -c $< -o $@

$(PRINTF_NAME): $(PRINTF_OBJS_PATH)
	@mkdir -p $(@D)
	@ar rcs $(PRINTF_NAME) $(PRINTF_OBJS_PATH)

$(OBJS_DIR)/%.o: %.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(LIBFT_NAME) $(PRINTF_NAME) $(OBJS_PATH)
	@$(CC) $(CFLAGS) $(OBJS_PATH) $(LIBFT_NAME) $(PRINTF_NAME) -o $(NAME)

all: $(NAME)

clean:
	@rm -rf $(OBJS_DIR)

fclean: clean
	@rm -rf $(BUILD_DIR) $(NAME)

re: fclean all

.PHONY: all clean fclean re
