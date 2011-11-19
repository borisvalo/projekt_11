#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scaner.h"
#include "interpret.h"
#include "bvs.h"
#include "parser.h"
/* SEZNAM pro instrukce */

extern UkTToken token;
// inicializace seznamu
void Sez_init(UkTSezInstr L) {
	L->aktivni = NULL;
	L->prvni = NULL;
  L->posledni = NULL;
}

// zruseni celeho seznamu
void Sez_zrus(UkTSezInstr L) {
	UkTPlzkaSez PomUk;
	while (L->prvni != NULL) {
		PomUk = L->prvni->ukdalsi;
		free(L->prvni);
		L->prvni = PomUk;
	}
    L->aktivni = NULL;
}

// vlozeni prvku do seznamu
void Sez_vloz(UkTSezInstr L, UkTInstr instr) {
	UkTPlzkaSez PomUk;
	if ((PomUk = malloc(sizeof(TPlzkaSez))) == NULL) {
		printf("chyba mallocu\n");
		return;
	}
	PomUk->ukdalsi = NULL;
    PomUk->instrukce.typInstr = instr->typInstr;
    PomUk->instrukce.op1 = instr->op1;
    PomUk->instrukce.op2 = instr->op2;
    PomUk->instrukce.op3 = instr->op3;
    if (L->prvni == NULL) { //seznam je prazdny
        L->prvni = PomUk;
    }
    else {
        L->posledni->ukdalsi = PomUk;
    }
    
    L->posledni = PomUk;
}

// nastaveni aktivity na prvni prvek
void Sez_prvni(UkTSezInstr L) {
    L->aktivni = L->prvni;
}

// vraci odkaz na strukturu instrukce
void *Sez_hodnota_aktivniho(UkTSezInstr L) {
    if (L->aktivni == NULL) {
        printf("chybka - seznam neni aktivni..\n");
        return NULL;
    }
    
	return &(L->aktivni->instrukce);
}

// nastavi aktivitu na dalsi prvek
void Sez_dalsi(UkTSezInstr L) {
    if (L->aktivni != NULL) {
        L->aktivni = L->aktivni->ukdalsi;
    }
}

// nastaveni aktivity na konkretni instrukci
void Sez_nastav_aktivni(UkTSezInstr L, UkTPlzkaSez instrukce) {
    L->aktivni = instrukce;
}

//vrati ukazatel na posledni prvek
void *Sez_vrat_uk_posledni(UkTSezInstr L) {
		return (void *) L->posledni;
}

/* SEZNAM pro parametry funkce */


// inicializace seznamu
void Sez_init_funkce(UkTSezPar L) {
	L->aktivni = NULL;
	L->prvni = NULL;
	L->posledni = NULL;
}


// zruseni celeho seznamu
void Sez_zrus_funkce(UkTSezPar L) {
	UkTPlzkaSezPar PomUk;
	while (L->prvni != NULL) {
		PomUk = L->prvni->ukdalsi;
		free(L->prvni);
		L->prvni = PomUk;
	}
    L->aktivni = NULL;
}

// vlozeni prvku do seznamu
int insert_last(UkTSezPar L, char *ret) {
	UkTPlzkaSezPar PomUk;
	if ((PomUk = malloc(sizeof(struct plzkaSezPar))) == NULL) {
		printf("chyba mallocu\n");
		return ERR_INTERNI;
	}
	if((PomUk->parametr.data = malloc (sizeof(struct bsdata) ))==NULL){
		return ERR_INTERNI;
	}
	PomUk->ukdalsi = NULL;
	if((PomUk->parametr.klic = malloc((strlen(ret)+1)*sizeof(char)))==NULL) {
		return ERR_INTERNI;
	}
	printf("%s\n", ret);
  strcpy(PomUk->parametr.klic, ret);
  PomUk->parametr.data->typ = TDNIL;

  if (L->prvni == NULL) { //seznam je prazdny
      L->prvni = PomUk;
      L->posledni = PomUk;
      printf("mozna\n");
  }
  else {
	L->posledni->ukdalsi = PomUk;
	L->posledni = L->posledni->ukdalsi;
  }
  
  return ERR_OK;
}

// nastaveni aktivity na prvni prvek
void set_first(UkTSezPar L) {
    L->aktivni = L->prvni;
}
// nastavi aktivitu na dalsi prvek
void set_nasl(UkTSezPar L) {
    if (L->aktivni != NULL) {
        L->aktivni = L->aktivni->ukdalsi;
    }
}
// vraci odkaz na strukturu instrukce
void *hodnota_aktivniho(UkTSezPar L) {
  if (L->aktivni == NULL) {
      printf("chybka - seznam neni aktivni..\n");
      return NULL;
  }
  
	return &(L->aktivni->parametr);
}

int zmen_data_par(UkTSezPar L, char *ret, int typ){
	
	switch(typ){
		case RETEZEC: if ((L->posledni->parametr.data->data.dataRet = malloc(strlen(ret)*sizeof(char)))==NULL){
										return ERR_INTERNI;
									}
									strcpy(	L->posledni->parametr.data->data.dataRet, ret );
									L->posledni->parametr.data->typ = TDRETEZEC;
									break;
		case INTKONEC:
    case DESKONEC:	
    case EXPKONEC: 	L->posledni->parametr.data->typ = TDCISLO;
    								L->posledni->parametr.data->data.dataCis = atof(ret);
    								break;
    case TNTRUE: 	L->posledni->parametr.data->typ = TDBOOL;
    							L->posledni->parametr.data->data.dataBool = TRUE;
    							break;
    case TNFALSE:	L->posledni->parametr.data->typ = TDBOOL;
    							L->posledni->parametr.data->data.dataBool = FALSE;
    							break;
	}
	
	return ERR_OK;
}
int najdi_prvek(UkTSezPar L, char *K){
		if (L==NULL){
			printf("nealokovan zasobnik\n");
			return ERR_INTERNI;
		}
		set_first(L);
		while(L->aktivni != NULL){
			if(strcmp(L->aktivni->parametr.klic, K) == 0){
					return TRUE;
			}
			
			set_nasl(L);
		}
		
		return FALSE;
}
