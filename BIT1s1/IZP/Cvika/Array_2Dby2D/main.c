#include <stdio.h>
#include <stdlib.h>

#define m_item(matrix, row, col) (matrix.data[row][col])

typedef struct matrix {
  int rows;
  int cols;
  float **data;
} MATRIX_T;




MATRIX_T matrix_create(int rows, int cols)
{
    MATRIX_T matrix = {0,0,NULL};

    float **matrixData;
    matrixData = malloc(rows * sizeof(float *));
    if (matrixData == NULL){
        return matrix;
    }
    int i;
    for (i = 0; i < rows; i++){
        matrixData[i] = malloc(cols * sizeof(float));
        if (matrixData[i] == NULL){
            for(; i >= 0; i--){
                free(matrixData[i]);
            }
            free(matrixData);
            return matrix;
        }
    }

    matrix.rows = rows;
    matrix.cols = cols;
    matrix.data = matrixData;

    return matrix;
}

void matrix_delete(MATRIX_T *matrix)
{
    if(matrix->data != NULL){
        for (int i = 0; i < matrix->rows; i++)
        {
            free(matrix->data[i]);
        }
        free(matrix->data);

        matrix->rows = 0;
        matrix->cols = 0;
        matrix->data = NULL;
    }
}

void matrix_set_value(MATRIX_T *matrix, int row, int col, float value)
{
    if(matrix->data != NULL && row <= matrix->rows && col <= matrix->cols){
        matrix->data[row][col] = value;
    }
}

float matrix_get_value(MATRIX_T *matrix, int row, int col)
{
    if(matrix->data != NULL && row <= matrix->rows && col <= matrix->cols){
        return matrix->data[row][col];
    } else {
        printf("ERROR: matrix undefined!\n");
        return 0;
    }
}

int main()
{
    MATRIX_T matrix = matrix_create(10,20);
    if (matrix.data == NULL)
        return -1;

    matrix_set_value(&matrix, 4, 5, 12.14);
    printf("<%f>\n",matrix_get_value(&matrix, 4, 5));
    printf("<%f>\n",m_item(matrix, 4, 5));

    matrix_delete(&matrix);

    return 0;
}
