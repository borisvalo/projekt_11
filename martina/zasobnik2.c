#define REALLOC_SOUSTO 4 // TODO: zmenit na vetsi pred odevzdanim
                         // ZMENIT ZPATKY NA 2 - KVULI TESTOVANI ALOKACE

                         // FUNKCE NA FREE ZASOBNIKU



#include <stddef.h>
#include <stdlib.h>


// jestli to tam bude mit Pavel nebo co :-D
typedef enum {

    ERR_OK = 333,   // = bez chyb    // ZVOLIT TAM NEJAKE CISLO, TREBA 333 :-D
    ERR_INTERNI,    // chyba pri praci se zasobnikem
    ERR_SYNTAX,     // syntakticka chyba
    ERR_SEMANT      // semanticka chyba

} chybove_stavy;


typedef struct stTPrvek {
    int typ;   // typ tokenu
    UkTBSPolozka uk_na_prvek_ts;
} TPrvek;

typedef struct stTZasobnik {
    int top; //vrchol zasobniku
    int velikost; //velikost alokovaneho prostoru
    TPrvek * array; //data zasobniku
} TZasobnik;


/*
* Inicializace zasobniku
* @return = chybovy kod
*/
int zasobnik_init (TZasobnik *zas){
    if ((zas->array = malloc(REALLOC_SOUSTO*sizeof(TPrvek)))==NULL){    // k malloc musi byt free !!!
        return ERR_INTERNI;
    }
    printf(">>>>>>>>>>>>>Pro me: sizeof(int) je %d, REALLOC_SOUSTO JE %d\n", sizeof(int), REALLOC_SOUSTO);
    zas->velikost = REALLOC_SOUSTO;
    zas->top = -1;
    return ERR_OK;
}

/*
* Vyjme hodnotu z vrcholu zasobniku a (!)nevrati
* @return = chybovy kod
*/
int zasobnik_pop (TZasobnik *zas){
    if(zas->top>=0){
        zas->top--;
    }
    return ERR_OK;
}

/*
* Vlozi hodnotu na vrcholu zasobniku a posune ho
* @return = chybovy kod
*/
int zasobnik_push (TZasobnik *zas, TPrvek prvek){
    if(zas->top == zas->velikost-1) {
        if((zas->array = realloc(zas->array, ((zas->velikost + REALLOC_SOUSTO) * sizeof(TPrvek)))) == NULL) {       // k malloc musi byt free !!!
            zasobnik_free(zas);
            return ERR_INTERNI;
        }
        zas->velikost += REALLOC_SOUSTO;
        printf(">>>Probehla realokace<<<\n");
        printf(">>>>>>>>>>>>>Pro me: sizeof(int) je %d, REALLOC_SOUSTO JE %d, zas->velikost je %d\n", sizeof(TPrvek), REALLOC_SOUSTO, zas->velikost);
    }
    zas->top++;
    zas->array[zas->top] = prvek;
    return ERR_OK;
}


/*
* Vrati hodnotu na vrcholu zasobniku (+ s posunem)
* == zobecneni ZasobnikTop
* @return = chybovy kod
* *hodn = vraci hodnotu na vrcholu zasobniku
*/
int zasobnik_pristup (TZasobnik *zas, TPrvek * hodn, int posun){
    if (zas->top < 0-posun){
        return ERR_INTERNI;
    }
    *hodn = zas->array[zas->top-posun];
    return ERR_OK;
}


/*
* Vrati hodnotu na vrcholu zasobniku za pouziti fce zasobnik_pristup
* @return = chybovy kod
* *hodn = vraci hodnotu na vrcholu zasobniku
*/
int zasobnik_top (TZasobnik *zas, TPrvek * hodn){
    return zasobnik_pristup(zas, hodn,0);
}

/*
* Uvolni dynamicky alokovanou pamet
* @return = chybovy kod
*/
int zasobnik_free(TZasobnik *zas){
	free(zas -> array);
}


