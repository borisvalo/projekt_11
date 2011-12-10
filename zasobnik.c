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
int zasobnik_init (TZasobnikUk *zas_uk, TZasobnikInt *zas_int){
    if ((zas_uk->array = malloc(REALLOC_SOUSTO*sizeof(UkTBSPolozka)))==NULL){    // k malloc musi byt free !!!
        return ERR_INTERNI;
    }
    zas_uk->velikost = REALLOC_SOUSTO;
    zas_uk->top = -1;
    
    if ((zas_int->array = malloc(REALLOC_SOUSTO*sizeof(int)))==NULL){    // k malloc musi byt free !!!
        return ERR_INTERNI;
    }
    zas_int->velikost = REALLOC_SOUSTO;
    zas_int->top = -1;
    
    return ERR_OK;
}

/*
* Vynulovani zasobniku
*/
void zasobnik_vynuluj (TZasobnikUk *zas_uk, TZasobnikInt *zas_int){
    zas_uk->top = -1;
    zas_int->top = -1;
}




/*
* Uvolni dynamicky alokovanou pamet
* @return = chybovy kod
*/
void zasobnik_free(TZasobnikUk *zas_uk, TZasobnikInt *zas_int){
	free(zas_uk -> array);
	free(zas_int -> array);
}


/*
* Vyjme hodnotu z vrcholu zasobniku a (!)nevrati
* @return = chybovy kod
*/
int zasobnik_pop (TZasobnikUk *zas_uk, TZasobnikInt *zas_int){
    if(zas_uk->top>=0){
        zas_uk->top--;
        zas_int->top--;
    }
    return ERR_OK;
}

/*
* Vlozi hodnotu na vrcholu zasobniku a posune ho
* @return = chybovy kod
*/
int zasobnik_push (TZasobnikUk *zas_uk, int hodnota, TZasobnikInt *zas_int, UkTBSPolozka ukazatel_do_ts){
    if(zas_uk->top == zas_uk->velikost-1) {
        if((zas_uk->array = realloc(zas_uk->array, ((zas_uk->velikost + REALLOC_SOUSTO) * sizeof(UkTBSPolozka)))) == NULL) {       // k malloc musi byt free !!!
            zasobnik_free(zas_uk, zas_int);
            return ERR_INTERNI;
        }
        zas_uk->velikost += REALLOC_SOUSTO;
    }
    zas_uk->top++;
    zas_uk->array[zas_uk->top] = ukazatel_do_ts;
    
    if(zas_int->top == zas_int->velikost-1) {
        if((zas_int->array = realloc(zas_int->array, ((zas_int->velikost + REALLOC_SOUSTO) * sizeof(int)))) == NULL) {       // k malloc musi byt free !!!
            zasobnik_free(zas_uk, zas_int);
            return ERR_INTERNI;
        }
        zas_int->velikost += REALLOC_SOUSTO;
    }
    zas_int->top++;
    zas_int->array[zas_int->top] = hodnota;
    
    return ERR_OK;
}


/*
* Vrati hodnotu na vrcholu zasobniku (+ s posunem)
* == zobecneni ZasobnikTop
* @return = chybovy kod
* *hodn = vraci hodnotu na vrcholu zasobniku
*/
int zasobnik_pristup_int (TZasobnikInt *zas_int, int * hodn, int posun){
    if (zas_int->top <= 0-posun){
        return ERR_INTERNI;
    }
    *hodn = zas_int->array[zas_int->top-posun];
    //printf("zasobnik-pristup: %d", (int)hodn);
    return ERR_OK;
}
/*
* Vrati hodnotu na vrcholu zasobniku (+ s posunem)
* == zobecneni ZasobnikTop
* @return = chybovy kod
* *hodn = vraci hodnotu na vrcholu zasobniku
*/
int zasobnik_pristup_uk (TZasobnikUk *zas_uk, UkTBSPolozka * hodn, int posun){
    if (zas_uk->top <= 0-posun){
        return ERR_INTERNI;
    }
    *hodn = zas_uk->array[zas_uk->top-posun];
    //printf("zasobnik-pristup: %d", (int)hodn);
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


