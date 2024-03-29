//bvs.c

//potrebne knihovny
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpret.h"
#include "str.h"
#include "parser.h"


//Inicializace stromu
void BVSInit (UkTBSUzel *Kor) {
    (*Kor) = NULL;
}	

int BVSNajdi (UkTBSUzel Kor, char *K, UkTBSPolozka *obsah)	{
	if (Kor == NULL) { //neni v cem hledat
		printf("bvs najdi nenaslo - neni obsah\n");
        return FALSE;
    }else {
        if (strcmp(Kor->klic,K) != 0) {
            if (strcmp(Kor->klic, K) > 0) { //doleva
                return BVSNajdi(Kor->luk, K, obsah);
            }
            else { //doprava
                return BVSNajdi(Kor->puk, K, obsah);
            }
        }
        else if (strcmp(Kor->klic,K) == 0){
        		//if(obsah != NULL){
            	*obsah = &(Kor->data); //vratime ukazatel na strukturu dat
            	printf("tohle bzlo v korenu nalezeneho ve striome %d \n",Kor->data.typ);
            //}
            return TRUE;
        }
    }	
    return FALSE;
} 

void BVSVloz (UkTBSUzel* Kor, char *K, UkTBSPolozka obsah) {	
	if ((*Kor) == NULL) { //vytvorime novy uzel

        UkTBSUzel ptr;
        if ((ptr = (UkTBSUzel) malloc(sizeof(TBSUzel))) == NULL) {
            printf("chyba mallocu!\n");
        }
        
        
        Ret_alokuj(&(ptr->klic), strlen(K)+1); //alokujeme misto pro klic
        
        ptr->luk = NULL;
        ptr->puk = NULL;
        strcpy(ptr->klic, K); //vlozime klic
        if(obsah != NULL){
        	ptr->data.typ = obsah->typ;
        }else{
        	ptr->data.typ = TDNIL;
        }
        if (ptr->data.typ == TDRETEZEC) {
        				Ret_alokuj(&(ptr->data.data.dataRet), strlen(obsah->data.dataRet)+1);
        				strcpy(ptr->data.data.dataRet, obsah->data.dataRet);
        }
        else {
        				if(obsah != NULL){
        					ptr->data.data = obsah->data;        					
        				}
        }
        
        (*Kor) = ptr; //nakonec novy uzel vlozime primo do stromu
    }
    else { //aktualni ukazatel ukazuje na existujici uzel
        if (strcmp(K, (*Kor)->klic) < 0) { //dame se do leveho podstromu
            BVSVloz(&((*Kor)->luk), K, obsah);
        }
        else if (strcmp(K, (*Kor)->klic) > 0) { //dame se do praveho podstromu
            BVSVloz(&((*Kor)->puk), K, obsah);
        }
        else { //obnovime data aktualniho uzlu
        // opravit upravu dat
					if(obsah->typ == TDRETEZEC){
						if((*Kor)->data.typ == TDRETEZEC){
							free((*Kor)->data.data.dataRet);
						}
				
						(*Kor)->data.typ = obsah->typ;
						(*Kor)->data.data.dataRet = malloc((strlen(obsah->data.dataRet)+1)*sizeof(char));
						strcpy((*Kor)->data.data.dataRet, obsah->data.dataRet);
			
					}else{
						if((*Kor)->data.typ == TDRETEZEC){
							free((*Kor)->data.data.dataRet);
						}
				
						(*Kor)->data.typ = obsah->typ;
						(*Kor)->data.data = obsah->data;
					}
        }
    }
}

void BVSNahradZaPraveho(UkTBSUzel PtrReplaced, UkTBSUzel *Kor) {
	UkTBSUzel pom;
    pom = NULL;
    
    if ((*Kor) != NULL) {
        if ((*Kor)->puk != NULL) {
            BVSNahradZaPraveho(PtrReplaced, (&((*Kor)->puk)));
        }
        else {
            PtrReplaced->data.typ = (*Kor)->data.typ;
            PtrReplaced->data.data = (*Kor)->data.data;
            PtrReplaced->klic = (*Kor)->klic;
            pom = (*Kor);
            (*Kor) = (*Kor)->luk;
            free(pom);
        }
    }
}

void BVSVymaz (UkTBSUzel *Kor, char *K) {
	UkTBSUzel pom;
    pom = NULL;
    
    if ((*Kor) != NULL) {
        if (strcmp((*Kor)->klic, K) > 0) {
            BVSVymaz(&((*Kor)->luk), K);
        }
        else if (strcmp((*Kor)->klic, K) < 0) {
            BVSVymaz(&((*Kor)->puk), K);
        }
        else {
            pom = (*Kor);
            if (pom->puk == NULL) {
                (*Kor) = pom->luk;
                free(pom);
            }
            else if (pom->luk == NULL) {
                (*Kor) = pom->puk;
                free(pom);
            }
            else {
                BVSNahradZaPraveho(*Kor, (&((*Kor)->luk)));
            }
        }
    }
} 

