/****************************************************************************************
*   File:           proj2.c
*   Description:    IZP projekt 2 - Iteracni vypocty
*   Author:         Karel Jirgl
*   Creation date:  2019/11/27
*   Version:        1.2
*   Last update:    2019/11/28
****************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define ERRORs

#ifdef ERRORs
    #define error(format, ...) fprintf(stderr, format, ##__VA_ARGS__)
#else
    #define error(format, ...) fprintf(stderr, "")
#endif

// scan double from string with error check
int sscand(const char *str, double *num)
{
    char temp_str[2];
    if(sscanf(str, "%lf%1s", num, temp_str) == 1){
        return 1; // only number or number with white-spaces
    }
    return 0; // 2 or 0 or EOF
}

// check and load all input arguments
int checkInput(int argc, char **argv, double *U0, double *R, double *eps)
{
    // check argument count
    if(argc != 4){
        error("error: invalid arguments%c", '\n'); // ERROR: wrong count of arguments! Correct is [3].
        return -1;
    }

    // check if voltage is INF
    if(strlen(argv[1]) == 3){
        int i;
        char temp_str[4] = "inf";
        for(i = 0; i < 3; i++){
            if(tolower(argv[1][i]) != temp_str[i]){
                i = 4;
            }
        }
        if(i == 3){
            printf("Up=inf V\nIp=inf A\n");
            return 0;
        }
    }

    // check if inputs are numbers & check if numbers are correct
    if(!sscand(argv[1], U0) || !sscand(argv[2], R) || !sscand(argv[3], eps) || (*U0) <= 0 || (*R) <= 0 || (*eps) <= 0){
        error("error: invalid arguments%c", '\n');
        return -1;
    }
    return 1;
}

double diode(double U0, double R, double eps)
{
    const double I0 = 1e-12, Ut = 0.0258563;
    double a = 0, b = U0, x = (a+b)/2, x_old; // Up

    do{
        if((I0 * (exp(x / Ut) - 1.0)) - ((U0 - x) / R) > 0){// PROUD_V_DIODE - PROUD_V_REZISTORU > 0 - posunujeme pravy bod (b) do leva
            b = x;
        } else {// PROUD_V_DIODE - PROUD_V_REZISTORU <= 0 - posunujeme levy bod (a) do prava
            a = x;
        }
        x_old = x;
        x = (a+b)/2;
    }while(fabs(x_old - x) > eps);

    return x; // Up
}

int main(int argc, char **argv)
{
    double U0, R, eps;
    double Up, Ip;

    int res = checkInput(argc, argv, &U0, &R, &eps);
    if(res != 1){return -res;}

    Up = diode(U0, R, eps);
    Ip = (U0 - Up) / R;

    printf("Up=%g V\nIp=%g A\n", Up, Ip);

    return 0;
}
