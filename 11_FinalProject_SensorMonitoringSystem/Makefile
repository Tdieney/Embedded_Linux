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
SS_EXE_FILE := $(BIN_DIR)/sensor
GW_EXE_FILE := $(BIN_DIR)/gateway

# Flags
C_FLAGS := -pthread -lrt -lsqlite3
HDR_INC := -I $(TEST_DIR) -I $(INC_DIR) -I $(ASW_DIR)

# Object files
SS_OBJ_FILES := $(OBJ_DIR)/sensor_lib.o $(OBJ_DIR)/sensor_sim.o $(OBJ_DIR)/common.o
GW_OBJ_FILES := $(OBJ_DIR)/shm_buffer.o $(OBJ_DIR)/log_process.o $(OBJ_DIR)/connection_mgr.o $(OBJ_DIR)/data_mgr.o $(OBJ_DIR)/storage_mgr.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/common.o $(OBJ_DIR)/main.o

# Default target
all: sensor_lib gateway

# Sensor lib build
sensor_lib: $(BUILD_DIR) $(BIN_DIR) $(OBJ_DIR) $(SS_OBJ_FILES)
	$(CC) $(SS_OBJ_FILES) -o $(SS_EXE_FILE)

$(OBJ_DIR)/sensor_lib.o: $(TEST_DIR)/sensor_lib.c
	$(CC) -c $< -o $@ $(HDR_INC)

$(OBJ_DIR)/sensor_sim.o: $(TEST_DIR)/sensor_sim.c
	$(CC) -c $< -o $@ $(HDR_INC)

$(OBJ_DIR)/common.o: $(INC_DIR)/common.c
	$(CC) -c $< -o $@ $(HDR_INC)

# Gateway build
gateway: $(BUILD_DIR) $(BIN_DIR) $(OBJ_DIR) $(GW_OBJ_FILES)
	$(CC) $(GW_OBJ_FILES) -o $(GW_EXE_FILE) $(C_FLAGS)

$(OBJ_DIR)/main.o: $(ASW_DIR)/main.c
	$(CC) -c $< -o $@ $(HDR_INC)

$(OBJ_DIR)/shm_buffer.o: $(ASW_DIR)/shm_buffer.c
	$(CC) -c $< -o $@ $(HDR_INC) $(C_FLAGS)
	
$(OBJ_DIR)/log_process.o: $(ASW_DIR)/log_process.c
	$(CC) -c $< -o $@ $(HDR_INC)

$(OBJ_DIR)/connection_mgr.o: $(ASW_DIR)/connection_mgr.c
	$(CC) -c $< -o $@ $(HDR_INC) $(C_FLAGS)

$(OBJ_DIR)/data_mgr.o: $(ASW_DIR)/data_mgr.c
	$(CC) -c $< -o $@ $(HDR_INC) $(C_FLAGS)

$(OBJ_DIR)/storage_mgr.o: $(ASW_DIR)/storage_mgr.c
	$(CC) -c $< -o $@ $(HDR_INC) $(C_FLAGS)

$(OBJ_DIR)/utils.o: $(ASW_DIR)/utils.c
	$(CC) -c $< -o $@ $(HDR_INC) $(C_FLAGS)

# Ensure directories exist
$(BUILD_DIR):
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@

# Clean
clean:
	rm -rf $(BIN_DIR)/* $(OBJ_DIR)/* *.log *.db