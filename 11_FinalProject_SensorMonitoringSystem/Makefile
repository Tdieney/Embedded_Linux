.PHONY = clean all sensor_lib gateway

# Compiler and flags
CC := gcc

# Directories
CUR_DIR := .
BUILD_DIR := $(CUR_DIR)/_builds
BIN_DIR := $(CUR_DIR)/_builds/bin
OBJ_DIR := $(CUR_DIR)/_builds/obj
ASW_DIR := $(CUR_DIR)/asw
TEST_DIR := $(CUR_DIR)/test
INC_DIR := $(CUR_DIR)/include

# Output executables
NODE_EXE_FILE := $(BIN_DIR)/node
GW_EXE_FILE := $(BIN_DIR)/gateway

# Includes
HDR_INC := -I $(TEST_DIR) -I $(INC_DIR) -I $(ASW_DIR)

# Object files
OBJ_FILES := $(OBJ_DIR)/sensor_lib.o $(OBJ_DIR)/sensor_sim.o $(OBJ_DIR)/common.o

# Default target
all: sensor_lib gateway

# Sensor lib build
sensor_lib: $(BUILD_DIR) $(BIN_DIR) $(OBJ_DIR) $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(NODE_EXE_FILE)

$(OBJ_DIR)/sensor_lib.o: $(TEST_DIR)/sensor_lib.c
	$(CC) -c $< -o $@ $(HDR_INC)

$(OBJ_DIR)/sensor_sim.o: $(TEST_DIR)/sensor_sim.c
	$(CC) -c $< -o $@ $(HDR_INC)

$(OBJ_DIR)/common.o: $(INC_DIR)/common.c
	$(CC) -c $< -o $@ $(HDR_INC)

# Gateway build
gateway: $(BUILD_DIR) $(BIN_DIR)
	$(CC) $(OBJ_DIR)/common.o -o $(GW_EXE_FILE) $(ASW_DIR)/main.c $(HDR_INC)

# Ensure directories exist
$(BUILD_DIR):
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@

# Clean
clean:
	rm -rf $(BIN_DIR)/*
	rm -rf $(OBJ_DIR)/*
