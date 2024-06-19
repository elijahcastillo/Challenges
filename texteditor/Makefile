
CC = gcc 
CFLAGS = -g 

SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build
OBJECT_DIR := $(BUILD_DIR)/obj
EXECUTABLE := ${BUILD_DIR}/editor

SRC_FILES := ${wildcard $(SRC_DIR)/*.c}
#OBJ_FILES := ${wildcard $(OBJECT_DIR)/*.o}
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(OBJECT_DIR)/%.o, $(SRC_FILES))

# Default target
all: $(EXECUTABLE)


# Link objects to create the executable
$(EXECUTABLE): ${OBJ_FILES}
	${CC} ${CFLAGS} -o $@ $^ 

# Compile source files to object files
$(OBJECT_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJECT_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@


# Clean build files
clean:
	rm -rf $(BUILD_DIR)/*

# Phony targets
.PHONY: all clean


