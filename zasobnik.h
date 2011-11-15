

#define REALLOC_SOUSTO 4 // TODO: zmenit na vetsi pred odevzdanim
                         // ZMENIT ZPATKY NA 2 - KVULI TESTOVANI ALOKACE

typedef struct stTPrvek {
	int typ;   // typ tokenu
	//UkTBSPolozka uk_na_prvek_ts;
	struct bsdata *uk_na_prvek_ts;
} TPrvek;

typedef struct stTZasobnik {
    int top; //vrchol zasobniku
    int velikost; //velikost alokovaneho prostoru
    TPrvek * array; //data zasobniku
} TZasobnik;


int zasobnik_init (TZasobnik *zas);
void zasobnik_free(TZasobnik *zas);
int zasobnik_pop (TZasobnik *zas);
int zasobnik_push (TZasobnik *zas, TPrvek prvek);
int zasobnik_pristup (TZasobnik *zas, TPrvek * hodn, int posun);
int zasobnik_top (TZasobnik *zas, TPrvek * hodn);
void zasobnik_vynuluj (TZasobnik *zas);
