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
TZasobnikInt zas_int;
TZasobnikUk zas_uk;
UkTBSUzel tab_sym;
UkTBSUzel pom_tab_sym;
UkTBSUzel nazvy_funkci; // kam odkazuje navesti
UkTBSPolozka obsah;
UkTSezInstr seznam_instrukci;
unsigned int klic_cislo=0;
//TPrvek prvek_pomocny;
int typ_instrukce;
UkTBSPolozka op1; 
UkTBSPolozka op2; 
UkTBSPolozka op3;

// ---- nove pridano
UkTBSFunkce strom_funkci;
UkTBSFunkPol uzel_aktualni_funkce;
char *pom_token_data;
char * gen_klic;
// --- konec novyho


UkTBSUzel pole_stromu;
int delka_pole_stromu;

UkTZasAdr zas_navr_adres;
UkTSezPar zas_zpracovani;

UkTBSPolozka *uk_na_zasobnik;


void odalokuj_vse(){
	Sez_zrus(seznam_instrukci);
	Sez_zrus_funkce(uzel_aktualni_funkce->zasobnik);
	//BVSZrus (&uzel_aktualni_funkce->koren);
	token_uvolni(token);
	zasobnik_free(&zas_uk, &zas_int);
	fclose(soubor);
	BVSZrus (&pom_tab_sym);
	BVSFunkceZrus(&strom_funkci);
	zas_adres_zrus(zas_navr_adres);
	Sez_zrus_funkce(zas_zpracovani);
	free(zas_navr_adres);
	free(seznam_instrukci);
	free(zas_zpracovani);
	free(gen_klic);
	if((obsah)->typ == TDRETEZEC){
		free((obsah)->data.dataRet);
	}
	free(obsah);
}




/************************************
 * FUNKCE MAIN CELEHO INTERPRETU ****
 ***********************************/
int main(){
	chyba = ERR_OK;
	if ((soubor = fopen("kod", "r")) == NULL) {
		printf("main: otevreni souboru\n");
		return ERR_INTERNI;
  }
  
	chyba = zasobnik_init(&zas_uk, &zas_int);
	if (chyba!=ERR_OK){
		fclose(soubor);
		return chyba;
	}
	
	seznam_instrukci = malloc(sizeof(struct seznamInstr));
	Sez_init(seznam_instrukci);
	
	//strom_funkci = malloc(sizeof(struct bsfunkce));
	BVSFunkceInit(&strom_funkci);
	BVSInit(&pom_tab_sym);
	delka_pole_stromu=1;
	
	zas_navr_adres = malloc(sizeof(struct zasAdr));
	zas_adres_in(zas_navr_adres);
	
	zas_zpracovani = malloc(sizeof(struct sezPar));
	Sez_init_funkce(zas_zpracovani);
	
	gen_klic = malloc(1);
	
	if ((obsah = malloc(sizeof (TBSPolozka)))==NULL){
		return ERR_INTERNI;
	}
	obsah->typ = TDNIL;
	
	printf("main: druha kontrola\n");
	
	chyba = syntakticky_analyzator();
	
	if (chyba != ERR_OK){
		printf(" ----- VSTUP NEPRIJAT s kodem: %d ------\n",chyba);
		odalokuj_vse();
		return chyba;
	}else{
		printf(" ----- VSTUP PRIJAT ----- \n");
	}
	
	//Interpret(seznam_instrukci);
	chyba = Interpret(seznam_instrukci);
	
		printf("*************Vypis zasobniku****klokan******\n");
	set_first(zas_zpracovani);
	while(zas_zpracovani->aktivni != NULL){
		switch (zas_zpracovani->aktivni->parametr.data->typ){
			case TDCISLO: 	printf("-> ->Klic: %s, dataCis: %f, typ: %d\n", zas_zpracovani->aktivni->parametr.klic, zas_zpracovani->aktivni->parametr.data->data.dataCis, zas_zpracovani->aktivni->parametr.data->typ);
				break;
			case TDRETEZEC:	printf("-> ->Klic: %s, dataRet: %s, typ: %d\n", zas_zpracovani->aktivni->parametr.klic, zas_zpracovani->aktivni->parametr.data->data.dataRet, zas_zpracovani->aktivni->parametr.data->typ);
				break;
			case TDBOOL: printf("-> ->Klic: %s, dataBool: %d, typ: %d\n", zas_zpracovani->aktivni->parametr.klic, zas_zpracovani->aktivni->parametr.data->data.dataBool, zas_zpracovani->aktivni->parametr.data->typ);
				break;
			case TDNIL: printf("-> ->Klic: %s, nil, typ: %d\n", zas_zpracovani->aktivni->parametr.klic, zas_zpracovani->aktivni->parametr.data->typ);

		}
		set_nasl(zas_zpracovani);
	}
	printf("**************************************\n");
	//BVSVypisStrom(&pom_tab_sym);

	odalokuj_vse();
	
	if (chyba != ERR_OK){
		printf(" ----- VSTUP NEPRIJAT s kodem: %d ------\n",chyba);
		return chyba;
	}else{
		printf(" ----- VSTUP PRIJAT ----- \n");
		return ERR_OK;
	}
} 
