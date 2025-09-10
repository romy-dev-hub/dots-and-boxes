# Compiler and flags
CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -std=c99
LDFLAGS = -lraylib -lm -ldl -lpthread -lGL -lX11

# Project name
TARGET = dots-and-boxes

# Directories
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

# Find all .c files in src/
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Convert src/file.c -> obj/file.o
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Default rule
all: $(TARGET)

# Link final executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile each .c into .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure obj/ exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Cleanup
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Run the game
run: all
	./$(TARGET)
