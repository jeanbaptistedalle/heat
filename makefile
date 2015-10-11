CC=gcc
CFLAGS= -g -O3
LDFLAGS= -g -lm -O3
EXEC=heat.exe
EXEC_OP=heat_openmp.exe

all: $(EXEC) $(EXEC_OP)

heat.exe: main.o
	$(CC) -o $@ $^ $(LDFLAGS)

main.o: main.c
	$(CC) -o $@ -c $< $(CFLAGS)

heat_openmp.exe: main_openmp.o
	$(CC) -o $@ $^ $(LDFLAGS) -fopenmp

main_openmp.o: main.c
	$(CC) -o $@ -c $< $(CFLAGS) -fopenmp

clean:
	rm -rf *.o

mrproper: clean
	rm -rf *.o *.bmp $(EXEC)
