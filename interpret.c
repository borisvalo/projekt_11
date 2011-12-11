/*
 *   Nazev projektu: Implementace interpretu imperativniho jazyka IFJ11
 * 
 *   Autori:   	Boris Valo, xvalob00
 * 				Pavel Slaby, xslaby00
 * 				Ondrej Vohanka, xvohan00
 * 				Matej Stepanek, xstepa43
 * 				Martina Stodolova, xstodo04 
 * 
 *   Datum odevzdani: 11.12.2011
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "interpret.h"

#include "str.h"
#include "bvs.h"
#include "scaner.h"
#include "parser.h"
#include "vfce.h"
//#include "ial.c"

extern UkTToken token;
extern UkTZasAdr zas_navr_adres; // zasobnik navratovych adres
extern UkTSezPar zas_zpracovani; // globalni zasobnik promennych
extern UkTPlzkaSez adresa_konce;
extern UkTBSFunkPol uzel_aktualni_funkce;
extern UkTBSPolozka obsah;

extern UkTBSUzel pom_tab_sym; //jenom na zkousku..
UkTBSPolozka pom_substr;

int Vloz_instrukci(UkTSezInstr seznam, int typ, void *op1, void *op2, void *op3) {
    TInstr polozka;
    printf("VLOZINSTRUKCI!!!!!!!!\n");
    polozka.typInstr = typ;
    polozka.op1 = op1;
    polozka.op2 = op2;
    polozka.op3 = op3;
    
    Sez_vloz(seznam, &polozka);
    
    return KONEC_OK;
}

int Interpret(UkTSezInstr list) {
    Sez_prvni(list);
    UkTInstr ukinstr;

	while(list->aktivni != NULL) {
		printf("Cislo instrukce je %d\n", list->aktivni->instrukce.typInstr);
		list->aktivni = list->aktivni->ukdalsi;
	}
	Sez_nastav_aktivni(list, uzel_aktualni_funkce->adresa);
    while (1) {
        ukinstr = Sez_hodnota_aktivniho(list); //nahrajeme aktivni instrukci
        
        printf("_____ Instrukce cislo: %d _____\n", ukinstr->typInstr);
        //printf("Adresa aktualni instrukce: %d\n", Sez_hodnota_aktivniho(list));
        switch (ukinstr->typInstr) { //automat
            //cteni
            case IN_READ:
                
								printf("------------------------vkladam retezec: %s\n", ((UkTBSPolozka)ukinstr->op1)->data.dataRet);	
								//read((UkTBSPolozka)ukinstr->op1, &(ukinstr->op3->data));
                
                if (( strcmp( ((UkTBSPolozka)ukinstr->op1)->data.dataRet, "*n")==0 ) || (strcmp( ((UkTBSPolozka)ukinstr->op1)->data.dataRet, "*l")==0) || (strcmp( ((UkTBSPolozka)ukinstr->op1)->data.dataRet, "*a")==0)){
            	     ((UkTBSPolozka)ukinstr->op1)->typ = TDRETEZEC;           
                }else{
	        	     ((UkTBSPolozka)ukinstr->op1)->typ = TDCISLO; 
                	int cislo;
                	cislo = atoi( ((UkTBSPolozka)ukinstr->op1)->data.dataRet );
                	free( ((UkTBSPolozka)ukinstr->op1)->data.dataRet);
                	((UkTBSPolozka)ukinstr->op1)->data.dataCis = cislo;
                }
                
                
                
                read((UkTBSPolozka)ukinstr->op1, ukinstr->op3);
                break;
                
            //vypis
            case IN_WRITE:
                write((UkTBSPolozka)ukinstr->op3);
                break;
                
            //soucet
            case IN_ADD:
				printf("SCITAME\n");
				printf("Co je sakra za typ: %d -- %d\n", ((UkTBSPolozka)ukinstr->op1)->typ, ((UkTBSPolozka)ukinstr->op2)->typ);
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
                    ((UkTBSPolozka)ukinstr->op3)->data.dataCis = ((UkTBSPolozka)ukinstr->op1)->data.dataCis + ((UkTBSPolozka)ukinstr->op2)->data.dataCis;
                    ((UkTBSPolozka)ukinstr->op3)->typ = TDCISLO;
                    printf("%d - %d - %d",(int)ukinstr->op1, (int)ukinstr->op2, (int)ukinstr->op3);
                    printf("Co vysledek: %f\n", ((UkTBSPolozka)ukinstr->op3)->data.dataCis);
                    BVSVypisStrom(&pom_tab_sym);
                    //printf("Cislo adresy: %d\n", ((int)(UkTBSPolozka)ukinstr->op3));
                }
                else {
                    printf("semanticka chyba - typy nejsou cisla\n");
                    return ERR_SEMANT;
                }
                break;
            
            //odcitani
            case IN_SUB:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
                    ((UkTBSPolozka)ukinstr->op3)->data.dataCis = ((UkTBSPolozka)ukinstr->op1)->data.dataCis - ((UkTBSPolozka)ukinstr->op2)->data.dataCis;
                    ((UkTBSPolozka)ukinstr->op3)->typ = TDCISLO;
                }
                else {
                    printf("semanticka chyba - spatne typy odecitani\n");
                    return ERR_SEMANT;
                }
                break;
            
            //nasobeni
            case IN_MUL:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
                    ((UkTBSPolozka)ukinstr->op3)->data.dataCis = ((UkTBSPolozka)ukinstr->op1)->data.dataCis * ((UkTBSPolozka)ukinstr->op2)->data.dataCis;
                    ((UkTBSPolozka)ukinstr->op3)->typ = TDCISLO;
                }
                else {
                    printf("semanticka chyba - spatne typy nasobeni\n");
                    return ERR_SEMANT;
                }
                break;
                
            //deleni
            case IN_DIV:
				printf("Deleni: typ op2 - %d\n",  ((UkTBSPolozka)ukinstr->op2)->typ);
				printf("Deleni: typ op1 - %d\n",  ((UkTBSPolozka)ukinstr->op1)->typ);
				
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
                    if (((UkTBSPolozka)ukinstr->op2)->data.dataCis == 0) {
                        printf("chyba: deleni nulou!!\n");
                        return ERR_INTERPRET;
                    }
                    ((UkTBSPolozka)ukinstr->op3)->data.dataCis = ((UkTBSPolozka)ukinstr->op1)->data.dataCis / ((UkTBSPolozka)ukinstr->op2)->data.dataCis;
                    ((UkTBSPolozka)ukinstr->op3)->typ = TDCISLO;
                }
                else {
                    printf("semanticka chyba - spatne typy deleni\n");
                    return ERR_SEMANT;
                }
                break;
                
            //mocnina
            case IN_MOCN:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
                    ((UkTBSPolozka)ukinstr->op3)->data.dataCis = pow(((UkTBSPolozka)ukinstr->op1)->data.dataCis, ((UkTBSPolozka)ukinstr->op2)->data.dataCis);
                    ((UkTBSPolozka)ukinstr->op3)->typ = TDCISLO;
                }
                else {
                    printf("semanticka chyba\n");
                    return ERR_SEMANT;
                }
                break;
            
            //navesti
            case IN_NVSTI:
                //ukinstr->op1 = list->aktivni;
                break;
                
            //goto - nastavi aktivitu na konkretni prvek
            case IN_GOTO:
				//Sez_nastav_aktivni(list, (UkTPlzkaSez)ukinstr->op1);
				if(ukinstr->op1 != NULL){
					Sez_nastav_aktivni(list, (UkTPlzkaSez)ukinstr->op1);
				}else{
					//printf("adresa konce u skoku je: %d \n", adresa_konce);
					Sez_nastav_aktivni(list, adresa_konce);
				}
                break;
                
            //podmineny skok
            case IN_PGOTO:
            		printf("PGOTO typ: %d , hodn: %d\n", (((UkTBSPolozka)ukinstr->op1)->typ), ((UkTBSPolozka)ukinstr->op1)->data.dataBool);
            		if (!((((UkTBSPolozka)ukinstr->op1)->typ == TDBOOL && ((UkTBSPolozka)ukinstr->op1)->data.dataBool == FALSE) || ((UkTBSPolozka)ukinstr->op1)->typ == TDNIL)) { //cokoliv jineho nez FALSE a NIL znamena skok
            			printf("-- PGOTO --\n");
            				  Sez_nastav_aktivni(list, (UkTPlzkaSez)ukinstr->op2);
            		}else {
            			Sez_dalsi(list);
            		}
            		break;
                
            //mensi <
            case IN_MENSI:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
					((UkTBSPolozka)ukinstr->op3)->typ = TDBOOL;
                    if (((UkTBSPolozka)ukinstr->op1)->data.dataCis < ((UkTBSPolozka)ukinstr->op2)->data.dataCis) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else if (((UkTBSPolozka)ukinstr->op1)->typ == TDRETEZEC && ((UkTBSPolozka)ukinstr->op2)->typ == TDRETEZEC) {
					((UkTBSPolozka)ukinstr->op3)->typ = TDBOOL;
                    if ((strcmp(((UkTBSPolozka)ukinstr->op1)->data.dataRet, ((UkTBSPolozka)ukinstr->op2)->data.dataRet)) < 0) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else {
                    //return KONEC_CHYBA;
                    return ERR_SEMANT;
                }
                break;
                
            //vetsi >
            case IN_VETSI:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
					((UkTBSPolozka)ukinstr->op3)->typ = TDBOOL;
                    if (((UkTBSPolozka)ukinstr->op1)->data.dataCis > ((UkTBSPolozka)ukinstr->op2)->data.dataCis) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else if (((UkTBSPolozka)ukinstr->op1)->typ == TDRETEZEC && ((UkTBSPolozka)ukinstr->op2)->typ == TDRETEZEC) {
					((UkTBSPolozka)ukinstr->op3)->typ = TDBOOL;
                    if ((strcmp(((UkTBSPolozka)ukinstr->op1)->data.dataRet, ((UkTBSPolozka)ukinstr->op2)->data.dataRet)) > 0) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else {
                    //return KONEC_CHYBA;
                    return ERR_SEMANT;
                }
                break;
                
            //mensi nebo rovno <=
            case IN_MENROV:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
					((UkTBSPolozka)ukinstr->op3)->typ = TDBOOL;
                    if (((UkTBSPolozka)ukinstr->op1)->data.dataCis <= ((UkTBSPolozka)ukinstr->op2)->data.dataCis) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else if (((UkTBSPolozka)ukinstr->op1)->typ == TDRETEZEC && ((UkTBSPolozka)ukinstr->op2)->typ == TDRETEZEC) {
					((UkTBSPolozka)ukinstr->op3)->typ = TDBOOL;
                    if ((strcmp(((UkTBSPolozka)ukinstr->op1)->data.dataRet, ((UkTBSPolozka)ukinstr->op2)->data.dataRet)) <= 0) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else {
                    //return KONEC_CHYBA;
                    return ERR_SEMANT;
                }
                break;
                
            //vetsi nebo rovno >=
            case IN_VETROV:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
					((UkTBSPolozka)ukinstr->op3)->typ = TDBOOL;
                    if (((UkTBSPolozka)ukinstr->op1)->data.dataCis >= ((UkTBSPolozka)ukinstr->op2)->data.dataCis) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else if (((UkTBSPolozka)ukinstr->op1)->typ == TDRETEZEC && ((UkTBSPolozka)ukinstr->op2)->typ == TDRETEZEC) {
					((UkTBSPolozka)ukinstr->op3)->typ = TDBOOL;
                    if ((strcmp(((UkTBSPolozka)ukinstr->op1)->data.dataRet, ((UkTBSPolozka)ukinstr->op2)->data.dataRet)) >= 0) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else {
                    //return KONEC_CHYBA;
                    return ERR_SEMANT;
                }
                break;
                
            //rovna se ==
            case IN_ROVNO:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
					((UkTBSPolozka)ukinstr->op3)->typ = TDBOOL;
                    if (((UkTBSPolozka)ukinstr->op1)->data.dataCis == ((UkTBSPolozka)ukinstr->op2)->data.dataCis) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else if (((UkTBSPolozka)ukinstr->op1)->typ == TDRETEZEC && ((UkTBSPolozka)ukinstr->op2)->typ == TDRETEZEC) {
					((UkTBSPolozka)ukinstr->op3)->typ = TDBOOL;
                    if ((strcmp(((UkTBSPolozka)ukinstr->op1)->data.dataRet, ((UkTBSPolozka)ukinstr->op2)->data.dataRet)) == 0) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else {
                    //return KONEC_CHYBA;
                    return ERR_SEMANT;
                }
                break;
            
            //nerovna se ~=
            case IN_NEROVNO:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
					((UkTBSPolozka)ukinstr->op3)->typ = TDBOOL;
                    if (((UkTBSPolozka)ukinstr->op1)->data.dataCis != ((UkTBSPolozka)ukinstr->op2)->data.dataCis) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else if (((UkTBSPolozka)ukinstr->op1)->typ == TDRETEZEC && ((UkTBSPolozka)ukinstr->op2)->typ == TDRETEZEC) {
					((UkTBSPolozka)ukinstr->op3)->typ = TDBOOL;
                    if ((strcmp(((UkTBSPolozka)ukinstr->op1)->data.dataRet, ((UkTBSPolozka)ukinstr->op2)->data.dataRet)) != 0) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else {
                    //return KONEC_CHYBA;
                    return ERR_SEMANT;
                }
                break;
            
            //konkatenace
            case IN_KONK:
				printf("instrukce konkatenace\n\n\n");
				
//				printf("op3 adresa v konk: %d\n", (int)ukinstr->op3);
				printf("retkonk %s , %s\n", ((UkTBSPolozka)ukinstr->op1)->data.dataRet, ((UkTBSPolozka)ukinstr->op2)->data.dataRet);
				BVSVypisStrom(&pom_tab_sym);
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDRETEZEC && ((UkTBSPolozka)ukinstr->op2)->typ == TDRETEZEC) {
					((UkTBSPolozka)ukinstr->op3)->typ = TDRETEZEC;
                    Ret_konkatenace(((UkTBSPolozka)ukinstr->op1)->data.dataRet, ((UkTBSPolozka)ukinstr->op2)->data.dataRet, &(((UkTBSPolozka)ukinstr->op3)->data.dataRet));
                    printf("Co vysledek konk: _%s_\n", ((UkTBSPolozka)ukinstr->op3)->data.dataRet);
                }
                else {
                    printf("semanticka chyba - spatne typy v operaci konk %d\n",((UkTBSPolozka)ukinstr->op1)->typ);
                    return ERR_SEMANT;
                }
                break;
                
            case IN_TYPE:
            		if ( ((UkTBSPolozka)ukinstr->op3) != NULL ){
            			
            			if (((UkTBSPolozka)ukinstr->op3)->typ == TDRETEZEC){
            				free(((UkTBSPolozka)ukinstr->op3)->data.dataRet);
            			}
            			((UkTBSPolozka)ukinstr->op3)->data.dataRet = malloc( (strlen( type((UkTBSPolozka)ukinstr->op1) )+1) * sizeof(char) ); 
	                strcpy(((UkTBSPolozka)ukinstr->op3)->data.dataRet, type((UkTBSPolozka)ukinstr->op1));
	                ((UkTBSPolozka)ukinstr->op3)->typ = TDRETEZEC;
                }
                break;
                
            case IN_FIND:
                //zavolani funkce find()
                break;
                
            case IN_SORT:
                //strcpy(strcmp(((UkTBSPolozka)ukinstr->op3)->data.dataRet, strcmp(((UkTBSPolozka)ukinstr->op1)->data.dataRet);
                //heapsort(strcmp(((UkTBSPolozka)ukinstr->op3)->data.dataRet);
                break;
                
            case IN_SUBSTR_1:
            	pom_substr = ((UkTBSPolozka)ukinstr->op1);
	            break;
            case IN_SUBSTR_2:
		          	if (((UkTBSPolozka)ukinstr->op3) == NULL){
			          	break;
		          	}
		          /*
		          	if (((UkTBSPolozka)ukinstr->op3)->typ == TDRETEZEC){
		          		free(((UkTBSPolozka)ukinstr->op3)->data.dataRet);
		          	}
		          	((UkTBSPolozka)ukinstr->op3)->typ = TDRETEZEC;
            */
            		pom_substr->typ = TDRETEZEC;
		          	(((UkTBSPolozka)ukinstr->op3))->typ = TDRETEZEC;
		          	printf("string: _%s_\n",(((UkTBSPolozka)ukinstr->op3))->data.dataRet);
                substr(pom_substr, &(((UkTBSPolozka)ukinstr->op3)->data.dataRet), ((UkTBSPolozka)ukinstr->op1)->data.dataCis, ((UkTBSPolozka)ukinstr->op2)->data.dataCis);
                
		          	printf("string: _%s_\n",(((UkTBSPolozka)ukinstr->op3)->data.dataRet));
		          	
                //osetrit oprandy na NULL, pokud bude jakykoliv z nich NULL, vraci se rovnou NULL
                break;
                
            //operace prirazeni
            case IN_PRIRAD:
            set_first(zas_zpracovani);
        char znacka[] = "function";
				UkTPlzkaSezPar PomUk = NULL;
				UkTBSPolozka retezec = ((UkTBSPolozka)ukinstr->op2); // kam
				UkTBSPolozka ukazatel = ((UkTBSPolozka)ukinstr->op3); // co
				//UkTSezPar L = ((UkTSezPar)ukinstr->op1); // kde
			if (zas_zpracovani != NULL)	{
				//L->aktivni = L->prvni;
				
					while(zas_zpracovani->aktivni != NULL){
						//printf("!!!!!!!!!!!!!!!!!! %s\n", retezec->data.dataRet);
						if(strcmp(zas_zpracovani->aktivni->parametr.klic, znacka) == 0){
						
							PomUk = zas_zpracovani->aktivni;
						}
						set_nasl(zas_zpracovani);
					}
				}
				zas_zpracovani->aktivni = PomUk;
		
				while(zas_zpracovani->aktivni != NULL){
					if(strcmp(zas_zpracovani->aktivni->parametr.klic, retezec->data.dataRet) == 0){
							//pri nahrazovani retezce odalokuji jeho pamet, nez zmenit data
							if (zas_zpracovani->aktivni->parametr.data->typ == TDRETEZEC){
								free(zas_zpracovani->aktivni->parametr.data->data.dataRet);
							}
							switch(ukazatel->typ){
								case TDCISLO:
									zas_zpracovani->aktivni->parametr.data->data.dataCis = ukazatel->data.dataCis;
									printf("promenna nalezena a zkopirovana je cislo a jeho hodnota je: %f ma byt: %f\n", ukazatel->data.dataCis, zas_zpracovani->aktivni->parametr.data->data.dataCis);
									break;
								case TDRETEZEC:
									printf("U konkatenace blbne strlen : %s\n", ukazatel->data.dataRet);
									if ((zas_zpracovani->aktivni->parametr.data->data.dataRet = malloc((strlen(ukazatel->data.dataRet)+1)*sizeof(char)))==NULL){
										return ERR_INTERNI;
									}
									strcpy(zas_zpracovani->aktivni->parametr.data->data.dataRet, ukazatel->data.dataRet);
									
									printf("promenna nalezena a zkopirovana je retezec a jeho hodnota je: %s\n", ukazatel->data.dataRet);
									break;
								case TDBOOL:
									zas_zpracovani->aktivni->parametr.data->data.dataBool = ukazatel->data.dataBool;
									printf("promenna nalezena a zkopirovana je boolean a jeho hodnota je: %d\n", ukazatel->data.dataBool);
									break;
								case TDNIL:
									break;
							}
							zas_zpracovani->aktivni->parametr.data->typ= ukazatel->typ;
							printf("dostal sem se za switch\n");
					}
					
					set_nasl(zas_zpracovani);
				}
				/*
				adr = ukinstr->op3;
				printf("3------- adresa ktera se vyhledala v zasobniku (uvnitr instrukce): %d ---------------------\n", (int)ukinstr->op3);
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO) {
					printf("dostalo se ke zpracovani instrukce PRIRAD, prirazuje se cislo!!!!!!!, jeho hodnota je %f \n", ((UkTBSPolozka)ukinstr->op1)->data.dataCis);
					((UkTBSPolozka)adr)->typ = TDCISLO;
					printf("typ noveho prvku, kam se uklada hodnota je %d \n", ((UkTBSPolozka)adr)->typ);
                    ((UkTBSPolozka)adr)->data.dataCis = ((UkTBSPolozka)ukinstr->op1)->data.dataCis;
                    printf("hodnota noveho prvku je %f \n", ((UkTBSPolozka)adr)->data.dataCis);
                }
                else if (((UkTBSPolozka)ukinstr->op1)->typ == TDRETEZEC) {
                    ((UkTBSPolozka)adr)->typ = TDRETEZEC;
                    strcpy(((UkTBSPolozka)adr)->data.dataRet, ((UkTBSPolozka)ukinstr->op1)->data.dataRet);
                }
                else if (((UkTBSPolozka)ukinstr->op1)->typ == TDBOOL) {
					((UkTBSPolozka)adr)->typ = TDBOOL;
                    ((UkTBSPolozka)adr)->data.dataBool = ((UkTBSPolozka)ukinstr->op1)->data.dataBool;
                }
                else {
                    return KONEC_CHYBA;
                }
                */
                break;
                
            // instrukce pro vytazeni navratove adresy ze zasobniku
            case IN_POP:
					if((UkTBSFunkPol)ukinstr->op1 != NULL && (UkTInstr)ukinstr->op2 != NULL){
						 Pop_adr(zas_navr_adres, (UkTInstr)ukinstr->op2);
						 vymaz_promenne(zas_zpracovani);
					}
				break;
				
			// instrukce pro vlozeni navratove adresy na zasobnik 
			case IN_PUSH:
					if((UkTBSFunkPol)ukinstr->op1 != NULL && (UkTInstr)ukinstr->op2 != NULL){
						Push_adr(zas_navr_adres, (UkTInstr)ukinstr->op2);	
						kopiruj_parametry(zas_zpracovani, ((UkTBSFunkPol)ukinstr->op1)->zasobnik);	
						kopiruj_promenne(zas_zpracovani, &((UkTBSFunkPol)ukinstr->op1)->koren);
						
						/*
						printf("*************Vypis zasobniku tesne po PUSH**********\n");
						set_first(zas_zpracovani);
						while(zas_zpracovani->aktivni != NULL){
							printf("-> ->Klic: %s, dataCis: %s, typ: %d\n", zas_zpracovani->aktivni->parametr.klic, 
							zas_zpracovani->aktivni->parametr.data->data.dataRet, 
							zas_zpracovani->aktivni->parametr.data->typ);
							set_nasl(zas_zpracovani);
						}
						printf("**************************************\n");
						*/
					}
				break;
				
				
			// instrukce pro hledani promenne v globalnim zasobniku 
					case IN_HLEDEJ:;
						/*//break; //TODO: pozor, odstranit!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
							printf("--------------- kam_priradit ma hodnotu: %s -----------------------\n", ukinstr->op2->data.dataRet);
							UkTBSPolozka *pomocna_kralici_polozka;
							pomocna_kralici_polozka = (UkTBSPolozka*) ukinstr->op3;
							if(najdi_prom((UkTSezPar)ukinstr->op1, ukinstr->op2->data.dataRet, pomocna_kralici_polozka) == TRUE){
								//printf("promenna nalezena!! typ %d\n", (*ukinstr->op3).typ);
							}else{
								printf("promenna nebyla do haje nalezena!!!!!!!!!!!!!!!\n");
							}
							printf("op3: %d\n", (*((UkTBSPolozka*) ukinstr->op3))->typ);
							printf("------------------------------------------------navratova hodnota pred pred %d\n", (int) (*((UkTBSPolozka*) ukinstr->op3)));
						break;*/
						
						//op1 = zasobnik;
						//op2 = int
						//op3 = uktbsplzka s retezcem
						UkTPlzkaSez pom_uk;
						UkTBSPolozka *pomocna_kralici_polozka=NULL;
						pom_uk = list->aktivni; // zapamatuji kam se vratit
						pomocna_kralici_polozka = malloc(100);
						list->aktivni = list->aktivni->ukdalsi;
						while (list->aktivni->instrukce.typInstr == IN_HLEDEJ){
							list->aktivni = list->aktivni->ukdalsi;
						}
						// list aktivni je prvni instrukce za hledej
						
						if(najdi_prom((UkTSezPar)ukinstr->op1, ukinstr->op3->data.dataRet, pomocna_kralici_polozka) == TRUE){
							printf("promenna nalezena!!\n");
						}else{
							printf("promenna nebyla do haje nalezena!!!!!!!!!!!!!!!\n");
						}
						
						//priradim vyhledany ukazatek do spravneho operandu spravne instrukce
						switch((int) ukinstr->op2->data.dataCis){
							case 1:
								printf("prepisuji prvni operand\n");
								list->aktivni->instrukce.op1 = *pomocna_kralici_polozka;
								break; 
							case 2:
								printf("prepisuji druhy operand\n");
								list->aktivni->instrukce.op2 = *pomocna_kralici_polozka;
								break; 
							case 3:
								printf("prepisuji treti operand\n");
								list->aktivni->instrukce.op3 = *pomocna_kralici_polozka;
								break; 
							default: 
								printf("fatal error pri predavani do instrukce IN_HLEDEJ");
						}
						
						// navrat aktivity zpet
						list->aktivni = pom_uk;
						
						break;
          //konec interpretu
          case IN_KONEC:
              return KONEC_OK;
          
          default:
              break;
      }
        
        if (ukinstr->typInstr != IN_GOTO && ukinstr->typInstr != IN_PGOTO) {
			Sez_dalsi(list); //presuneme se na dalsi instrukci
		}
    }
    
    return KONEC_OK;
}

