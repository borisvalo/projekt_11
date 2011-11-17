#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpret.h"
#include "bvs.h"

void Vloz_instrukci(UkTSezInstr seznam, int typ, void *op1, void *op2, void *op3) {
    TInstr polozka;
    polozka.typInstr = typ;
    polozka.op1 = op1;
    polozka.op2 = op2;
    polozka.op3 = op3;
    
    Sez_vloz(seznam, &polozka);
}

int Interpret(UkTSezInstr list) {
    Sez_prvni(list); //nastavime aktivitu na prvni instrukci
    UkTInstr ukinstr;
    
    while (1) {
        ukinstr = Sez_hodnota_aktivniho(list); //nahrajeme dalsi instrukci
        
        switch (ukinstr->typInstr) { //automat
            //zadna operace
            case IN_NOOP:
                break;
                
            //cteni
            case IN_READ:
                break;
                
            //vypis
            case IN_WRITE:
                break;
                
            //soucet
            case IN_ADD:
                if (ukinstr->op1->typ == TDCISLO && ukinstr->op2->typ == TDCISLO) {
                    ukinstr->op3->data.dataCis = ukinstr->op1->data.dataCiss + ukinstr->op2->data.dataCis;
                }
                else {
                    printf("semanticka chyba\n");
                }
                break;
            
            //odcitani
            case IN_SUB:
                if (ukinstr->op1->typ == TDCISLO && ukinstr->op2->typ == TDCISLO) {
                    ukinstr->op3->data.dataCis = ukinstr->op1->data.dataCis - ukinstr->op2->data.dataCis;
                }
                else {
                    printf("semanticka chyba\n");
                }
                break;
            
            //nasobeni
            case IN_MUL:
                if (ukinstr->op1->typ == TDCISLO && ukinstr->op2->typ == TDCISLO) {
                    ukinstr->op3->data.dataCis = ukinstr->op1->data.dataCis * ukinstr->op2->data.dataCis;
                }
                else {
                    printf("semanticka chyba\n");
                }
                break;
                
            //deleni
            case IN_DIV:
                if (ukinstr->op1->typ == TDCISLO && ukinstr->op2->typ == TDCISLO) {
                    ukinstr->op3->data.dataCis = ukinstr->op1->data.dataCis / ukinstr->op2->data.dataCis;
                }
                else {
                    printf("semanticka chyba\n");
                }
                break;
            
            //navesti
            case IN_LABEL:
                break;
                
            //goto - nastavi aktivitu na konkretni prvek
            case IN_GOTO:
                Sez_nastav_aktivni(list, ukinstr->op1);
                break;
                
            //mensi <
            case IN_MENSI:
                if (ukinstr->op1->typ == TDCISLO) {
                    if (ukinstr->op1->data.dataCis < ukinstr->op2->data.dataCis) {
                        ukinstr->op3->data.dataBool = TRUE;
                    }
                    else {
                        ukinstr->op3->data.dataBool = FALSE;
                    }
                }
                else if (ukinstr->op1->typ == TDRETEZEC) {
                    if ((strcmp(ukinstr->op1->data.dataRet, ukinstr->op2->data.dataRet)) < 0) {
                        ukinstr->op3->data.dataBool = TRUE;
                    }
                    else {
                        ukinstr->op3->data.dataBool = FALSE;
                    }
                }
                break;
                
            //vetsi >
            case IN_VETSI:
                if (ukinstr->op1->typ == TDCISLO) {
                    if (ukinstr->op1->data.dataCis > ukinstr->op2->data.dataCis) {
                        ukinstr->op3->data.dataBool = TRUE;
                    }
                    else {
                        ukinstr->op3->data.dataBool = FALSE;
                    }
                }
                else if (ukinstr->op1->typ == TDRETEZEC) {
                    if ((strcmp(ukinstr->op1->data.dataRet, ukinstr->op2->data.dataRet)) > 0) {
                        ukinstr->op3->data.dataBool = TRUE;
                    }
                    else {
                        ukinstr->op3->data.dataBool = FALSE;
                    }
                }
                break;
                
            //mensi nebo rovno <=
            case IN_MENROV:
                if (ukinstr->op1->typ == TDCISLO) {
                    if (ukinstr->op1->data.dataCis <= ukinstr->op2->data.dataCis) {
                        ukinstr->op3->data.dataBool = TRUE;
                    }
                    else {
                        ukinstr->op3->data.dataBool = FALSE;
                    }
                }
                else if (ukinstr->op1->typ == TDRETEZEC) {
                    if ((strcmp(ukinstr->op1->data.dataRet, ukinstr->op2->data.dataRet)) <= 0) {
                        ukinstr->op3->data.dataBool = TRUE;
                    }
                    else {
                        ukinstr->op3->data.dataBool = FALSE;
                    }
                }
                break;
                
            //vetsi nebo rovno >=
            case IN_VETROV:
                if (ukinstr->op1->typ == TDCISLO) {
                    if (ukinstr->op1->data.dataCis >= ukinstr->op2->data.dataCis) {
                        ukinstr->op3->data.dataBool = TRUE;
                    }
                    else {
                        ukinstr->op3->data.dataBool = FALSE;
                    }
                }
                else if (ukinstr->op1->typ == TDRETEZEC) {
                    if ((strcmp(ukinstr->op1->data.dataRet, ukinstr->op2->data.dataRet)) >= 0) {
                        ukinstr->op3->data.dataBool = TRUE;
                    }
                    else {
                        ukinstr->op3->data.dataBool = FALSE;
                    }
                }
                break;
                
            //rovna se ==
            case IN_ROVNO:
                if (ukinstr->op1->typ == TDCISLO) {
                    if (ukinstr->op1->data.dataCis == ukinstr->op2->data.dataCis) {
                        ukinstr->op3->data.dataBool = TRUE;
                    }
                    else {
                        ukinstr->op3->data.dataBool = FALSE;
                    }
                }
                else if (ukinstr->op1->typ == TDRETEZEC) {
                    if ((strcmp(ukinstr->op1->data.dataRet, ukinstr->op2->data.dataRet)) == 0) {
                        ukinstr->op3->data.dataBool = TRUE;
                    }
                    else {
                        ukinstr->op3->data.dataBool = FALSE;
                    }
                }
                break;
                
            case IN_KONK:
                //zavolani funkce konkatenace
                break;
                
            case IN_TYPE:
                //zavolani funkce type()
                break;
                
            case IN_FIND:
                //zavolani funkce find()
                break;
                
            case IN_SORT:
                //zavolani funkce sort()
                break;
                
            case IN_SUBSTR:
                //zavolani funkce substr()
                break;
                
            //konec interpretu
            case IN_KONEC:
                return KONEC_OK;
            
            default:
                break;
        }
    }
    
    return KONEC_OK;
}
