OPEN_CC=gcc
MPICC=mpicc

all: totient totient_omp totient_mpi1 totient_mpi2

totient_mpi1: TotientRange_mpi1.c
	$(MPI_CC) -o totient_mpi1 TotientRange_mpi1.c

totient_mpi2: TotientRange_mpi2.c
	$(MPI_CC) -o totient_mpi2 TotientRange_mpi2.c

totient_omp: TotientRange_omp.c
	$(OPEN_CC) -fopenmp -g -o totient_omp TotientRange_omp.c

totient: TotientRange.c
	$(OPEN_CC) -o totient TotientRange.c

test: totient
	./totient 0 100
	./totient 0 1000
	./totient 5 3000
	./totient 100 10000

.PHONY: clean

clean:
	rm totient totient_mpi1 totient_mpi2 totient_omp
