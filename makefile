CC=gcc

MACROS=-D INFO_ENABLE -D WARN_ENABLE -D DEBUG_ENABLE -D TRACE_ENABLE
OPTIONS=-std=gnu11 -ggdb
WARNINGS_FILE=warnings
IDIR=include
ODIR=obj
SRCDIR=src

CFLAGS = -I$(IDIR) $(OPTIONS) @$(WARNINGS_FILE) $(MACROS)
TRACEE_TARGET_STEM=tracee

SRC = $(filter-out $(TRACEE_TARGET_STEM).c, $(notdir $(wildcard $(SRCDIR)/*.c)))
DEPS = $(wildcard $(IDIR)/*.h)
OBJ = $(patsubst %,$(ODIR)/%,$(SRC:.c=.o))


$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tracer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

tracee: $(SRCDIR)/$(TRACEE_TARGET_STEM).c
	$(CC) -o $@ $^ $(CFLAGS) -fverbose-asm

.PHONY: clean clean_tracee

clean:
	rm -f $(ODIR)/*.o tracer

clean_tracee:
	rm -f tracee
