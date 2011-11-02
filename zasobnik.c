#define REALLOC_SOUSTO 2 // TODO: zmenit na vetsi pred odevzdanim


typedef struct stTZasobnik {
    int top;                //vrchol zasobniku
    int velikost;           //velikost alokovaneho prostoru
    int * array;						//data zasobniku
} TZasobnik;


/*
 * Inicializace zasobniku
 * @return = chybovy kod
 */
int zasobnik_init (TZasobnik *zas){
	if ((zas->array = malloc(REALLOC_SOUSTO*sizeof(int)))==NULL){
		return ERR_INTERNI;
	}
	zas->velikost = REALLOC_SOUSTO;
	zas->top = -1; 
	return ERR_OK;
}

/*
 * Vyjme hodnotu z vrcholu zasobniku a (!)nevrati
 * @return = chybovy kod
 */
int zasobnik_pop (TZasobnik *zas){
	if (zas->top>=0){
		zas->top--;
	}
	return ERR_OK;
}

/*
 * Vlozi hodnotu na vrcholu zasobniku a posune ho
 * @return = chybovy kod
 */
int zasobnik_push (TZasobnik *zas, int hodn){
	if (zas->top == zas->velikost-1){
		if((zas->array=realloc(zas->array, zas->velikost+(REALLOC_SOUSTO * sizeof(int))))==NULL){
			return ERR_INTERNI;
		}
		zas->velikost += REALLOC_SOUSTO;
	}
	zas->top++;
	zas->array[zas->top] = hodn;
	return ERR_OK;
}


/*
 * Vrati hodnotu na vrcholu zasobniku za pouziti fce zasobnik_pristup
 * @return = chybovy kod
 * *hodn = vraci hodnotu na vrcholu zasobniku
 */
int zasobnik_top (TZasobnik *zas, int * hodn){
	return zasobnik_pristup(zas, hodn,0);
}

/*
 * Vrati hodnotu na vrcholu zasobniku (+ s posunem)
 * == zobecneni ZasobnikTop
 * @return = chybovy kod
 * *hodn = vraci hodnotu na vrcholu zasobniku
 */
int zasobnik_pristup (TZasobnik *zas, int * hodn, int posun=0){
	if (zas->top < 0-posun){
		return ERR_INTERNI;
	}
	*hodn = zas->array[zas->top-posun];
	return ERR_OK;
}
