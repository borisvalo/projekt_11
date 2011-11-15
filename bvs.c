//bvs.c

//potrebne knihovny
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bvs.h"
#include "str.h"

//Inicializace stromu
void BVSInit (UkTBSUzel *Kor) {
    (*Kor) = NULL;
}	

int BVSNajdi (UkTBSUzel Kor, char *K, UkTBSPolozka obsah)	{
	if (Kor == NULL) { //neni v cem hledat
        return FALSE;
    }
    else {
        if (strcmp(Kor->klic, K) != 0) {
            if (strcmp(Kor->klic, K) > 0) { //doleva
                return BVSNajdi(Kor->luk, K, obsah);
            }
            else { //doprava
                return BVSNajdi(Kor->puk, K, obsah);
            }
        }
        else {
            obsah = &(Kor->data); //vratime ukazatel na strukturu dat
            return TRUE;
        }
    }	
} 

void BVSVloz (UkTBSUzel* Kor, char *K, UkTBSPolozka obsah) {	
	if ((*Kor) == NULL) { //vytvorime novy uzel
        UkTBSUzel ptr;
        if ((ptr = (UkTBSUzel) malloc(sizeof(TBSUzel))) == NULL) {
            printf("chyba mallocu!\n");
        }
        
        Ret_alokuj(&(ptr->klic), 10); //alokujeme misto pro klic
        
        ptr->luk = NULL;
        ptr->puk = NULL;
        strcpy(ptr->klic, K); //vlozime klic
        ptr->data.typ = obsah->typ;
        ptr->data.data = obsah->data;
        
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
            (*Kor)->data.typ = obsah->typ;
            (*Kor)->data.data = obsah->data;
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
        
        free(*Kor);
        (*Kor) = NULL;
    }
}

/*
int main() {
    TBSUzel str;
    UkTBSUzel strom;
    strom = &str;
    BVSInit(&strom);
    
    //pridani cisla
    TBSPolozka polozka1;
    polozka1.typ = TDCISLO;
    polozka1.data.dataCis = 10;
    BVSVloz(&strom, "prvni", &polozka1);
    
    //pridani bool hodnoty
    TBSPolozka polozka2;
    polozka2.typ = TDCISLO;
    polozka2.data.dataBool = TRUE;
    BVSVloz(&strom, "druhy", &polozka2);
    
    //pridani retezce - nutnost alokace!!!
    TBSPolozka polozka3;
    polozka3.typ = TDCISLO;
    Ret_alokuj(&(polozka3.data.dataRet), 10);
    strcpy(polozka3.data.dataRet, "ret");
    BVSVloz(&strom, "treti", &polozka3);
    
    BVSZrus(&strom);
    
    return 0;
}
*/
