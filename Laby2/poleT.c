#include <stdio.h>
#include <math.h>
double poleT(int a, int b, int c){
    double p = (a+b+c)/2.0;
    double wynik = sqrt(p*(p-a)*(p-b)*(p-c));
    return wynik;
}