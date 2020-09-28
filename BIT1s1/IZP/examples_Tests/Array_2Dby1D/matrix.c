#include <stdlib.h>
#include <math.h>
#include "matrix.h"

matrix_t matrix_ctor(unsigned rows, unsigned cols) {
	// implementujte funkci inicializace matice

	matrix_t m = {0, 0, NULL};

	m.data = malloc(sizeof(float)*rows*cols);
	if(m.data != NULL){
		m.rows = rows;
		m.cols = cols;
	}

	return m;
}

void matrix_dtor(matrix_t *matrix) {
	// implementujte funkci pro dealokaci matice

	if(matrix->data != NULL){
		free(matrix->data);
	}
	matrix->data = NULL;
	matrix->rows = 0;
	matrix->cols = 0;
}

float matrix_get_item(matrix_t *m, unsigned r, unsigned c) {
	// implementujte funkci pro ziskani hodnoty prvku matice

	float ret = NAN;

	if(r < m->rows && c < m->cols){
		ret = m->data[r*m->cols + c];
	}

	return ret;
}

void matrix_set_item(matrix_t *m, unsigned r, unsigned c, float value) {
	// implementujte funkci nastaveni hodnoty matice

	if(r < m->rows && c < m->cols){
		m->data[r*m->cols + c] = value;
	}
}