void BVSZrus (UkTBSUzel *Kor) {	
    if ((*Kor) != NULL) {
        if ((*Kor)->luk != NULL) {
            BVSZrus(&(*Kor)->luk);
        }
        if ((*Kor)->puk != NULL) {
            BVSZrus(&(*Kor)->puk);
        }
        
        if ((*Kor)->data.typ == TDRETEZEC) { // kvuli retezci, potrebujeme od-/alokovat novy prostor
        				Ret_uvolni((*Kor)->data.data.dataRet);
        }
        free((*Kor)->klic);
        free(*Kor);
        (*Kor) = NULL;
    }
}

//------------------ Funkce pro BVS Funkci -------------------------
void BVSFunkceInit (UkTBSFunkce *Kor) {
    (*Kor) = NULL;
}	

int BVSFunkceNajdi (UkTBSFunkce Kor, char *K, UkTBSFunkPol *obsah)	{
	if (Kor == NULL) { //neni v cem hledat
			printf("bvs_funkcenajdi: koren je null\n");
        return FALSE;
    }
    else {
        if (strcmp(Kor->klic, K) != 0) {
            if (strcmp(Kor->klic, K) > 0) { //doleva
                return BVSFunkceNajdi(Kor->luk, K, obsah);
            }
            else { //doprava
                return BVSFunkceNajdi(Kor->puk, K, obsah);
            }
        }
        else if(strcmp(Kor->klic, K) == 0){
			printf("spravne\n");
				
			*obsah = Kor->data; //vratime ukazatel na strukturu dat
			
            return TRUE;
        }else{
			printf("neocekavana chyba\n");
			return FALSE;
		}
    }	
} 

void BVSFunkceVloz (UkTBSFunkce* Kor, char *K, UkTBSFunkPol obsah) {	
	if ((*Kor) == NULL) { //vytvorime novy uzel
        UkTBSFunkce ptr;
        if ((ptr = (UkTBSFunkce) malloc(sizeof(TBSFunkce))) == NULL) {
            printf("chyba mallocu!\n");
        }
        Ret_alokuj(&(ptr->klic), strlen(K)+1); //alokujeme misto pro klic
        
        if((ptr->data = (UkTBSFunkPol) malloc(sizeof(struct bsfunkpol))) == NULL){
				printf("chyba mallocu \n");
		}
		
        ptr->luk = NULL;
        ptr->puk = NULL;
        strcpy(ptr->klic, K); //vlozime klic
        
        printf("%s\n", ptr->klic);
        ptr->data->pocet_param = 0;
        ptr->data->koren = NULL;
        ptr->data->zasobnik = NULL;
        ptr->data->adresa = NULL;
        
        (*Kor) = ptr; //nakonec novy uzel vlozime primo do stromu
    }
    else { //aktualni ukazatel ukazuje na existujici uzel
        if (strcmp(K, (*Kor)->klic) < 0) { //dame se do leveho podstromu
            BVSFunkceVloz(&((*Kor)->luk), K, obsah);
        }
        else if (strcmp(K, (*Kor)->klic) > 0) { //dame se do praveho podstromu
            BVSFunkceVloz(&((*Kor)->puk), K, obsah);
        }
        else { //obnovime data aktualniho uzlu
			if(obsah != NULL){
				(*Kor)->data->pocet_param = obsah->pocet_param;
				(*Kor)->data->koren = obsah->koren;
				(*Kor)->data->zasobnik = obsah->zasobnik;
			}
        }
    }
}
void BVSFunkceZrus (UkTBSFunkce *Kor) {	
    if ((*Kor) != NULL) {
        if ((*Kor)->luk != NULL) {
            BVSFunkceZrus(&(*Kor)->luk);
        }
        if ((*Kor)->puk != NULL) {
            BVSFunkceZrus(&(*Kor)->puk);
        }
        BVSZrus (&(*Kor)->data->koren);
        free((*Kor)->data->zasobnik);
        free((*Kor)->klic);
        free((*Kor)->data);
        free(*Kor);
        (*Kor) = NULL;
    }
}

void BVSVypisStrom (UkTBSUzel *Kor) {	
    if ((*Kor) != NULL) {
			switch((*Kor)->data.typ){
				case TDRETEZEC:
					printf("hodnota: %s klic: %s \n", (*Kor)->data.data.dataRet, (*Kor)->klic);
					break;
				case TDCISLO:
					printf("hodnota: %f klic: %s \n", (*Kor)->data.data.dataCis, (*Kor)->klic);
					break;
				case TDBOOL:
					printf("hodnota: %d klic: %s \n", (*Kor)->data.data.dataBool, (*Kor)->klic);
					break;
				case TDNIL: 
					printf("hodnota: nil klic: %s \n", (*Kor)->klic);
					break;
				default:
					printf("je to na dve veci klic: %s \n", (*Kor)->klic);
			}
        if ((*Kor)->luk != NULL) {
            BVSVypisStrom(&(*Kor)->luk);
        }
        if ((*Kor)->puk != NULL) {
            BVSVypisStrom(&(*Kor)->puk);
        }
    }else
    printf("vzpis : koren je null\n");
}



