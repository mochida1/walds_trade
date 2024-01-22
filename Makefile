NAME = augmenta_test
MAKEFLAGS = --no-print-directory
BUILDDIR = objs
VPATH = src

# headers
# INCLUDES = -I ./headers
INCLUDES = -I .

MAIN = OrderCache.cpp
MAIN += OrderCacheTests.cpp
# MAIN += main.cpp


# Names sources
SOURCES = $(MAIN)

# Names objects
OBJS = $(addprefix $(BUILDDIR)/, $(SOURCES:%.cpp=%.o))

# Compiler
CC = c++
CF = -Wall -Wextra -Werror
CF += -lgtest_main -lgtest 
GDB = -ggdb
GO = ./$(NAME) 
VAL = valgrind --trace-children=no --leak-check=full --track-origins=yes \
		./$(NAME)

$(NAME): $(BUILDDIR) $(OBJS)
	@printf "Compiling $(NAME)\n"
	@$(CC) $(CF) $(OBJS) $(INCLUDES) -o $(NAME)
	@printf "Done!\n"

$(NAME_TEST): $(TEST_OBJS)
	@printf "Compiling test files...\n"
	@$(CC) $(CF) $(TEST_OBJS) $(INCLUDES) -o $(NAME_TEST)

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: %.cpp
	@$(CC) $(CF) $(GDB) $(INCLUDES) -c $< -o $@

all: $(NAME)

test:
	@make -C tests/
	@printf "\n"
	@./tests/tester

clean:
	@rm -rf $(BUILDDIR)
	@rm -f vgcore*

fclean: clean
	@rm -f $(NAME)
	@rm -f vgcore*

re: fclean all

# from here on shit ain't mandatory or bonus
run: all
	$(VAL)

go: all
	$(GO)

fs: $(NAME_FS)
	./$(NAME_FS)

git: fclean
	git add -A
	git commit -m "make git"
	git push

PHONY:	all clean fclean re run
