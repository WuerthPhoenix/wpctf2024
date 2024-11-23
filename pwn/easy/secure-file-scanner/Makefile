CC = gcc
CFLAGS=-Wall -s -no-pie -fstack-protector-all -D_FORTIFY_SOURCE=2
LDFLAGS=-Wl,-z,now -Wl,-z,relro -lcrypto
SRC_DIR = src
OBJ_DIR = obj
EXEC = scanner

# find all the .o files in the src directory
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Default target
all: $(EXEC)

# Actually build the executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Create the object directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Build files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INC_FLAGS) -c $< -o $@


# clean
clean:
	rm -f $(OBJ_DIR)/*.o $(EXEC)

.PHONY: all clean
