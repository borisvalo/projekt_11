#include <stdio.h>
#include <stdlib.h>

#include "interpret.h"

void Sez_init(UkTSezInstr L) {
	L->aktivni = NULL;
	L->prvni = NULL;
    L->posledni = NULL;
}

void Sez_zrus(UkTSezInstr L) {
	UkTPlzkaSez PomUk;
	while (L->prvni != NULL) {
		PomUk = L->prvni->ukdalsi;
		free(L->prvni);
		L->prvni = PomUk;
	}
    L->aktivni = NULL;
}

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

void Sez_prvni(UkTSezInstr L) {
    L->aktivni = L->prvni;
}

void *Sez_hodnota_aktivniho(UkTSezInstr L)		{
    if (L->aktivni == NULL) {
        printf("chybka - seznam neni aktivni..\n");
        return NULL;
    }
    
	return &(L->aktivni->instrukce);
}


void Sez_dalsi(UkTSezInstr L)	{
    if (L->aktivni != NULL) {
        L->aktivni = L->aktivni->ukdalsi;
    }
}

void Sez_nastav_aktivni(UkTSezInstr L, void *instrukce) {
    L->aktivni = instrukce;
}

int main() {
    UkTSezInstr seznam;
    Sez_init(seznam);
    
    TInstr instr1;
    instr1.typInstr = IN_ADD;
    instr1.op1 = NULL;
    instr1.op2 = NULL;
    instr1.op3 = NULL;
    
    Sez_vloz(seznam, &instr1);
    
    Sez_zrus(seznam);
    
    return 0;
}
