#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "scaner.h"
#include "zasobnik.h"
#include "str.h"
#include "parser.h"
#include "bvs.h"
#include "interpret.h"
/*
 * Autor: Pavel Slabý, xslaby00
 * 				a další
 * Datum odevzdání: 11.12.2011
 */
 



UkTToken token;
FILE *soubor;
int chyba;
TZasobnik zasobnik;
UkTBSUzel tab_sym;
UkTBSUzel pom_tab_sym;
UkTBSUzel nazvy_funkci; // kam odkazuje navesti
UkTBSPolozka obsah;
UkTSezInstr seznam_instrukci;
unsigned int klic_cislo=0;
TPrvek prvek_pomocny;
int typ_instrukce;
void *op1; 
void *op2; 
void *op3;

// ---- nove pridano
UkTBSFunkce strom_funkci;
UkTBSFunkPol uzel_aktualni_funkce;
char *pom_token_data;
// --- konec novyho


UkTBSUzel pole_stromu;
int delka_pole_stromu;

UkTSezZal zaloha_stromy;
int main(){
	
	
	printf("main: prvni kontrola\n");
	chyba = ERR_OK;
	if ((soubor = fopen("kod", "r")) == NULL) {
		printf("main: otevreni souboru\n");
		return ERR_INTERNI;
  }
	//token = NULL;
	chyba = zasobnik_init( &zasobnik);
	if (chyba!=ERR_OK){
	//TODO: zavrit soubor
	// odalokovat token
		return chyba;
	}
	
	seznam_instrukci = malloc(sizeof(struct seznamInstr));
	Sez_init(seznam_instrukci);
	//seznam_instrukci = malloc(sizeof(struct bsfunkce));
	//Sez_init_funkce(strom_funkci);
	delka_pole_stromu=1;
	
	if ((obsah = malloc(sizeof (TBSPolozka)))==NULL){
		return ERR_INTERNI;
	}
	
	if ((zaloha_stromy = malloc(sizeof(struct sezZal)))==NULL){
		return ERR_INTERNI;
	}
	
	printf("main: druha kontrola\n");
	Sez_init_zaloha(zaloha_stromy);
	
	chyba = syntakticky_analyzator();
	
	Sez_zrus(seznam_instrukci);
	
	Sez_zrus_funkce(uzel_aktualni_funkce->zasobnik);
	
	BVSZrus (&uzel_aktualni_funkce->koren);
	
	token_uvolni(token);
	zasobnik_free(&zasobnik);
	fclose(soubor);
	
	if (chyba != ERR_OK){
		printf(" ----- VSTUP NEPRIJAT s kodem: %d ------\n",chyba);
		return chyba;
	}else{
		printf(" ----- VSTUP PRIJAT ----- \n");
		return ERR_OK;
	}
} 
