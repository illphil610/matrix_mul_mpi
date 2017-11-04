PGMS=matrix_mul_mpi

all:	${PGMS}

matrix_mul_mpi:	matrix_mul_mpi.c
	mpicc -o matrix_mul_mpi matrix_mul_mpi.c
clean:
	rm -f ${PGMS}
