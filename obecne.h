//  obecne.h

//Chybove kody:
typedef enum {
    EOK,            //vse je v poradku
    ENEZNAMYZNAK,   //pri cteni se objevil neznamy znak
    EOTVSOUBOR,     //chyba pri otevirani souboru
    EZAVSOUBOR,     //chyba pri zavirani souboru
} CHYBKODY;

//Funkce:
void Error(int errcode);
int otevreni(char *nazevsouboru, FILE *soubor);
int zavreni(FILE *soubor);