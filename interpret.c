#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "interpret.h"
#include "string.h"
#include "bvs.h"
//#include "ial.c"

int Vloz_instrukci(UkTSezInstr seznam, int typ, void *op1, void *op2, void *op3) {
    TInstr polozka;
    
    polozka.typInstr = typ;
    polozka.op1 = op1;
    polozka.op2 = op2;
    polozka.op3 = op3;
    
    Sez_vloz(seznam, &polozka);
    
    return KONEC_OK;
}

int Interpret(UkTSezInstr list) {
    Sez_prvni(list); //nastavime aktivitu na prvni instrukci
    UkTInstr ukinstr;
    
    while (1) {
        ukinstr = Sez_hodnota_aktivniho(list); //nahrajeme aktivni instrukci
        
        switch (ukinstr->typInstr) { //automat
            //cteni
            case IN_READ:
                break;
                
            //vypis
            case IN_WRITE:
                //write(ukinstr->op1);
                break;
                
            //soucet
            case IN_ADD:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
                    ((UkTBSPolozka)ukinstr->op3)->data.dataCis = ((UkTBSPolozka)ukinstr->op1)->data.dataCis + ((UkTBSPolozka)ukinstr->op2)->data.dataCis;
                }
                else {
                    printf("semanticka chyba\n");
                }
                break;
            
            //odcitani
            case IN_SUB:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
                    ((UkTBSPolozka)ukinstr->op3)->data.dataCis = ((UkTBSPolozka)ukinstr->op1)->data.dataCis - ((UkTBSPolozka)ukinstr->op2)->data.dataCis;
                }
                else {
                    printf("semanticka chyba\n");
                }
                break;
            
            //nasobeni
            case IN_MUL:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
                    ((UkTBSPolozka)ukinstr->op3)->data.dataCis = ((UkTBSPolozka)ukinstr->op1)->data.dataCis * ((UkTBSPolozka)ukinstr->op2)->data.dataCis;
                }
                else {
                    printf("semanticka chyba\n");
                }
                break;
                
            //deleni
            case IN_DIV:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
                    if (((UkTBSPolozka)ukinstr->op2)->data.dataCis == 0) {
                        printf("chyba: deleni nulou!!\n");
                        return KONEC_CHYBA;
                    }
                    ((UkTBSPolozka)ukinstr->op3)->data.dataCis = ((UkTBSPolozka)ukinstr->op1)->data.dataCis / ((UkTBSPolozka)ukinstr->op2)->data.dataCis;
                }
                else {
                    printf("semanticka chyba\n");
                }
                break;
                
            //mocnina
            case IN_MOCN:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
                    ((UkTBSPolozka)ukinstr->op3)->data.dataCis = pow(((UkTBSPolozka)ukinstr->op1)->data.dataCis, ((UkTBSPolozka)ukinstr->op2)->data.dataCis);
                }
                else {
                    printf("semanticka chyba\n");
                }
                break;
            
            //navesti
            case IN_NVSTI:
                //ukinstr->op1 = list->aktivni;
                break;
                
            //goto - nastavi aktivitu na konkretni prvek
            case IN_GOTO:
                Sez_nastav_aktivni(list, ukinstr->op1);
                break;
                
            //mensi <
            case IN_MENSI:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
                    if (((UkTBSPolozka)ukinstr->op1)->data.dataCis < ((UkTBSPolozka)ukinstr->op2)->data.dataCis) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else if (((UkTBSPolozka)ukinstr->op1)->typ == TDRETEZEC && ((UkTBSPolozka)ukinstr->op2)->typ == TDRETEZEC) {
                    if ((strcmp(((UkTBSPolozka)ukinstr->op1)->data.dataRet, ((UkTBSPolozka)ukinstr->op2)->data.dataRet)) < 0) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else {
                    return KONEC_CHYBA;
                }
                break;
                
            //vetsi >
            case IN_VETSI:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
                    if (((UkTBSPolozka)ukinstr->op1)->data.dataCis > ((UkTBSPolozka)ukinstr->op2)->data.dataCis) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else if (((UkTBSPolozka)ukinstr->op1)->typ == TDRETEZEC && ((UkTBSPolozka)ukinstr->op2)->typ == TDRETEZEC) {
                    if ((strcmp(((UkTBSPolozka)ukinstr->op1)->data.dataRet, ((UkTBSPolozka)ukinstr->op2)->data.dataRet)) > 0) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else {
                    return KONEC_CHYBA;
                }
                break;
                
            //mensi nebo rovno <=
            case IN_MENROV:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
                    if (((UkTBSPolozka)ukinstr->op1)->data.dataCis <= ((UkTBSPolozka)ukinstr->op2)->data.dataCis) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else if (((UkTBSPolozka)ukinstr->op1)->typ == TDRETEZEC && ((UkTBSPolozka)ukinstr->op2)->typ == TDRETEZEC) {
                    if ((strcmp(((UkTBSPolozka)ukinstr->op1)->data.dataRet, ((UkTBSPolozka)ukinstr->op2)->data.dataRet)) <= 0) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else {
                    return KONEC_CHYBA;
                }
                break;
                
            //vetsi nebo rovno >=
            case IN_VETROV:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
                    if (((UkTBSPolozka)ukinstr->op1)->data.dataCis >= ((UkTBSPolozka)ukinstr->op2)->data.dataCis) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else if (((UkTBSPolozka)ukinstr->op1)->typ == TDRETEZEC && ((UkTBSPolozka)ukinstr->op2)->typ == TDRETEZEC) {
                    if ((strcmp(((UkTBSPolozka)ukinstr->op1)->data.dataRet, ((UkTBSPolozka)ukinstr->op2)->data.dataRet)) >= 0) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else {
                    return KONEC_CHYBA;
                }
                break;
                
            //rovna se ==
            case IN_ROVNO:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
                    if (((UkTBSPolozka)ukinstr->op1)->data.dataCis == ((UkTBSPolozka)ukinstr->op2)->data.dataCis) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else if (((UkTBSPolozka)ukinstr->op1)->typ == TDRETEZEC && ((UkTBSPolozka)ukinstr->op2)->typ == TDRETEZEC) {
                    if ((strcmp(((UkTBSPolozka)ukinstr->op1)->data.dataRet, ((UkTBSPolozka)ukinstr->op2)->data.dataRet)) == 0) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else {
                    return KONEC_CHYBA;
                }
                break;
            
            //nerovna se ~=
            case IN_NEROVNO:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO && ((UkTBSPolozka)ukinstr->op2)->typ == TDCISLO) {
                    if (((UkTBSPolozka)ukinstr->op1)->data.dataCis != ((UkTBSPolozka)ukinstr->op2)->data.dataCis) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else if (((UkTBSPolozka)ukinstr->op1)->typ == TDRETEZEC && ((UkTBSPolozka)ukinstr->op2)->typ == TDRETEZEC) {
                    if ((strcmp(((UkTBSPolozka)ukinstr->op1)->data.dataRet, ((UkTBSPolozka)ukinstr->op2)->data.dataRet)) != 0) {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = TRUE;
                    }
                    else {
                        ((UkTBSPolozka)ukinstr->op3)->data.dataBool = FALSE;
                    }
                }
                else {
                    return KONEC_CHYBA;
                }
                break;
            
            //konkatenace
            case IN_KONK:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDRETEZEC && ((UkTBSPolozka)ukinstr->op2)->typ == TDRETEZEC) {
                    Ret_konkatenace(((UkTBSPolozka)ukinstr->op1)->data.dataRet, ((UkTBSPolozka)ukinstr->op2)->data.dataRet, &(((UkTBSPolozka)ukinstr->op3)->data.dataRet));
                }
                else {
                    printf("semanticka chyba\n");
                }
                break;
                
            case IN_TYPE:
                //zavolani funkce type()
                break;
                
            case IN_FIND:
                //zavolani funkce find()
                break;
                
            case IN_SORT:
                //strcpy(strcmp(((UkTBSPolozka)ukinstr->op3)->data.dataRet, strcmp(((UkTBSPolozka)ukinstr->op1)->data.dataRet);
                //heapsort(strcmp(((UkTBSPolozka)ukinstr->op3)->data.dataRet);
                break;
                
            case IN_SUBSTR:
                //zavolani funkce substr()
                break;
                
            //operace prirazeni
            case IN_PRIRAD:
                if (((UkTBSPolozka)ukinstr->op1)->typ == TDCISLO) {
                    ((UkTBSPolozka)ukinstr->op3)->data.dataCis = ((UkTBSPolozka)ukinstr->op1)->data.dataCis;
                }
                else if (((UkTBSPolozka)ukinstr->op1)->typ == TDRETEZEC) {
                    strcpy(((UkTBSPolozka)ukinstr->op3)->data.dataRet, ((UkTBSPolozka)ukinstr->op1)->data.dataRet);
                }
                else if (((UkTBSPolozka)ukinstr->op1)->typ == TDBOOL) {
                    ((UkTBSPolozka)ukinstr->op3)->data.dataBool = ((UkTBSPolozka)ukinstr->op1)->data.dataBool;
                }
                else {
                    return KONEC_CHYBA;
                }
                break;
                
            //konec interpretu
            case IN_KONEC:
                return KONEC_OK;
            
            default:
                break;
        }
        
        Sez_dalsi(list); //presuneme se na dalsi instrukci
    }
    
    return KONEC_OK;
}