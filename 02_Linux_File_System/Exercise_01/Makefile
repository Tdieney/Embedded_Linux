.PHONY := clean all

CC := gcc
CUR_DIR := .
BIN_DIR := $(CUR_DIR)/bin
SRC_DIR := $(CUR_DIR)/src
EXE_FILE := $(CUR_DIR)/bin/append_seek

all:
	$(CC) -o $(EXE_FILE) $(SRC_DIR)/main.c

clean:
	rm -rf $(EXE_FILE)
	rm -rf *.txt
