CC = gcc

CFLAGS = -Wall -pedantic -std=gnu99

SRC_DIR = src
INC_DIR = include

SRC_FILES = \
$(SRC_DIR)/main.c \
$(SRC_DIR)/init_shell.c \
$(SRC_DIR)/signal_handler.c \
$(SRC_DIR)/command_parser.c \
$(SRC_DIR)/pipe_handler.c \
$(SRC_DIR)/hist_handler.c

all: $(SRC_FILES)
	$(CC) $(CFLAGS) -o CIS3110sh $(SRC_FILES)

clean:
	rm -f CIS3110sh
