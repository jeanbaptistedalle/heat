CC=gcc
CFLAGS= -g -O3 -fopenmp
LDFLAGS= -g -lm -O3 -fopenmp
EXEC=heat.exe

all: $(EXEC)

heat.exe: main.o
	$(CC) -o $@ $^ $(LDFLAGS)

main.o: main.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf *.o *.bmp $(EXEC)
