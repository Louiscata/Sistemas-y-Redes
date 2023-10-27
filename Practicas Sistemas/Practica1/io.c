#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

int main(){

    FILE *fp;
    int i;
    char a = 'A';

    //fp = fopen ("fichero.in", "a");
    //if (fp == NULL) {fputs ("File error", stderr); exit (1);}
    printf("Preparado\n");
    for (i = 0; i < 100000000; i++) {
        printf("%c", a);
        //fprintf(fp,"%c\n", a);
    }
    //fclose (fp);

    return 0;
}