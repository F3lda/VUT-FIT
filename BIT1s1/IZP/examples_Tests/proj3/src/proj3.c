/****************************************************************************************
*   File:           proj3.c
*   Description:    IZP projekt 3 - Pruchod bludistem
*   Author:         Karel Jirgl
*   Creation date:  2019/12/10
*   Version:        1.0
*   Last update:    2019/12/10
****************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

#define IS_TRIANGLE_NORMAL(r,c) (r % 2 != c % 2)
#define GET_BIT(n,b) ((n >> b) & 1)

enum triangle_sides{LEFT, RIGHT, TOP_BOTTOM};
enum hand_rules{LEFT_HAND, RIGHT_HAND};

typedef struct {
  int rows;
  int cols;
  unsigned char **cells;
} Map;

// scan double from string with error check
int sscand(const char *str, double *num)
{
    char temp_str[2];
    if(sscanf(str, "%lf%1s", num, temp_str) == 1){
        return 1; // only number or number with white-spaces
    }
    return 0; // 2 or 0 or EOF
}

// scan int from string with error check
int sscani(const char *str, int *num)
{
    char temp_str[2];
    if(sscanf(str, "%d%1s", num, temp_str) == 1){
        return 1; // only number or number with white-spaces
    }
    return 0; // 2 or 0 or EOF
}

void help()
{
    printf("|==========|\n");
    printf("|   HELP   |\n");
    printf("|==========|\n");
    printf("./proj3 --help\n");
    printf("   -> prints help\n\n");
    printf("./proj3 --test <file>\n");
    printf("   -> checks if a map in a file is valid or not\n\n");
    printf("./proj3 --rpath <start_pos_row> <start_pos_col> <file>\n");
    printf("   -> finds a path out of the map by right-hand rule\n\n");
    printf("./proj3 --lpath <start_pos_row> <start_pos_col> <file>\n");
    printf("   -> finds a path out of the map by left-hand rule\n\n");
    printf("./proj3 --shortest <start_pos_row> <start_pos_col> <file>\n");
    printf("   -> finds the shortest path out of the map\n\n");
}

int load_map(char *file_name, Map *map)
{
    FILE *file = fopen(file_name,"r");
    if(file == NULL){
        return 1;
    }

    // read map size
    unsigned char uc_temp;
    char c_temp, c_temp2;
    int current_row = 0, current_col = 0, res;
    if(fscanf(file, "%d%c%d%c", &map->rows, &c_temp, &map->cols, &c_temp2) != 4 || c_temp != ' ' || c_temp2 != '\n' || map->rows <= 0 || map->cols <= 0){
        fclose(file);
        return 2;
    }

    // malloc map var
    unsigned char **mapCells;
    mapCells = malloc(map->rows * sizeof(char *));
    if (mapCells == NULL){
        fclose(file);
        return 3;
    }
    int i;
    for(i = 0; i < map->rows; i++){
        mapCells[i] = malloc(map->cols * sizeof(char));
        if (mapCells[i] == NULL){
            for(; i >= 0; i--){
                free(mapCells[i]);
            }
            free(mapCells);
            fclose(file);
            return 4;
        }
    }
    map->cells = mapCells;

    // read map
    int loaded_triangles = 0;
	while((res = fscanf(file, "%c%c", &uc_temp, &c_temp)) != EOF){
        if(uc_temp < '0' || '7' < uc_temp || !(c_temp == ' ' || c_temp == '\n')){
            fclose(file);
            return 5;
        } else if(map->cols <= current_col || map->rows <= current_row){
			fclose(file);
            return 6;
        } else if(res != 2 && getc(file) != EOF){
            fclose(file);
            return 7;
        } else if((res == 2 || res == 1) && '0' <= uc_temp && uc_temp <= '7'){
            loaded_triangles++;
        }

        map->cells[current_row][current_col] = uc_temp - (unsigned char)'0';

        uc_temp = (unsigned char)'\0';
        if(c_temp == '\n'){
            current_row++;
            current_col = 0;
        } else {
            current_col++;
        }
    }
    if(loaded_triangles != map->rows*map->cols) return 9;

    fclose(file);

    return 0;
}

bool test_map(Map *map)
{
    int i, ii;
    for(i = 0; i < map->rows; i++){
        for(ii = 0; ii < map->cols; ii++){
            if(ii+1 < map->cols){// check right side
                if(GET_BIT(map->cells[i][ii], RIGHT) != GET_BIT(map->cells[i][ii+1], LEFT)){
                    //printf("\nERROR [%d][%d] and [%d][%d]\n",i,ii,i,ii+1);
                    return 1;
                }
            }
            if(i+1 < map->rows && IS_TRIANGLE_NORMAL((i+1), (ii+1))){ // check bottom side
                if(GET_BIT(map->cells[i][ii], TOP_BOTTOM) != GET_BIT(map->cells[i+1][ii], TOP_BOTTOM)){
                    //printf("\nERROR [%d][%d] and [%d][%d]\n",i,ii,i+1,ii);
                    return 1;
                }
            }
        }
    }
    return 0;
}

void free_map(Map *map)
{
    if(map == NULL || map->cells == NULL) return;
    int i;
    for(i = 0; i < map->rows; i++){
        free(map->cells[i]);
    }
    free(map->cells);
}

int isBorder(Map *map, int r, int c, int border)
{
    if(map->rows < r || map->cols < c) return 2;
    return GET_BIT(map->cells[r-1][c-1], border);
}

int start_border(Map *map, int r, int c, int leftright)
{
    // check corners
    if(r == 1 && c == 1){// left top
        if(IS_TRIANGLE_NORMAL(r,c)){
            return LEFT;
        } else {
            if(leftright == LEFT_HAND){
                return TOP_BOTTOM;
            } else if(leftright == RIGHT_HAND){
                return LEFT;
            }
        }
    } else if(r == 1 && c == map->cols){// right top
        if(IS_TRIANGLE_NORMAL(r,c)){
            return RIGHT;
        } else {
            if(leftright == LEFT_HAND){
                return RIGHT;
            } else if(leftright == RIGHT_HAND){
                return TOP_BOTTOM;
            }
        }
    } else if(r == map->rows && c == 1){// left bottom
        if(IS_TRIANGLE_NORMAL(r,c)){
            if(leftright == LEFT_HAND){
                return LEFT;
            } else if(leftright == RIGHT_HAND){
                return TOP_BOTTOM;
            }
        } else {
            return LEFT;
        }
    } else if(r == map->rows && c == map->cols){// right bottom
        if(IS_TRIANGLE_NORMAL(r,c)){
            if(leftright == LEFT_HAND){
                return TOP_BOTTOM;
            } else if(leftright == RIGHT_HAND){
                return RIGHT;
            }
        } else {
            return RIGHT;
        }
    } else

    // check sides
    if(c == 1){ // left
        return LEFT;
    } else if(r == map->rows){ // bottom
        if(IS_TRIANGLE_NORMAL(r,c)){
            return TOP_BOTTOM;
        } else {
            if(leftright == LEFT_HAND){
                return RIGHT;
            } else if(leftright == RIGHT_HAND){
                return LEFT;
            }
        }
    } else if(c == map->cols){ // right
        return RIGHT;
    } else if(r == 1){ // top
        if(IS_TRIANGLE_NORMAL(r,c)){
            if(leftright == LEFT_HAND){
                return LEFT;
            } else if(leftright == RIGHT_HAND){
                return RIGHT;
            }
        } else {
            return TOP_BOTTOM;
        }
    }

    return 4;
}

bool map_step(Map *map, int *start_pos_row, int *start_pos_col, int *start_side, int leftright)
{
    int i;
    for(i = 1; i <= 3; i++){
        // set next triangle side by leftright hand
        if(leftright == RIGHT_HAND){
            if(IS_TRIANGLE_NORMAL(*start_pos_row,*start_pos_col)){
                (*start_side)--;
                if(*start_side == -1){
                    *start_side = 2;
                } else if(*start_side == 3){
                    *start_side = 0;
                }
            } else {
                if(*start_side == LEFT){
                    *start_side = RIGHT;
                } else if(*start_side == RIGHT){
                    *start_side = TOP_BOTTOM;
                } else if(*start_side == TOP_BOTTOM){
                    *start_side = LEFT;
                }
            }
        } else if(leftright == LEFT_HAND){
            if(IS_TRIANGLE_NORMAL(*start_pos_row,*start_pos_col)){
                (*start_side)++;
                if(*start_side == -1){
                    *start_side = 2;
                } else if(*start_side == 3){
                    *start_side = 0;
                }
            } else {
                if(*start_side == LEFT){
                    *start_side = TOP_BOTTOM;
                } else if(*start_side == RIGHT){
                    *start_side = LEFT;
                } else if(*start_side == TOP_BOTTOM){
                    *start_side = RIGHT;
                }
            }
        }

        // check if there is no wall
        if(!isBorder(map,*start_pos_row,*start_pos_col,*start_side)){
            if(*start_side == LEFT){
                (*start_pos_col)--;
                *start_side = RIGHT;
            } else if(*start_side == RIGHT){
                (*start_pos_col)++;
                *start_side = LEFT;
            } else if(*start_side == TOP_BOTTOM){
                if(IS_TRIANGLE_NORMAL(*start_pos_row,*start_pos_col)){
                    (*start_pos_row)++;
                } else {
                    (*start_pos_row)--;
                }
            }
            i = 4; // outside loop will be i=5
        }
    }
    // if all side of triangle are walls
    if(i != 5){
        return 1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    int result;
    if(argc == 2 && strcmp(argv[1],"--help") == 0){
        help();
    } else if(argc == 3 && strcmp(argv[1],"--test") == 0){ // soubor.txt
        Map map = {0, 0, NULL};

        result = load_map(argv[2], &map);
        if(result != 0){printf("Invalid file\n"); free_map(&map); return result;}

        result = test_map(&map);
        if(result != 0){printf("Invalid\n"); free_map(&map); return result;}

        free_map(&map);
        printf("Valid\n");
    } else if(argc == 5){
        Map map = {0, 0, NULL};

        result = load_map(argv[4], &map);
        if(result != 0){printf("ERROR: Invalid file\n"); free_map(&map); return result;}

        result = test_map(&map);
        if(result != 0){printf("ERROR: Invalid map\n"); free_map(&map); return result;}

        int start_pos_row, start_pos_col;
        if(!sscani(argv[2], &start_pos_row) || !sscani(argv[3], &start_pos_col)){
            printf("ERROR: start position params should be int numbers\n");
            free_map(&map);
            return 1;
        }
        if(start_pos_row <= 0 || start_pos_col <= 0 || start_pos_row > map.rows || start_pos_col > map.cols){
            printf("ERROR: start position out of map\n");
            free_map(&map);
            return 2;
        }

        if(strcmp(argv[1],"--rpath") == 0){ // R C soubor.txt
            int start_side = start_border(&map, start_pos_row, start_pos_col, RIGHT_HAND);

            while(0 < start_pos_col && start_pos_col <= map.cols && 0 < start_pos_row && start_pos_row <= map.rows){
                printf("%d,%d\n",start_pos_row,start_pos_col);
                
                if(map_step(&map,&start_pos_row,&start_pos_col,&start_side,RIGHT_HAND)){
                    printf("ERROR: solid triangle");
                    free_map(&map);
                    return 3;
                }
            }

        } else if(strcmp(argv[1],"--lpath") == 0){ // R C soubor.txt
            int start_side = start_border(&map, start_pos_row, start_pos_col, LEFT_HAND);

            while(0 < start_pos_col && start_pos_col <= map.cols && 0 < start_pos_row && start_pos_row <= map.rows){
                printf("%d,%d\n",start_pos_row,start_pos_col);
                
                if(map_step(&map,&start_pos_row,&start_pos_col,&start_side,LEFT_HAND)){
                    printf("ERROR: solid triangle");
                    free_map(&map);
                    return 3;
                }
            }
        } else if(strcmp(argv[1],"--shortest") == 0){ // R C soubor.txt
			//TODO shortest path
        } else {
            printf("ERROR: wrong arguments. Try --help");
            return 4;
        }
        
        free_map(&map);
    } else {
        printf("ERROR: wrong arguments. Try --help");
        return 5;
    }

    return 0;
}
