#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "str.h"

//alokace pameti pro retezec
void Ret_alokuj(TRetezec *ret, int pocet) {
	if(ret == NULL){
			printf("doslo k velmi zavazne chybe ze ktere se jen tak nedostaneme \n");
			return;
	}

    if (((*ret) = (TRetezec) malloc(pocet * sizeof(char))) == NULL) {
        printf("chyba malloc!!\n");
    }

}

//ulozeni retezce
void Ret_vloz(TRetezec *ret, TRetezec novy) {
    strcpy((*ret), novy);
}

//uvolneni pameti alokovane pro retezec
void Ret_uvolni(TRetezec ret) {
    free(ret);
}

//realokace pameti na potrebnou delku
void Ret_realokuj(TRetezec *ret, int delka) {
    if (((*ret) = realloc((*ret), delka)) == NULL) {
        printf("chyba realokace!!\n");
    }
}

//konkatenace dvou retezcu
//posledni parametr bude vysledny retezec (jedna se o ukazatel)
void Ret_konkatenace(TRetezec ret1, TRetezec ret2, TRetezec *ret3) {
    //ziskame soucet delky operatoru
    int delkaop;
    delkaop = strlen(ret1) + strlen(ret2);
    
    //alokace pro vysledny retezec
    Ret_alokuj(ret3, (delkaop + 1));
    if (ret3 == NULL) {
        printf("chyba mallocu\n");
        return;
    }
    
    //samotna konkatenace
    strcpy((*ret3), strcat(ret1, ret2));
}
