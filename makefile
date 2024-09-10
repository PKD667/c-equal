
#The target library
LIB_DIR = lib

# The directories containing source files
SRC_DIR = src

# The directories containing headers files
INC_DIR = include

# The directory for object files
OBJ_DIR = obj

# The directory for storing binary executables
BIN_DIR = bin

#The target binary
BINARY = $(BIN_DIR)/cc

# The C compiler to use
CC = gcc

# The C flags to use
CFLAGS = -I$(INC_DIR)

LOCAL_LIBS = $(wildcard $(LIB_DIR)/*/*.a)

LIBS = $(LOCAL_LIBS)

# The source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)

# The object files
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# The default target
all: dirs libs $(BINARY)

libs:
	for i in $(LOCAL_LIBS); do make -C $$(dirname $$i) all; done

dirs:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)

# Link the objects into the binary
$(BINARY): $(OBJECTS) $(LIBRARY)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARY) $(OBJECTS) $(LIBS)

# Build the object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

test:
	$(CC) $(CFLAGS) -o $(BIN_DIR)/test tests/main.c $(SRC_DIR)/{parse.c,parseutils.c,tokenize.c,hashtable.c,pattern.c} $(LIBS)
	$(BIN_DIR)/test


# Clean up
clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(BINARY)
