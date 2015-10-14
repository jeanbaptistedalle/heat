CC=gcc
CFLAGS= -g -std=c99
LDFLAGS= -g -lm
EXEC=heat.exe
EXEC_OP=heat_openmp.exe
EXEC_MPI=heat_mpi.exe
EXEC_MPI_OP=heat_mpiop.exe

all: $(EXEC) $(EXEC_OP) $(EXEC_MPI) $(EXEC_MPI_OP)

heat.exe: main.o
	$(CC) -o $@ $^ $(LDFLAGS)

main.o: main.c
	$(CC) -o $@ -c $< $(CFLAGS)

heat_openmp.exe: main_openmp.o
	$(CC) -o $@ $^ $(LDFLAGS) -fopenmp

main_openmp.o: main.c
	$(CC) -o $@ -c $< $(CFLAGS) -fopenmp
	
heat_mpi.exe: main_mpi.o
	mpicc -o $@ $^ $(LDFLAGS)
	
main_mpi.o: main_mpi.c
	mpicc -o $@ -c $< $(CFLAGS)	
	
heat_mpiop.exe: main_mpiop.o
	mpicc -o $@ $^ $(LDFLAGS) -fopenmp
	
main_mpiop.o: main_mpi.c
	mpicc -o $@ -c $< $(CFLAGS) -fopenmp

clean:
	rm -rf *.o

mrproper: clean
	rm -rf *.o *.bmp $(EXEC) $(EXEC_OP) $(EXEC_MPI)
