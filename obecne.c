//  obecne.c

#include <stdio.h>
#include "obecne.h"

//chybove hlasky
const char* ErrorMessages[] = {
    "",
    "Spatna posloupnost znaku!",
    "Chyba pri otevirani souboru pro cteni!",
};

//funkce vypisuje chybova hlaseni
void Error(int errcode) {
    printf("Chyba: %s\n", ErrorMessages[errcode]);
}

//otevirani souboru
int otevreni(char *nazevsouboru, FILE *soubor) {
    if ((soubor = fopen(nazevsouboru, "r")) == NULL) {
        return EOTVSOUBOR;
    }
    
    return EOK;
}

//zavreni souboru
int zavreni(FILE *soubor) {
    if (fclose(soubor) == EOF) {
        return EZAVSOUBOR;
    }
    
    return EOK;
}