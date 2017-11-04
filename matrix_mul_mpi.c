#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/*
  This program utilizes MPI to calculate the product of two matricies.  The main
  objective is to send the entire matrix B and slices of rows from matrix A.  After
  the calculation is performed, the calculated result matrix will be displayed
  for the user in a presentable format.  At this moment the matrices have to be
  hardcoded due to debugging issues with reading from a file.  Will be updated in
  the next patch.

  Written by Philip Cappelli
*/

#define numRowsMatrixA 5
#define numColsMatrixA 5
#define numColsMatrixB 5
#define FROM_MASTER 1
#define FROM_WORKER 2
#define MASTER 0

int main (int argc, char *argv[]) {

    int numtasks, taskid, numworkers,       // run-time variables
        source, dest, messageType,          // message info variables
        rows, averageRow, extra, offset,    // matrix info variables
        i, j, k;                            // loop variables

    // Declare matricies for calculations
    int a[numRowsMatrixA][numColsMatrixA];
    int b[numColsMatrixA][numColsMatrixB];
    int c[numRowsMatrixA][numColsMatrixB];

    // Initialize MPI, then get the size and rank
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    // Root is not considered a slave in this situation
    numworkers = numtasks - 1;

    // Master
    if (taskid == MASTER) {
        int a[numRowsMatrixA][numColsMatrixA] =
          { {1, 2, 3, 4, 5},
            {6, 7, 8, 9, 10},
            {11, 12, 13, 14, 15},
            {16, 17, 18, 19, 20},
            {21, 22, 23, 24, 25} };

        int b[numColsMatrixA][numColsMatrixB] =
          { {1, 0, 0, 0, 0},
            {0, 1, 0, 0, 0},
            {0, 0, 1, 0, 0},
            {0, 0, 0, 1, 0},
            {0, 0, 0, 0, 1} };

        // Send matrix data to the worker tasks
        averageRow = numRowsMatrixA / numworkers;
        extra = numRowsMatrixA % numworkers;
        messageType = FROM_MASTER;
        offset = 0;
        for (dest = 1; dest <= numworkers; dest++) {
            rows = (dest <= extra) ? averageRow + 1 : averageRow;
            MPI_Send(&offset, 1, MPI_INT, dest, messageType, MPI_COMM_WORLD);
            MPI_Send(&rows, 1, MPI_INT, dest, messageType, MPI_COMM_WORLD);
            MPI_Send(&a[offset][0], rows * numColsMatrixA, MPI_DOUBLE, dest, messageType, MPI_COMM_WORLD);
            MPI_Send(&b, numColsMatrixA * numColsMatrixB, MPI_DOUBLE, dest, messageType, MPI_COMM_WORLD);
            offset = offset + rows;
        }

        // Receive results from worker tasks
        messageType = FROM_WORKER;
        for (source = 1; source <= numworkers; source++) {
            MPI_Recv(&offset, 1, MPI_INT, source, messageType, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, source, messageType, MPI_COMM_WORLD, &status);
            MPI_Recv(&c[offset][0], rows * numColsMatrixB, MPI_DOUBLE, source, messageType, MPI_COMM_WORLD, &status);
        }

        printf("Finalized Matrix:\n");
        for (i = 0; i < numRowsMatrixA; i++) {
              for (j = 0; j < numColsMatrixB; j++) {
                  printf("%d \t", c[i][j]);
              }
              printf("\n");
        }
    }

    // Workers
    if (taskid > MASTER) {

        // Receive data from the master for calculation
        messageType = FROM_MASTER;
        MPI_Recv(&offset, 1, MPI_INT, MASTER, messageType, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, MASTER, messageType, MPI_COMM_WORLD, &status);
        MPI_Recv(&a, rows * numColsMatrixA, MPI_DOUBLE, MASTER, messageType, MPI_COMM_WORLD, &status);
        MPI_Recv(&b, numColsMatrixA * numColsMatrixB, MPI_DOUBLE, MASTER, messageType, MPI_COMM_WORLD, &status);

        // #pragma omp parallel for
        for (k = 0; k < numColsMatrixB; k++) {
            for (i = 0; i < rows; i++) {
                c[i][k] = 0;
                for (j = 0; j < numColsMatrixA; j++) {
                    c[i][k] = c[i][k] + a[i][j] * b[j][k];
                }
            }
        }

        // Send the calculated data to the master to present results
        messageType = FROM_WORKER;
        MPI_Send(&offset, 1, MPI_INT, MASTER, messageType, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, MASTER, messageType, MPI_COMM_WORLD);
        MPI_Send(&c, rows*numColsMatrixB, MPI_DOUBLE, MASTER, messageType, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    exit(EXIT_SUCCESS);
}
