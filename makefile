#The target binary
BINARY = bin/cc

#The target library
LIB_DIR = lib

# The directories containing source files
SRC_DIR = src

# The directories containing headers files
INC_DIR = include

# The directory for object files
OBJ_DIR = obj

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
all: libs $(BINARY)

libs:
	for i in $(LOCAL_LIBS); do make -C $$(dirname $$i) all; done

# Link the objects into the binary
$(BINARY): $(OBJECTS) $(LIBRARY)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINARY) $(OBJECTS) $(LIBS)

# Build the object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean up
clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(BINARY)
