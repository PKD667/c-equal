OUT = bin/ce

CC = gcc

ODIR = obj
SDIR = src

CFLAGS = -Wall


SRCS = $(wildcard $(SDIR)/*.c)
OBJS = $(patsubst %,$(ODIR)/%,$(notdir $(SRCS:.c=.o)))

all: $(OUT)

$(OBJS): $(SRCS)
	$(CC) -c $(INC) -o $@  $(subst .o,.c,$(subst ${ODIR},${SDIR},$@)) $(CFLAGS) 

$(OUT): $(OBJS) 
	$(CC) -o $@  $(OBJS) $(CFLAGS) 

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(LIBOUT)
install:
	cp $(LIBOUT) $(DESTDIR)/lib
	cp $(EXEOUT) $(DESTDIR)/bin