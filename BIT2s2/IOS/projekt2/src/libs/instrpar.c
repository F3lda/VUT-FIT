/**
 * @file instrpar.c
 * 
 * @brief Library for parsing string input from streams
 * @date 2020-05-06
 * @author F3lda
 */
#include "instrpar.h"


// scan double from string with error check
int sscand(const char *str, double *num)
{
    char temp_str[2];
	int result;
    if((result = sscanf(str, "%lf%1s", num, temp_str)) == 1){
        return 0; // OK - only number or number with white-spaces
    }
    return result; // ERROR - 2 or 0 or EOF
}


// scan int from string with error check
int sscani(const char *str, int *num)
{
    char temp_str[2];
	int result;
    if((result = sscanf(str, "%d%1s", num, temp_str)) == 1){
        return 0; // OK - only number or number with white-spaces
    }
    return result; // ERROR - 2 or 0 or EOF
}


#ifndef INSTRPAR_FGETL_C

/****************************************************************************************
* @desc reads chars from input stream
* @param char *buffer - pointer to output char array
* @param int array_size - output char array size; maximum of chars to read -1
* @param FILE *stream - pointer to FILE input stream
* @return int - success or failure or EOF
****************************************************************************************/
int fgetl(char *buffer, int array_size, FILE *stream) // getline() using fgets()
{
    if(buffer == NULL){
        return -1;
    }
    if(array_size <= 1){
        return -2;
    }
    if(stream == NULL){
        return -3;
    }
    buffer[0] = '\0'; // initialize or clear array (not necessary - prevents printing random chars)
    if(fgets(buffer, array_size, stream) == NULL){ // read ARRAY_SIZE chars from stream
        return -4;
    }
    int lastCharIndex = strlen(buffer)-1; // this index should never go -1 because of newline char
    if(lastCharIndex >= 0){
        if(buffer[lastCharIndex] != '\n'){ // check if newline char was read
            char ctemp;
            if(fscanf(stream, "%c", &ctemp) == 1 && ctemp != '\n'){ // check if only newline char stayed in STDIN buffer
                while(fscanf(stream, "%c", &ctemp) == 1 && ctemp != '\n'){} // clear STDIN buffer by reading char by char
                //char stemp[array_size]; stemp[0] = '\0';
                //while(fgets(stemp, array_size, stream) != NULL && strchr(stemp, '\n') == NULL){} // clear STDIN buffer by reading with array buffer
                return 1; // overflow
            } else {
                return 0; // OK
            }
        } else {
            buffer[lastCharIndex] = '\0'; // delete newline char
            return 0; // OK
        }
    } else {
        return -5; // index error
    }
    return 0;
}

#else

int fgetl(char *buffer, int array_size, FILE *stream) // getline() using fscanf("%c")
{
    if(buffer == NULL){return -1;}
    if(array_size <= 1){return -2;}
    if(stream == NULL){return -3;}
    buffer[0] = '\0';
    char ctemp = '\0';
    int i = 0, result = 0;
    while((result = fscanf(stream, "%c", &ctemp)) == 1 && ctemp != '\n' && i < array_size-1){buffer[i] = ctemp; i++; buffer[i] = '\0';}
    if(result != 1){if(result == EOF) return -4; else if(result == 0) return -5; else return -6;}
    if(i == array_size-1 && ctemp != '\n'){i = array_size;}
    while(ctemp != '\n' && fscanf(stream, "%c", &ctemp) == 1){}
    return i == array_size;
}

#endif
