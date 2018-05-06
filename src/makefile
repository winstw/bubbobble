IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR) -lglut -lGL -lGLU -lm -Wall -std=c99

ODIR=obj
LDIR =../lib



_DEPS = entity.h graphics.h map.h tga_small.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = entity.o graphics.o map.o tga_small.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) 

../bubbob: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
