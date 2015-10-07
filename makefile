CC=gcc
CFLAGS=-g -O3
LDFLAGS=-lm -O3
EXEC=heat.exe

all: $(EXEC)

heat.exe: main.o
	$(CC) -o $@ $^ $(LDFLAGS)

main.o: main.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o *.exe

mrproper: clean
	rm -rf $(EXEC)
