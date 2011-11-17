#include <stdio.h>
#include <stdlib.h>

#include "interpret.h"

/* SEZNAM pro instrukce */

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
    printf("%d\n", L->posledni->instrukce.typInstr);
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

int main() {
    TSezInstr sez;
    UkTSezInstr seznam;
    seznam = &sez;
    Sez_init(seznam);
    
    int cislo;
    cislo = 5;
    
    TInstr instr1;
    instr1.typInstr = IN_ADD;
    instr1.op1 = (int *) &cislo;
    instr1.op2 = NULL;
    instr1.op3 = NULL;
    
    Sez_vloz(seznam, &instr1);
    int *c = seznam->prvni->instrukce.op1;
    printf("%d\n", *c);
    
    Sez_zrus(seznam);
    
    return 0;
}





/* SEZNAM pro parametry funkce */


// inicializace seznamu
void Sez_init_funkce(UkTSezPar L) {
	L->aktivni = NULL;
	L->prvni = NULL;
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
int insert_last(UkTSezPar L, TToken token) {
	UkTPlzkaSezPar PomUk;
	if ((PomUk = malloc(sizeof(TPlzkaSezPar))) == NULL) {
		printf("chyba mallocu\n");
		return ERR_INTERNI;
	}
	PomUk->ukdalsi = NULL;
	if((pomUk->parametr.klic = malloc(token->delka*sizeof(char)))==NULL) {
		return ERR_INTERNI;
	}
  strcpy(PomUk->parametr.klic, token->data);
  PomUk->parametr.data.typ = TDNIL;
  
  if (L->prvni == NULL) { //seznam je prazdny
      L->prvni = PomUk;
      L->aktivni = PomUk;
  }
  else {
  	while (L->aktivni != NULL){ // pruchodu bude malo, protoze byvame uz na poslednim
  		L->aktivni = L->aktivni->ukdalsi;
  	}
    L->aktivni->ukdalsi = PomUk;
  }
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

void zmen_data_par(UkTSezPar L, TToken token){
	switch(token->typ){
		case RETEZEC: if ((L->aktivni->parametr.data.dataRet = malloc(token->delka*sizeof(char)))==NULL){
										return ERR_INTERNI;
									}
									strcpy(	L->aktivni->parametr.data.dataRet, token->data );
									L->aktivni->parametr.data.typ = TDRETEZEC;
									break;
		case INTKONEC:
    case DESKONEC:	
    case EXPKONEC: 	L->aktivni->parametr.data.typ = TDCISLO;
    								L->aktivni->parametr.data.dataCis = atof(token->data);
    								break;
    case TNTRUE: 	L->aktivni->parametr.data.typ = TDBOOL;
    							L->aktivni->parametr.data.dataBool = TRUE;
    							break;
    case TNFALSE:	L->aktivni->parametr.data.typ = TDBOOL;
    							L->aktivni->parametr.data.dataBool = FALSE;
    							break;
	}
}
int najdi_prvek(UkTSezPar L, char *K){
		set_first(L);
		while(L->aktivni != NULL){
			if(strcmp(L->aktivni->klic, K) == 0){
					return TRUE;
			}
			
			set_nasl(L);
		}
		
		return FALSE;
}
