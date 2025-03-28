.PHONY := clean all

CC := gcc
CUR_DIR := .
BIN_DIR := $(CUR_DIR)/bin
SRC_DIR := $(CUR_DIR)/src
EXE_FILE := $(CUR_DIR)/bin/chat_app

all:
	$(CC) -o $(EXE_FILE) $(SRC_DIR)/main.c -pthread

clean:
	rm -rf $(BIN_DIR)/*
