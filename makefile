CC=gcc

IDIR=include
ODIR=obj
SRCDIR=src

CFLAGS = -I$(IDIR) -Wall -Wextra -Wpedantic


_DEPS = cli.h pread.h process_info.h t_error.h tracer.h option_t.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = cli.o pread.o process_info.o t_error.o tracer.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tracer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

tracee: $(SRCDIR)/tracee.c
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean clean_tracee

clean:
	rm $(ODIR)/*.o tracer

clean_tracee:
	rm tracee
