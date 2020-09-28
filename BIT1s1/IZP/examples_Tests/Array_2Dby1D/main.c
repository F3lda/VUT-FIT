#include <stdlib.h>
#include <stdio.h>
#include "matrix.h"

int main(int argc, char * argv[]){
    int row_num = 5;
    int col_num = 5;

    if(argc != 3){
        fprintf(stderr, "spatny pocet parametru <%d>\n",argc);
        return 1;
    }



    row_num = atoi(argv[1]);
    col_num = atoi(argv[2]);
    //printf("DEBUG ROW: %i COL:%i\n", row_num, col_num);

	matrix_t matrix = matrix_ctor(row_num, col_num);

	float hodnota = 0;
	for (int it = 0; it < row_num; it++){
		for(int jt = 0; jt < col_num; jt++){
			matrix_set_item(&matrix, it, jt, hodnota);
			hodnota++;  		
		}
	}

    //vem hodnotu ze 4 radku a 3 sloupce
    for (int it = 0; it < row_num; it++){
		for(int jt = 0; jt < col_num; jt++){
	        printf(" %f", matrix_get_item(&matrix, it, jt));
        }
        printf("\n");
    }

    matrix_dtor(&matrix); //destrukce pole
    return 0;
}