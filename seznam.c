#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scaner.h"
#include "bvs.h"
#include "interpret.h"
#include "parser.h"
/* SEZNAM pro instrukce */

extern UkTToken token;
char retezec[] = "function";

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

int zmen_data_par(UkTSezPar L, void *ret, int typ){
	
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
int najdi_prvek_lok(UkTSezPar L, char *K){
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

// zkopirovani parametru z lokalniho zasobniku
void kopiruj_parametry(UkTSezPar zas_zpracovani, UkTSezPar zasobnik){
		/*
		if((retezec = malloc((strlen(ZNACKA)+1) * sizeof(char))) == NULL){
				return ERR_INTERNI;
		}
		
		strcpy(retezec,ZNACKA);
		*/
		insert_last(zas_zpracovani, retezec); // vlozeni znackz, ze zde zacina dalsi funkce
		
		set_first(zasobnik);
		while(zasobnik->aktivni != NULL){
			insert_last(zas_zpracovani, zasobnik->aktivni->parametr.klic);
			set_nasl(zasobnik);	
		}
		
}

// zkopirovani promennych z lokalni tabulky symbolu
void kopiruj_promenne(UkTSezPar zas_zpracovani, UkTBSUzel *UkKor) {
	if ((*UkKor)->luk != NULL) { //jestli je neco vlevo, jdeme tam
		kopiruj_promenne(zas_zpracovani, &((*UkKor)->luk));
	}
	if ((*UkKor)->puk != NULL) { //i vpravo
		kopiruj_promenne(zas_zpracovani, &((*UkKor)->puk));
	}
	
	insert_last(zas_zpracovani, (*UkKor)->klic); // zkopirovani klice
	
	switch((*UkKor)->data.typ){
		case TDCISLO:
			zmen_data_par(zas_zpracovani, (double*)&(*UkKor)->data.data.dataCis, (*UkKor)->data.typ);
			break;
		case TDRETEZEC:
			zmen_data_par(zas_zpracovani, (char*)(*UkKor)->data.data.dataRet, (*UkKor)->data.typ);
			break;
		case TDBOOL:
			zmen_data_par(zas_zpracovani, (int*)&(*UkKor)->data.data.dataBool, (*UkKor)->data.typ);
			break;
		case TDNIL:
			zmen_data_par(zas_zpracovani, NULL, (*UkKor)->data.typ);
			break;
	}
	
}

int najdi_prom(UkTSezPar L, char *K, UkTBSPolozka ukazatel){
		UkTPlzkaSezPar PomUk;
		/*
		if((retezec = malloc((strlen(ZNACKA)+1) * sizeof(char))) == NULL){
				return ERR_INTERNI;
		}
		
		strcpy(retezec,ZNACKA);
		*/
		
		if (L == NULL){
			printf("nealokovan zasobnik\n");
			return ERR_INTERNI;
		}
		
		set_first(L);
		
		while(L->aktivni != NULL){
			if(strcmp(L->aktivni->parametr.klic, retezec) == 0){
				PomUk = L->aktivni;
			}
			set_nasl(L);
		}
		
		L->aktivni = PomUk;
		
		while(L->aktivni != NULL){
			if(strcmp(L->aktivni->parametr.klic, K) == 0){
					ukazatel->typ = L->aktivni->parametr.data->typ;
					switch(L->aktivni->parametr.data->typ){
						case TDCISLO:
							ukazatel->data.dataCis = L->aktivni->parametr.data->data.dataCis;
							break;
						case TDRETEZEC:
							if ((ukazatel->data.dataRet = malloc(strlen(L->aktivni->parametr.data->data.dataRet)*sizeof(char)))==NULL){
								return ERR_INTERNI;
							}
							strcpy(ukazatel->data.dataRet, L->aktivni->parametr.data->data.dataRet);
							break;
						case TDBOOL:
							ukazatel->data.dataCis = L->aktivni->parametr.data->data.dataBool;
							break;
						case TDNIL:
							break;
						}
					
					return TRUE;
			}
			
			set_nasl(L);
		}
		
		return FALSE;
}
// vymazani promennych pro jednu instanci funkce
void vymaz_promenne(UkTSezPar L){
	UkTPlzkaSezPar PomUk;
	char *retezec = "function";
	set_first(L);
	while(L->aktivni != NULL){
		if(strcmp(L->aktivni->parametr.klic, retezec) == 0){
				PomUk = L->aktivni;
		}
		set_nasl(L);
	}
	
	L->aktivni = PomUk;
	
	while (L->aktivni != NULL) {
		PomUk = L->aktivni->ukdalsi;
		free(L->aktivni);
		L->aktivni = PomUk;
	}
   
}
/* Funkce pro praci se zasobnikem adres */

// inicializace zasobniku
void zas_adres_in(UkTZasAdr L) {
	L->prvni = NULL;
}

// zruseni celeho zasobniku
void zas_adres_zrus(UkTZasAdr L) {
	UkTPlzkaZas PomUk;
	while (L->prvni != NULL) {
		PomUk = L->prvni->ukdalsi;
		free(L->prvni);
		L->prvni = PomUk;
	}
}

// vlozeni prvku do zasobniku adres
int Push_adr(UkTZasAdr L, UkTInstr adresa) {
	UkTPlzkaZas PomUk;
	if ((PomUk = malloc(sizeof(struct plzkaZas))) == NULL) {
		printf("chyba mallocu\n");
		return ERR_INTERNI;
	}
	
  PomUk->ukdalsi = L->prvni;
  L->prvni = PomUk;
  
  return ERR_OK;
}

// navrat prvni polozky ze seznamu a nasledne odstraneni
/*
UkTInstr Pop_adr(UkTZasAdr L){
	UkTInstr adresa;
	UkTPlzkaZas PomUk;
	
	PomUk = L->prvni;
	adresa = PomUk->adresa;
	L->prvni = PomUk->ukdalsi;
	free(PomUk);
	
	return adresa;
		
}
*/
void Pop_adr(UkTZasAdr L, UkTInstr adresa){
	UkTPlzkaZas PomUk;
	
	PomUk = L->prvni;
	adresa = PomUk->adresa;
	L->prvni = PomUk->ukdalsi;
	free(PomUk);
		
}
