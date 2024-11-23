# manic miner

YEAR = $(shell date +'%Y')

TARGET = manicminer

CC = gcc

CFLAGS = -pedantic -Wall -Wextra -g -MMD -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE
LDFLAGS = -L/opt/homebrew/lib -lSDL2

SRC = src
O = linux

OBJS = $(O)/main.o $(O)/system.o $(O)/video.o $(O)/loader.o $(O)/title.o $(O)/audio.o $(O)/miner.o $(O)/levels.o $(O)/game.o $(O)/portal.o $(O)/trans.o $(O)/gameover.o $(O)/robots.o $(O)/victory.o $(O)/cheat.o $(O)/die.o $(O)/spg.o $(O)/misc.o

all:	dir $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(O)/loader.o:	$(SRC)/loader.c
	$(CC) $(CFLAGS) -DBUILD=\"v1.0.$(YEAR)\" -c $< -o $@

$(O)/%.o:	$(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(O) $(TARGET)

install: all
	cp $(TARGET) ~/.local/bin/

uninstall:
	rm ~/.local/bin/$(TARGET)

dir:
	@mkdir -p $(O)

-include $(O)/*.d

# manic miner
