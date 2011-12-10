#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "bvs.h"
#include "zasobnik.h"
#include "interpret.h"

/*
* Inicializace zasobniku
* @return = chybovy kod
*/
int zasobnik_init (TZasobnik *zas){
    if ((zas->array = malloc(REALLOC_SOUSTO*sizeof(TPrvek)))==NULL){    // k malloc musi byt free !!!
        return ERR_INTERNI;
    }
    zas->velikost = REALLOC_SOUSTO;
    zas->top = -1;
    return ERR_OK;
}

/*
* Vynulovani zasobniku
*/
void zasobnik_vynuluj (TZasobnik *zas){
    zas->top = -1;
}




/*
* Uvolni dynamicky alokovanou pamet
* @return = chybovy kod
*/
void zasobnik_free(TZasobnik *zas){
	free(zas -> array);
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
int zasobnik_pristup_int (TZasobnik *zas, int * hodn, int posun){
    if (zas->top <= 0-posun){
        return ERR_INTERNI;
    }
    *hodn = zas->array[zas->top-posun].typ;
    printf("zasobnik-pristup: %d", (int)hodn);
    return ERR_OK;
}
/*
* Vrati hodnotu na vrcholu zasobniku (+ s posunem)
* == zobecneni ZasobnikTop
* @return = chybovy kod
* *hodn = vraci hodnotu na vrcholu zasobniku
*/
int zasobnik_pristup_uk (TZasobnik *zas, UkTBSPolozka * hodn, int posun){
    if (zas->top <= 0-posun){
        return ERR_INTERNI;
    }
    *hodn = zas->array[zas->top-posun].uk_na_prvek_ts;
    printf("zasobnik-pristup: %d", (int)hodn);
    return ERR_OK;
}


/*
* Vrati hodnotu na vrcholu zasobniku za pouziti fce zasobnik_pristup
* @return = chybovy kod
* *hodn = vraci hodnotu na vrcholu zasobniku
*/
/*
int zasobnik_top (TZasobnik *zas, TPrvek * hodn){
    return zasobnik_pristup(zas, hodn,0);
}*/


