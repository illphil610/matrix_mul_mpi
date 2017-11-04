typedef struct {
        unsigned int rows;
        unsigned int cols;
        int **mat_data;
} matrix_struct;

matrix_struct *get_matrix_struct(char matrix[]);
void free_matrix(matrix_struct *matrix_to_free);
