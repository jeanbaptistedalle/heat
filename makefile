CC=gcc
CFLAGS=-g
LDFLAGS=-lm
EXEC=heat.exe

all: $(EXEC)

heat.exe: main.o
	$(CC) -o $@ $^ $(LDFLAGS)

main.o: main.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
