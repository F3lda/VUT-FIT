typedef struct matrix {
	unsigned rows;
	unsigned cols;
	float *data;
} matrix_t;

matrix_t matrix_ctor(unsigned rows, unsigned cols);

void matrix_dtor(matrix_t *matrix);

float matrix_get_item(matrix_t *m, unsigned r, unsigned c);

void matrix_set_item(matrix_t *m, unsigned r, unsigned c, float value);

#define mitem(matrix, row, col) (/* TODO definujte makro-vyraz pro pristup k elementu matice */)