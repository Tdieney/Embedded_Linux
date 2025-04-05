.PHONY := clean all

CC := gcc
CUR_DIR := .
BIN_DIR := $(CUR_DIR)/bin
SRC_DIR := $(CUR_DIR)/src
CLI_FILE := $(CUR_DIR)/bin/client
SER_FILE := $(CUR_DIR)/bin/server

all:
	$(CC) -o $(CLI_FILE) $(SRC_DIR)/client.c
	$(CC) -o $(SER_FILE) $(SRC_DIR)/server.c

clean:
	rm -rf $(BIN_DIR)/*
