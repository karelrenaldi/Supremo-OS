#include "math.h"

int mod(int a, int m){
    while(a >= m){
        a -= m;
    }

    return a;
}

int div(int a, int m){
    int res = 0;

    while(res*m <=a){
        res++;
    }

    return res - 1;
}