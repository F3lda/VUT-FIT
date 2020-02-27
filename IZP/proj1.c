/****************************************************************************************
*   File:           proj1.c
*   Description:    IZP projekt 1 - Prace s textem
*   Author:         Karel Jirgl
*   Creation date:  2019/11/10
*   Version:        1.1
*   Last update:    2019/11/11
****************************************************************************************/
/* Revision History:
*************************************
** Version: Date:       Description:
** 1.0      2019/11/10  First try
** 1.1      2019/11/11  Debug and finish
*/

// uncomment below for a PREMIUM version
//#define PREMIUM_VERSION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STRING_ARRAY_SIZE 102


//#define DEBUG
#define ERRORs


#ifdef PREMIUM_VERSION
    #define PREMIUM if(1)
#else
    #define PREMIUM if(0)
#endif
#define NON {}else

#ifdef DEBUG
    #define D
#else
    #define D if(0)
#endif

#ifdef ERRORs
    //#define msg(format, ...) fprintf(stderr, "%s:%d:%s(): "format"\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
    #define error(format, ...) fprintf(stderr, format"Process terminated.\n", ##__VA_ARGS__)
#else
    #define error(format, ...) fprintf(stderr, "")
#endif

/****************************************************************************************
* @desc reads chars from input stream
* @param char *buffer - pointer to output char array
* @param int array_size - output char array size; maximum of chars to read -1
* @param FILE *stream - pointer to FILE input stream
* @return int - success or failure or EOF
****************************************************************************************/
int getl(char *buffer, int array_size, FILE *stream) // getline() using fgets()
{
    buffer[0] = '\0'; // initialize or clear the OUTPUT array (not necessary - prevents user from accessing random data)
    buffer[STRING_ARRAY_SIZE-2] = '\0';
    if(fgets(buffer, array_size, stream) == NULL){return EOF;} // read ARRAY_SIZE chars from stream
    if(buffer[STRING_ARRAY_SIZE-2] != '\0'){
        buffer[STRING_ARRAY_SIZE-2] = '\0';
        char ctemp;
        while(fscanf(stream, "%c", &ctemp) == 1 && ctemp != '\n'){} // clear the STREAM buffer by reading it char by char
        return 1; // OVERFLOW
    } else {
        buffer[strcspn(buffer,"\n")] = '\0'; // remove NEWLINE_CHAR
    }
    return 0; // OK
}

/****************************************************************************************
* @desc checks if digits in a string CISLO corresponds to chars found in a string RETEZEC
* @param char *CISLO - searched digits
* @param char *RETEZEC - scanned string
* @return int - boolean true or false
****************************************************************************************/
int validString(char *CISLO, char *RETEZEC)
{
    const char data[10][6] = {
        {"0+"},
        {"1"},
        {"2abc"},
        {"3def"},
        {"4ghi"},
        {"5jkl"},
        {"6mno"},
        {"7pqrs"},
        {"8tuv"},
        {"9wxyz"}
    };
    D printf("---------------\n");
    int i, j = 0, k = 0, lenCISLO = strlen(CISLO), lenRETEZEC = strlen(RETEZEC);
    for(i = 0; i < lenRETEZEC; i++){
        if('0' <= CISLO[j] && CISLO[j] <= '9' && '0' <= CISLO[k] && CISLO[k] <= '9'){// check if an input char is a digit
            if(strchr(data[CISLO[j] - '0'], tolower(RETEZEC[i])) != NULL){ // check if actual char is equal to [j] number
                D printf("ZNAK [%d]<%c> nalezen\n", j, RETEZEC[i]);
                j++;
            } else if(strchr(data[CISLO[0] - '0'], tolower(RETEZEC[i])) != NULL){ // check if actual char is equal to [0] number
                D printf("ZNAK [%d] nenalezen   ->\nZNAK [0]<%c> nalezen  <-\n", j, RETEZEC[i]);
                j = 1;
            } else { // reset search
                D printf("ZNAK [%d]<%c> nenalezen\n", j, RETEZEC[i]);
                j = 0;
            }
            if(strchr(data[CISLO[k] - '0'], tolower(RETEZEC[i])) != NULL){ // premium search -> check if actual char is equal to [k] number
                D printf("ZNAK PREM [%d]<%c> [%d] nalezen\n", k, RETEZEC[i], i);
                k++;
            }
            if(lenCISLO == j){
                D printf("RETEZEC Nalezen <Normal>\n");
                return 1;
            }
            if(lenCISLO == k){
                D printf("RETEZEC Nalezen <Premium>\n");
                PREMIUM return 1;   // uncomment for a PREMIUM
                PREMIUM NON k = 0;   // comment for a PREMIUM
            }
        } else {
            error("Unexpected char on input <CISLO>[%d][%d] - [%c][%c]!\n", j, k, CISLO[j], CISLO[k]);
            //return -1;
        }
    }
    return 0;
}

/****************************************************************************************
* @desc phone contacts search
* @param char *argv[1] - searched number
* @return int - success or failure
****************************************************************************************/
int main(int argc, char *argv[])
{
    char JMENO[STRING_ARRAY_SIZE], TELCISLO[STRING_ARRAY_SIZE];
    int resultJMENO = -1, resultTELCISLO = -1, line = 0, found = 0;
    while(!((resultJMENO = getl(JMENO, sizeof(JMENO), stdin)) == (resultTELCISLO = getl(TELCISLO, sizeof(TELCISLO), stdin)) && resultJMENO == EOF)){
        // check errors
        line++;
        if(resultJMENO == 1){
            error("ERROR<file_line:%d>: input line <JMENO> is longer than 100 chars!\n", line); return 1;
        } else if(resultJMENO == EOF) {
            error("ERROR<file_line:%d>: reading line!\n", line); return 1;
        }
        line++;
        if(resultTELCISLO == 1){
            error("ERROR<file_line:%d>: input line <TELCISLO> is longer than 100 chars!\n", line); return 1;
        } else if(resultTELCISLO == EOF) {
            error("ERROR<file_line:%d>: reading line!\n", line); return 1;
        }
        // check errors - end

        if(argc < 2 || (validString(argv[1], JMENO) || validString(argv[1], TELCISLO))){ // check if a contact can be printed
            printf("%s, %s\n", JMENO, TELCISLO);
            found++;
        }
    }

    if(line == 0){
        printf("Contact list is EMPTY!\n");
    } else if(found == 0 && argc > 1){
        printf("Not found\n");
    }

    return 0;
}
