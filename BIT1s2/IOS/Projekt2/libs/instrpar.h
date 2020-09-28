/**
 * @file instrpar.h
 * 
 * @brief Library for parsing string input from streams
 * @date 2020-05-06
 * @author F3lda
 */
#ifndef INSTRPAR_H_
#define INSTRPAR_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// scan double from string with error check
int sscand(const char *str, double *num);

// scan int from string with error check
int sscani(const char *str, int *num);

#ifndef INSTRPAR_FGETL_C

/****************************************************************************************
* @desc reads chars from input stream
* @param char *buffer - pointer to output char array
* @param int array_size - output char array size; maximum of chars to read -1
* @param FILE *stream - pointer to FILE input stream
* @return int - success or failure or EOF
****************************************************************************************/
int fgetl(char *buffer, int array_size, FILE *stream); // getline() using fgets()

#else

int fgetl(char *buffer, int array_size, FILE *stream); // getline() using fscanf("%c")

#endif

#endif