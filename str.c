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
    //delkaop = strlen(ret1) + strlen(ret2);
    
    printf("Delka ret1: %d, ret2: %d\n", (int) strlen(ret1), (int) strlen(ret2));
    printf("ret1: _%s_, ret2: _%s_\n", ret1, ret2);
    
    /*
    int j, k;
     char pom_slovo1[strlen(ret1)-2];
     char pom_slovo2[strlen(ret2)-2];
     
    for(j = 0;j<strlen(ret1)-2;j++) {
		pom_slovo1[j] = ret1[j+1];
	}
	pom_slovo1[j] = '\0';
	 
	 for(k = 0;k<strlen(ret2)-2;k++) {
		pom_slovo2[k] = ret2[k+1];
	}
	pom_slovo2[k] = '\0';
     
     */
    
    //printf("Zmenene ret1: _%s_, ret2: _%s_\n", pom_slovo1,  pom_slovo2);
    //printf("Delka pom_slovo1: %d, pom_slovo2: %d\n", (int) strlen(pom_slovo1), (int) strlen(pom_slovo2));
    
    delkaop = strlen(ret1) + strlen(ret2);
    printf("Delka konk retezce: %d\n", delkaop);
    
    //printf("ret1: %s\n", ret1);
    
    //alokace pro vysledny retezec
    Ret_alokuj(ret3, (delkaop + 3));
    if (ret3 == NULL) {
        printf("chyba mallocu\n");
        return;
    }
    *ret3[0] = '\0';
    
    printf("ret3: %s\n", *ret3);
    
    //samotna konkatenace
    strcat(*ret3, ret1);
    strcat(*ret3, ret2);
    //strcpy((*ret3), strcat(ret1, ret2));
    //strcpy((*ret3), strcat(pom_slovo1, pom_slovo2));
}
