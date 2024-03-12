#include <stdio.h>
#include <math.h>
//#include "calculations.h"
//#include "liczbapierwsza.h"
//#include "NWD.h"
//#include "NWW.h"
//#include "poleT.h"

int main()
{
    int a = 0, b = 0, c = 0;
    int wynik1 = 0, wynik2 = 0, wynik4 = 0;
    double wynik3 = 0;
    scanf("%d %d %d", &a, &b, &c);

    wynik1 = NWW(a,b);
    wynik2 = NWD(a,b);
    wynik3 = poleT(a,b,c);
    wynik4 = liczbapierwsza(a);
    printf("NWW: %d, NWD: %d, Pole Trojkata: %lf, Liczba pierwsza: %d\n", wynik1, wynik2, wynik3, wynik4);
    //printf("NWW: %d, NWD: %d", wynik1, wynik2);

    return 0;
}
