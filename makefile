# Compiler
CC=gcc


# Log output level
MACROS=-D INFO_ENABLE -D WARN_ENABLE

# Compiler options
OPTIONS=-std=gnu11 -ggdb
WARNINGS_FILE=warnings
OPTIMIZATIONS=-O1

ifeq ($(CC), gcc)
	CFLAGS = -I$(IDIR) $(OPTIONS) $(OPTIMIZATIONS) @$(WARNINGS_FILE) $(MACROS)
endif

ifeq ($(CC), clang)
	CFLAGS = -I$(IDIR) $(OPTIONS) $(OPTIMIZATIONS) -Weverything $(MACROS)
endif

# Directories
IDIR=include
ODIR=obj
SRCDIR=src
TARGET_DIR=target

# Options regarding code instrumentation
ASAN_OPTIONS=detect_invalid_pointer_pairs=2

TRACEE_TARGET_STEM=tracee

# Modules ending in .c
MODULES = $(filter-out $(TRACEE_TARGET_STEM).c, $(notdir $(wildcard $(SRCDIR)/*.c)))
# Source file list (ex. src/tracer.c)
SRC = $(patsubst %,$(SRCDIR)/%,$(MODULES))
# Dependency list - .h files in include directory
DEPS = $(wildcard $(IDIR)/*.h)
# Object list - .o files
OBJ = $(patsubst %,$(ODIR)/%,$(MODULES:.c=.o))


$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tracer: $(OBJ)
	$(CC) -o $(TARGET_DIR)/$@ $^ $(CFLAGS)

tracer_asan: $(SRC)
	$(CC) -o $(TARGET_DIR)/$@ $^ $(CFLAGS) -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract

tracer_ubsan: $(SRC)
	$(CC) -o $(TARGET_DIR)/$@ $^ $(CFLAGS) -fsanitize=undefined

tracee: $(SRCDIR)/$(TRACEE_TARGET_STEM).c
	$(CC) -o $(TARGET_DIR)/$@ -ggdb -fverbose-asm $^

all: tracer tracer_asan tracer_ubsan tracee

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(TARGET_DIR)/*

clean_tracee:
	rm -f tracee
