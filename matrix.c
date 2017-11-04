#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "matrix.h"

matrix_struct *get_matrix_struct(char matrix[]) {

        // Create a struct to hold the matrix values and set row/column count to 0.
        matrix_struct *m = malloc(sizeof(matrix_struct));
        m->rows = 0;
        m->cols = 0;

        // Open the specified file given in the parameter
        FILE* matrix_file = fopen(matrix, "r");

        // Check if the file exists
        if (matrix_file == NULL) {
                printf("Error: The file you entered could not be found.\n");
                exit(EXIT_FAILURE);
        }

         // get the rows and columns
         int charFromFile = 0;
         do {
                //  Retrieve each individual char from the file
                charFromFile = fgetc(matrix_file);

                // To count the number of columns, read the amount of tabs on the first row.
                // This wil require us to increment cols + 1 since the amount of tabs is colums - 1.
                if (m->rows == 0 && charFromFile == '\t')
                m->cols++;

                 // count the rows with "\n"
                if (charFromFile == '\n')
                m->rows++;
         } while (charFromFile != EOF);

        // Increment columns by 1 to get accurate colums amount.  Specified in a previous comment.
        m->cols++;

        // allocate memory for matrix
        m->mat_data = calloc(m->rows, sizeof(int*));
        int i;
        for (i = 0; i < m->rows; ++i) {
                m->mat_data[i]=calloc(m->cols, sizeof(int));
        }

        // go back to the beginning of the file
        rewind(matrix_file);

        // fill matrix with data from file
        int x, y;
        for (x = 0; x < m->rows; x++) {
                for (y = 0; y < m->cols; y++) {
                        if (!fscanf(matrix_file, "%d", &m->mat_data[x][y]))
                                break;
                }
        }

        // Close the file and return the matrix struct
        fclose(matrix_file);
        return m;
}

void free_matrix(matrix_struct *freedom_for_this_matrix) {
        int i;
        for (i = 0; i < freedom_for_this_matrix ->rows; i++) {
                free(freedom_for_this_matrix->mat_data[i]);
        }
        free(freedom_for_this_matrix->mat_data);
        free(freedom_for_this_matrix);
}
