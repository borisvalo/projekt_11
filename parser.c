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


extern UkTToken token;
extern int chyba;
extern FILE *soubor;
extern TZasobnik zasobnik;
extern UkTBSUzel pom_tab_sym;
extern UkTBSUzel nazvy_funkci; // kam odkazuje navesti
extern UkTBSPolozka obsah;
extern UkTSezInstr seznam_instrukci;
extern unsigned int klic_cislo;
extern TPrvek prvek_pomocny;
extern int typ_instrukce;
extern void *op1; 
extern void *op2; 
extern void *op3;

// ---- nove pridano
extern UkTBSFunkce strom_funkci;
extern UkTBSFunkPol pom_uzel_funkce;
extern char *pom_token_data;
// --- konec novyho


UkTBSUzel pole_stromu;
int delka_pole_stromu;


typedef enum {

    ROVNO = 555,   // =     // ZVOLIT TAM NEJAKE CISLO, TREBA 555
    VETSI,         // >
    MENSI,         // <

    NETERMINAL,    // E
    PTCHYBA          // chyba

} pt_symbol;


// typ operace / znaku
typedef enum {

    OPPLUS = 0,        //  +    0
    OPMINUS,           //  -
    OPKRAT,            //  *
    OPDELENO,          //  /
    OPMOCNINA,         //  ^
    OPLEVAZAVORKA,     //  (      5
    OPPRAVAZAVORKA,    //  )
    OPJEROVNO,         //  ==
    OPNENIROVNO,       //  ~=
    OPMENSITKO,        //  <
    OPVETSITKO,        //  >        10
    OPMENSITKOROVNO,   //  <=
    OPVETSITKOROVNO,   //  >=
    OPKONKATENACE,     //  ..
    OPIDENTIFIKATOR = 14,   //  i
    OPINTEGER = 14,         //  integer
    OPDESETINNE = 14,       //  desetinne cislo
    OPEXPCISLO = 14,        //  cislo s 'e' nebo 'E'
    OPRETEZEC = 14,         //  retezec
    OPFALSE = 14,
    OPTRUE = 14,
    OPNIL = 14,
    DOLAR = 15         // $

} typ_operace;



// Vygeneruje nazev (klic) pro pomocne symboly
//
char *generuj_klic(int posun) {
    char *klic = malloc(sizeof(char) * 20);
    sprintf(klic, "pom_%u", (klic_cislo++)-posun);
    return klic;
}



// Funkce na prevedeni Ondrova tokenu na muj OP:
int preved_z_tokenu(int dalsi_token) {

    switch(dalsi_token) {

        case INTKONEC:
            return OPINTEGER;
        case DESKONEC:
            return OPDESETINNE;
        case EXPKONEC:
            return OPEXPCISLO;
        case TNFALSE:
            return OPFALSE;
        case TNTRUE:
            return OPTRUE;
        case TNNIL:
            return OPNIL;
        case RETEZEC:
            return OPRETEZEC;
        case ZAVLEVA:
            return OPLEVAZAVORKA;
        case ZAVPRAVA:
            return OPPRAVAZAVORKA;
        case PLUS:
            return OPPLUS;
        case KRAT:
            return OPKRAT;
        case DELENO:
            return OPDELENO;
        case KONKATENACE:
            return OPKONKATENACE;
        case POROVNANI:
            return OPJEROVNO;
        case MOCNINA:
            return OPMOCNINA;
        case VETSIROVNO:
            return OPVETSITKOROVNO;
        case VETSITKOKONEC:
            return OPVETSITKO;
        case MENSIROVNO:
            return OPMENSITKOROVNO;
        case MENSITKOKONEC:
            return OPMENSITKO;
        case NEROVNASE:
            return OPNENIROVNO;
        case IDKONEC:
            return OPIDENTIFIKATOR;
        case MINUSKONEC:
            return OPMINUS;
        case NETERMINAL:
            return NETERMINAL;
        case MENSI:
            return MENSI;
        case DOLAR:
            return DOLAR;

    }

    return PTCHYBA;

}




// precedencni tabulka - staticka, aby se nedala zmenit a aby ji nevidely ostatni soubory

static int preced_tabulka[16][16] = {

//     +      -      *      /      ^      (      )      ==     ~=     <      >      <=     >=     ..     i      $
    {VETSI, VETSI, MENSI, MENSI, MENSI, MENSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, MENSI, VETSI},   // +
    {VETSI, VETSI, MENSI, MENSI, MENSI, MENSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, MENSI, VETSI},   // -
    {VETSI, VETSI, VETSI, VETSI, MENSI, MENSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, MENSI, VETSI},   // *
    {VETSI, VETSI, VETSI, VETSI, MENSI, MENSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, MENSI, VETSI},   // /
    {VETSI, VETSI, VETSI, VETSI, MENSI, MENSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, MENSI, VETSI},   // ^
    {MENSI, MENSI, MENSI, MENSI, MENSI, MENSI, ROVNO, MENSI, MENSI, MENSI, MENSI, MENSI, MENSI, MENSI, MENSI, PTCHYBA},   // (
    {VETSI, VETSI, VETSI, VETSI, VETSI, PTCHYBA, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, PTCHYBA, VETSI},   // )
    {MENSI, MENSI, MENSI, MENSI, MENSI, MENSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, MENSI, MENSI, VETSI},   // ==
    {MENSI, MENSI, MENSI, MENSI, MENSI, MENSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, MENSI, MENSI, VETSI},   // ~=
    {MENSI, MENSI, MENSI, MENSI, MENSI, MENSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, MENSI, MENSI, VETSI},   // <
    {MENSI, MENSI, MENSI, MENSI, MENSI, MENSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, MENSI, MENSI, VETSI},   // >
    {MENSI, MENSI, MENSI, MENSI, MENSI, MENSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, MENSI, MENSI, VETSI},   // <=
    {MENSI, MENSI, MENSI, MENSI, MENSI, MENSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, MENSI, MENSI, VETSI},   // >=
    {MENSI, MENSI, MENSI, MENSI, MENSI, MENSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, MENSI, VETSI},   // ..
    {VETSI, VETSI, VETSI, VETSI, VETSI, PTCHYBA, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, VETSI, PTCHYBA, VETSI},   // i
    {MENSI, MENSI, MENSI, MENSI, MENSI, MENSI, PTCHYBA, MENSI, MENSI, MENSI, MENSI, MENSI, MENSI, MENSI, MENSI, PTCHYBA},   // $

};













int dej_token(){
	chyba = ziskej_dalsi_token(soubor,token);
	if (token->typ == IDKONEC){
		kontrola_identifikatoru();
	}
	return chyba;
}

void kontrola_identifikatoru(){
	//assert(token->typ == IDKONEC);
	if (strcmp(token->data, "do") == 0) {
		token->typ = TNDO;
	}else	if (strcmp(token->data, "else") == 0) {
		token->typ = TNELSE;
	}else	if (strcmp(token->data, "end") == 0) {
		token->typ = TNEND;
	}else	if (strcmp(token->data, "false") == 0) {
		token->typ = TNFALSE;
	}else	if (strcmp(token->data, "function") == 0) {
		token->typ = TNFUNCTION;
	}else	if (strcmp(token->data, "if") == 0) {
		token->typ = TNIF;
	}else	if (strcmp(token->data, "local") == 0) {
		token->typ = TNLOCAL;
	}else	if (strcmp(token->data, "nil") == 0) {
		token->typ = TNNIL;
	}else	if (strcmp(token->data, "read") == 0) {
		token->typ = TNREAD;
	}else	if (strcmp(token->data, "return") == 0) {
		token->typ = TNRETURN;
	}else	if (strcmp(token->data, "then") == 0) {
		token->typ = TNTHEN;
	}else	if (strcmp(token->data, "true") == 0) {
		token->typ = TNTRUE;
	}else	if (strcmp(token->data, "while") == 0) {
		token->typ = TNWHILE;
	}else	if (strcmp(token->data, "write") == 0) {
		token->typ = TNWRITE;
	}else	if (strcmp(token->data, "and") == 0) {
		token->typ = RSAND;
	}else	if (strcmp(token->data, "break") == 0) {
		token->typ = RSBREAK;
	}else	if (strcmp(token->data, "elseif") == 0) {
		token->typ = RSELSEIF;
	}else	if (strcmp(token->data, "for") == 0) {
		token->typ = RSFOR;
	}else	if (strcmp(token->data, "in") == 0) {
		token->typ = RSIN;
	}else	if (strcmp(token->data, "not") == 0) {
		token->typ = RSNOT;
	}else	if (strcmp(token->data, "elseif") == 0) {
		token->typ = RSELSEIF;
	}else	if (strcmp(token->data, "or") == 0) {
		token->typ = RSOR;
	}else	if (strcmp(token->data, "repeat") == 0) {
		token->typ = RSREPEAT;
	}else	if (strcmp(token->data, "until") == 0) {
		token->typ = RSUNTIL;
	}else	if (strcmp(token->data, "sort") == 0) {
		token->typ = RSSORT;
	}else	if (strcmp(token->data, "find") == 0) {
		token->typ = RSFIND;
	}else	if (strcmp(token->data, "type") == 0) {
		token->typ = RSTYPE;
	}else	if (strcmp(token->data, "substr") == 0) {
		token->typ = RSSUBSTR;
	}
}

int ll_funkce (){
	printf("ll_funkce: vstup\n\n\n\n");
	chyba = dej_token();
	if (chyba != ERR_OK){
		return chyba;
	}
	if(token->typ == ENDOFFILE){
// pravidlo 2	<FUNKCE> -> Eps
		return ERR_OK;
	}else if (token->typ != TNFUNCTION){     
// pravidlo 1	<FUNKCE> -> function id ( PARAMETR ) DEKLARACE PŘÍKAZY end ; FUNKCE 
		return ERR_SYNTAX;
	}
	// id
	chyba = dej_token();
	if (chyba != ERR_OK){
		return chyba;
	}
	if (token->typ != IDKONEC) {
		return ERR_SYNTAX;
	}
	printf("prisel id - nazev fce\n");
	// vlozeni id funkce do TBS funkci
	
	BVSFunkceVloz(&strom_funkci, token->data, NULL);
	if (BVSFunkceNajdi(strom_funkci, token->data, &pom_uzel_funkce)){
		printf("redeklarace: ERR_SEMANT\n");
	}
	
	
	if ((pom_uzel_funkce->zasobnik = malloc(sizeof(struct sezPar)))== NULL){
		return ERR_INTERNI;
	}
	Sez_init_funkce(pom_uzel_funkce->zasobnik);

	if ((pom_uzel_funkce->koren = malloc(sizeof(struct bsuzel)))== NULL){
		return ERR_INTERNI;
	}
	BVSInit(&pom_uzel_funkce->koren);
	BVSInit(&pom_tab_sym);
	//printf("%d\n", *pom_uzel_funkce);
	// (
	chyba = dej_token();
	if (chyba != ERR_OK){
		return chyba;
	}
	if (token->typ != ZAVLEVA) {
		return ERR_SYNTAX;
	}
	// PARAMETR
	chyba = dej_token();
	if (chyba != ERR_OK){
		return chyba;
	}
	chyba=ll_parametr();
	if (chyba != ERR_OK){
		return chyba;
	}
	// )
	if (token->typ != ZAVPRAVA){
		return ERR_SYNTAX;
	}
	// DEKLARACE
	chyba = dej_token();
	if (chyba != ERR_OK){
		return chyba;
	}
	chyba=ll_deklarace();
	if (chyba != ERR_OK){
		return chyba;
	}
	// PRIKAZY
		printf("ll_funkce: po deklaraci\n");
	chyba=ll_prikazy();
		printf("ll_funkce: po prikazech eer: %d,  %d\n",chyba, token->typ);
	if (chyba != ERR_OK){
		return chyba;
	}
		
	if (token->typ != TNEND){
		printf("(mel by byt end)  token->typ: %d\n", token->typ);
		return ERR_SYNTAX;
	}
		printf("ll_funkce: prijato: end\n");
	chyba = dej_token();
	if (chyba != ERR_OK){
		return chyba;
	}
	if (token->typ != STREDNIK){
		return ERR_SYNTAX;
	}
	printf("ll_funkce: prijato: strednik\n");
	chyba=ll_funkce();		
	if (chyba!=ERR_OK){
		return chyba;
	}
	return chyba;
}

int ll_parametr(){
	printf("ll_parametr: vstup\n");
	// pravidlo 3	<PARAMETR> -> ID DALŠÍ_PARAMETR
	//nepovinne, tzn negeneruji errory
	if (token->typ == IDKONEC){	
		if (!najdi_prvek(pom_uzel_funkce->zasobnik, token->data)){ // pokud již je v tabulce -> redeklarace -> err
			return ERR_SEMANT;
		}else{
			printf("pred_vlozenim\n");
			insert_last(strom_funkci->data.zasobnik, token->data);
			(pom_uzel_funkce->pocet_param)++;
		}
	
		chyba = ll_dalsi_parametr();		
		if (chyba!=ERR_OK){
			return chyba;
		}
	}
	return chyba;
}

int ll_dalsi_parametr(){
	printf("ll_dalsi_parametr: vstup\n");
	// pravidlo 5	<DALŠÍ_PARAMETR> -> , ID DALŠÍ_PARAMETR
	// nepovinne, tzn negeneruji errory
	chyba = dej_token();		
	if (chyba!=ERR_OK){
		return chyba;
	}
	if (token->typ == CARKA){
		chyba = dej_token();		
		if (chyba!=ERR_OK){
			return chyba;
		}
		if (token->typ == IDKONEC){
			if (!najdi_prvek(pom_uzel_funkce->zasobnik, token->data)){ // pokud již je v tabulce -> redeklarace -> err
				return ERR_SEMANT;
			}else{
				printf("pred_vlozenim\n");
				insert_last(strom_funkci->data.zasobnik, token->data);
				(pom_uzel_funkce->pocet_param)++;
			}
			chyba = ll_dalsi_parametr();		
			if (chyba!=ERR_OK){
				return chyba;
			}
		}else{
			return ERR_SYNTAX;
		}
	}
	return chyba;
}

int ll_deklarace(){
	printf("ll_deklarace: vstup\n");
	// pravidlo 7	<DEKLARACE> -> local id INICIALIZACE ; DEKLARACE
	if (token->typ == TNLOCAL){
	
		chyba = dej_token();		
		if (chyba!=ERR_OK){
			return chyba;
		}
		if (token->typ == IDKONEC){
			printf("ll_deklarace: ID\n");
			if (!najdi_prvek(pom_uzel_funkce->zasobnik, token->data)){ // pokud již je v tabulce -> redeklarace -> err
				return ERR_SEMANT;
			}else{
				printf("pred_vlozenim\n");
				insert_last(strom_funkci->data.zasobnik, token->data);
				(pom_uzel_funkce->pocet_param)++;
			}
			printf("ll_deklarace: v tabulce\n");
			chyba = ll_inicializace();		
			if (chyba!=ERR_OK){
				return chyba;
			}
		}else {
			return ERR_SYNTAX;
		}
		
		if (token->typ != STREDNIK) {
			return ERR_SYNTAX;
		}
		chyba = dej_token();		
		if (chyba!=ERR_OK){
			return chyba;
		}
		chyba=ll_deklarace();				
		if (chyba!=ERR_OK){
			return chyba;
		}
	}
	return chyba;
}

int ll_inicializace(){
	printf("ll_inicializace: vstup\n");
// pravidlo 9	<INICIALIZACE> -> =VÝRAZ
// pravidlo 10	<INICIALIZACE> -> Eps
	chyba = dej_token();		
	if (chyba!=ERR_OK){
		return chyba;
	}
	if (token->typ != ROVNASEKONEC){
		return ERR_OK;
	}
	chyba = dej_token();	
	if (chyba!=ERR_OK){
		return chyba;
	}	
	/*
	switch(token->typ){
		case DESKONEC: 
		case INTKONEC: 
		case EXPKONEC:	obsah->typ = TDCISLO;
										obsah->data.dataCis = atof(token->data);
										break;
		case RETEZEC:	obsah->	typ = TDRETEZEC;
									Ret_alokuj(&(obsah->data.dataRet), token->delka);
									strcpy(obsah->data.dataRet,token->data);
									break;
		case TNTRUE:	obsah->typ = TDBOOL;
									obsah->data.dataBool = TRUE;
									break;
		case TNFALSE:	obsah->typ = TDBOOL;
									obsah->data.dataBool = FALSE;
									break;
		case TNNIL: break; // nil je implicitni
		
		
		default: return ERR_SEMANT;
	}
	*/
	zmen_data_par(pom_uzel_funkce->zasobnik, token->data, token->typ);
	chyba = dej_token();	
	if (chyba!=ERR_OK){
		return chyba;
	}	
	return chyba;
}

int ll_prikazy(){
	printf("ll_prikazy: vstup %d \n", token->typ);
	int *pom_pole;
	
	switch(token->typ){
		//case TNEND: printf("ll_prikazy: end: nic se nestane\n");break;
		//case TNELSE: printf("ll_prikazy: else: nic se nestane\n");return ERR_OK;break;
		case TNIF:	
		// pravidlo 11	<PŘÍKAZY> -> if VÝRAZ then PŘÍKAZY else PŘÍKAZY end ; PŘÍKAZY
		printf("\n\nprisel if\n");
								chyba = syntax_vyrazu();		
								if (chyba!=ERR_OK){
									return chyba;
								}
								printf("(ma prijit then) %d\n", token->typ);
								if (token->typ != TNTHEN){
									return ERR_SYNTAX;
								}
								
								chyba = dej_token();		
								if (chyba!=ERR_OK){
									return chyba;
								}
								printf("hura, budou prikazy\n");
								chyba = ll_prikazy();		
								if (chyba!=ERR_OK){
									printf("!!if: Prisel err z ll_prikazy... \n");
									return chyba;
								}
								printf("prikazy: if: po prikazech pred else %d\n", token->typ);
								if (token->typ != TNELSE){
									return ERR_SYNTAX;
								}
								chyba = dej_token();		
								if (chyba!=ERR_OK){
									return chyba;
								}
								chyba = ll_prikazy();		
								if (chyba!=ERR_OK){
									return chyba;
								}
								
								printf("ma byt end: %d\n", token->typ);
								
								if (token->typ != TNEND){
									return ERR_SYNTAX;
								}
								
								chyba = dej_token();		
								if (chyba!=ERR_OK){
									return chyba;
								}
								printf("ma byt strednik: %d\n", token->typ);
								if (token->typ != STREDNIK){
									return ERR_SYNTAX;
								}
								
								chyba = dej_token();		
								if (chyba!=ERR_OK){
									return chyba;
								}
								printf("pred dalsimi prikazy za if-else-end;: %d\n", token->typ);
								chyba = ll_prikazy();		
								if (chyba!=ERR_OK){
									return chyba;
								}
								break;
								
		// pravidlo 12	<PŘÍKAZY> -> id PŘÍKAZ_S_ID ; PŘÍKAZY
		case IDKONEC:	printf("ma byt ID %d\n",token->typ);
									chyba = ll_prikaz_s_id();	
									printf("prikaz_s_id: navrat %d\n", token->typ);	
									if (chyba!=ERR_OK){
										return chyba;
									}
									break;
		// pravidlo 13	<PŘÍKAZY> -> return VÝRAZ ; PŘÍKAZY
		case TNRETURN:	chyba = syntax_vyrazu();		
										if (chyba!=ERR_OK){
											return chyba;
										}
										
										if (token->typ != STREDNIK){
											return ERR_SYNTAX;
										}
										chyba = dej_token();		
										if (chyba!=ERR_OK){
											return chyba;
										}
										chyba = ll_prikazy();		
										if (chyba!=ERR_OK){
											return chyba;
										}
										break;
		// pravidlo 14	<PŘÍKAZY> -> while VÝRAZ do PŘÍKAZY end ; PŘÍKAZY
		case TNWHILE:	chyba = syntax_vyrazu();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != TNDO){
										return ERR_SYNTAX;
									}
						
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									chyba = ll_prikazy();		
									if (chyba!=ERR_OK){
										return chyba;
									}
						
									if (token->typ != TNEND){
										return ERR_SYNTAX;
									}
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != STREDNIK){
										return ERR_SYNTAX;
									}
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									chyba = ll_prikazy();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									break;
		case TNWRITE: chyba = dej_token();						
									if (chyba!=ERR_OK){
										return chyba;
									}
									if(token->typ != ZAVLEVA){
										return ERR_SYNTAX;
									}
									chyba = ll_write();
									if (chyba!=ERR_OK){
										return chyba;
									}
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									printf("write: ma byt strednik je: %d\n", token->typ);
									if (token->typ != STREDNIK){
										return ERR_SYNTAX;
									}
									//další příkazy
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									printf("write: pred dalsimi prikazy %d\n", token->typ);
									chyba = ll_prikazy();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									break;
		case TNREAD:  chyba = dej_token();						
									if (chyba!=ERR_OK){
										return chyba;
									}
									if(token->typ != ZAVLEVA){
										return ERR_SYNTAX;
									}
									
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != RETEZEC){
										return ERR_SYNTAX;
									}
									BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                  BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op3);
									BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                  BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op2);

									if ( (op2 = malloc( sizeof(TBSPolozka) )) == NULL){
										return ERR_INTERNI;
									}

                  if(( ( (UkTBSPolozka) op2)->data.dataRet = (char *) malloc(token->delka * sizeof(char)))==NULL){
										return ERR_INTERNI;
                  }
                  strcpy(((UkTBSPolozka) op2)->data.dataRet, token->data);
									Vloz_instrukci(seznam_instrukci, IN_READ, (UkTBSPolozka) op2, NULL, (UkTBSPolozka) op3);
									
									//TODO: pridat funkci generovani instrukce pro prirazeni do promenne
									// Vloz_instrukci(IN_PRIRAD, op3, NULL, vyhledana_promenna);
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != ZAVPRAVA){
										return ERR_SYNTAX;
									}
									
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != STREDNIK){
										return ERR_SYNTAX;
									}
									
									//další příkazy
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									chyba = ll_prikazy();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									break;
		case RSTYPE:  chyba = dej_token();						
									if (chyba!=ERR_OK){
										return chyba;
									}
									if(token->typ != ZAVLEVA){
										return ERR_SYNTAX;
									}
									
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != RETEZEC && token->typ != EXPKONEC && token->typ != DESKONEC && token->typ != INTKONEC && token->typ != IDKONEC){
										return ERR_SYNTAX;
									}
									BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                  BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op3);
									Vloz_instrukci(seznam_instrukci, IN_TYPE, token->data, NULL, op3);
									
									//TODO: pridat funkci generovani instrukce pro prirazeni do promenne
									// Vloz_instrukci(seznam_instrukci, IN_PRIRAD, op3, NULL, vyhledana_promenna);
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != ZAVPRAVA){
										return ERR_SYNTAX;
									}
									
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != STREDNIK){
										return ERR_SYNTAX;
									}
									
									//další příkazy
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									chyba = ll_prikazy();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									break;
		case RSFIND:  chyba = dej_token();						
									if (chyba!=ERR_OK){
										return chyba;
									}
									if(token->typ != ZAVLEVA){
										return ERR_SYNTAX;
									}
									//prvni parametr
									chyba = syntax_vyrazu();
									if (chyba!=ERR_OK){
										return chyba;
									}
									BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                  BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op1);
									if (token->typ != CARKA){
										return ERR_SYNTAX;
									}
									//druhy parametr
									chyba = syntax_vyrazu();
									if (chyba!=ERR_OK){
										return chyba;
									}
									BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                  BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op2);
									if (token->typ != ZAVPRAVA){
										return ERR_SYNTAX;
									}
									
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != STREDNIK){
										return ERR_SYNTAX;
									}
									BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                  BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op3);
                  Vloz_instrukci(seznam_instrukci, IN_FIND, op1, op2, op3);
									
									//TODO: pridat funkci generovani instrukce pro prirazeni do promenne
									// Vloz_instrukci(seznam_instrukci, IN_PRIRAD, op3, NULL, vyhledana_promenna);
									//další příkazy
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									chyba = ll_prikazy();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									break;		
		case RSSORT:  chyba = dej_token();						
									if (chyba!=ERR_OK){
										return chyba;
									}
									if(token->typ != ZAVLEVA){
										return ERR_SYNTAX;
									}
									//prvni parametr
									chyba = syntax_vyrazu(); // TODO: mozna jen retezec...
									if (chyba!=ERR_OK){
										return chyba;
									}
									BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                  BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op3);
                  Vloz_instrukci(seznam_instrukci, IN_SORT, token->data, NULL, op3);
									
									//TODO: pridat funkci generovani instrukce pro prirazeni do promenne
									// Vloz_instrukci(IN_PRIRAD, op3, NULL, vyhledana_promenna);
									//další příkazy
									if (token->typ != ZAVPRAVA){
										return ERR_SYNTAX;
									}
									
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != STREDNIK){
										return ERR_SYNTAX;
									}
									
									//další příkazy
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									chyba = ll_prikazy();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									break;
		case RSSUBSTR:  chyba = dej_token();						
										if (chyba!=ERR_OK){
											return chyba;
										}
										if(token->typ != ZAVLEVA){
											return ERR_SYNTAX;
										}
										if ((pom_pole = malloc(2 * sizeof(int))) == NULL){
	  									return ERR_INTERNI;
										}
										//prvni parametr
										chyba = dej_token();
										if (chyba!=ERR_OK){
											return chyba;
										}
										BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                  	BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op1);
										if (token->typ != RETEZEC){
											Ret_alokuj(&((UkTBSPolozka) op1)->data.dataRet, token->delka);
                  		strcpy(((UkTBSPolozka) op1)->data.dataRet, token->data);
										}else{
											op1 = NULL;
										}
										chyba = dej_token();
										if (chyba!=ERR_OK){
											return chyba;
										}
										if(token->typ != CARKA){
											return ERR_SYNTAX;
										}
										//druhy parametr
										chyba = dej_token();
										if (chyba!=ERR_OK){
											return chyba;
										}
										if (token->typ == MINUSKONEC){
											chyba = dej_token();
											if (chyba!=ERR_OK){
												return chyba;
											}
											if(token->typ == INTKONEC){
												pom_pole[0] = 0 - atof(token->data);
											}else{
												free(pom_pole);
												pom_pole = NULL;
											}
										}else{
											if(token->typ == INTKONEC){
												pom_pole[0] = atof(token->data);
											}else{
												free(pom_pole);
												pom_pole = NULL;
											}
										}
										chyba = dej_token();
										if (chyba!=ERR_OK){
											return chyba;
										}
										
										if(token->typ != CARKA){
											return ERR_SYNTAX;
										}
										//treti parametr
										chyba = dej_token();
										if (chyba!=ERR_OK){
											return chyba;
										}
										if (token->typ == MINUSKONEC){
											chyba = dej_token();
											if (chyba!=ERR_OK){
												return chyba;
											}
											if(pom_pole != NULL){
												if(token->typ == INTKONEC){
													pom_pole[0] = 0 - atof(token->data);
												}else{
													free(pom_pole);
													pom_pole = NULL;
												}
											}
										}else{
											if(pom_pole != NULL){
												if(token->typ == INTKONEC){
													pom_pole[0] = atof(token->data);
												}else{
													free(pom_pole);
													pom_pole = NULL;
												}
											}
										}
										BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                  	BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op3);
                  	Vloz_instrukci(seznam_instrukci, IN_SUBSTR, op1, pom_pole, op3);
									
										//TODO: pridat funkci generovani instrukce pro prirazeni do promenne
										// Vloz_instrukci(IN_PRIRAD, op3, NULL, vyhledana_promenna);
										chyba=dej_token();
										if (chyba!=ERR_OK){
											return chyba;
										}
										if (token->typ != ZAVPRAVA){
											return ERR_SYNTAX;
										}
									
										chyba = dej_token();		
										if (chyba!=ERR_OK){
											return chyba;
										}
										if (token->typ != STREDNIK){
											return ERR_SYNTAX;
										}
									
										//další příkazy
										chyba = dej_token();		
										if (chyba!=ERR_OK){
											return chyba;
										}
										chyba = ll_prikazy();		
										if (chyba!=ERR_OK){
											return chyba;
										}
										break;
	}//switch 
	printf("vracim prazdne prikazy %d \n", chyba);
	return ERR_OK;
}//fce

int ll_write(){
			printf("ll_write: vstup\n");
			chyba = syntax_vyrazu();
			if (chyba!=ERR_OK){
				if(chyba == ERR_PRAZDNY_VYRAZ){
					printf("prazdny vyraz ve write\n");
					chyba = ERR_SEMANT;
				}
				return chyba;
			}
			Vloz_instrukci(seznam_instrukci, IN_WRITE, op3, NULL, NULL);
			if(token->typ == CARKA){
					chyba = ll_write();
					if (chyba!=ERR_OK){
						return chyba;
					}
			}else if(token->typ != ZAVPRAVA){
				printf("neprislo uvnitr write\n");
					return ERR_SYNTAX;
			}
			
			return ERR_OK;
}

int ll_prikaz_s_id(){
	char * kam_priradit;
	if ((kam_priradit=malloc(token->delka * sizeof(char)))==NULL){
		return ERR_INTERNI;
	}
	printf("ll_prikazy_s_id: vstup s %d\n", token->typ);
	// pravidlo 16	<PŘÍKAZ_S_ID> -> ( volani_prvni_parametr )
	// pravidlo 17	<PŘÍKAZ_S_ID> -> = PŘÍKAZ_S_ID_A_ROVNASE
	chyba = dej_token();		
	if (chyba!=ERR_OK){
		free(kam_priradit);
		return chyba;
	}
	switch (token->typ){
		// pravidlo 16	<PŘÍKAZ_S_ID> -> ( PARAMETR )
		case ZAVLEVA: chyba=ll_volani_prvni_parametr();	//TODO: volani funkce
									free(kam_priradit);	
									if (chyba!=ERR_OK){
										return chyba;
									}
		
									if (token->typ != ZAVPRAVA){
										return ERR_SYNTAX;
									}
									break;
		// pravidlo 17	<PŘÍKAZ_S_ID> -> = PŘÍKAZ_S_ID_A_ROVNASE
		case ROVNASEKONEC:	printf("ll_prikaz_s_id: prislo = %d\n", token->typ);
												chyba = ll_prikaz_s_id_a_rovnase();		
												if(!BVSNajdi (pom_tab_sym, kam_priradit, op1)){
													//return ERR_SEMANT; // TODO: zkontrolovat logiku hledani
													printf("semanticka chyba\n");
												}
												Vloz_instrukci(seznam_instrukci, IN_PRIRAD, op3, NULL, op1);
												free(kam_priradit);
												if (chyba!=ERR_OK){
													return chyba;
												}
												break;
		default: printf("shazuju s err_syntax v prikaz_s_id");
							free(kam_priradit);
							return ERR_SYNTAX;
	}
	return ERR_OK;
}

int ll_prikaz_s_id_a_rovnase(){
	int *pom_pole;
	printf("ll_prikazy_s_id_a_rovnase: vstup\n");
	// pravidlo 18 <PŘÍKAZ_S_ID_A_ROVNASE> -> id ( volani_prvni_parametr )
	// pravidlo 19 <PŘÍKAZ_S_ID_A_ROVNASE> -> VYRAZ

	chyba = dej_token();
	if (chyba!=ERR_OK){
		return chyba;
	}
	printf("ll_prikazy_s_id_a_rovnase: token: %d\n", token->typ);
	
	switch (token->typ){
		//volani uzivatelske fce
		case IDKONEC:	chyba = dej_token();
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != ZAVLEVA){
										return ERR_SYNTAX;
									}
									
									chyba = ll_volani_prvni_parametr();
									if (chyba != ERR_OK){
										return chyba;
									}
									
									if (token->typ != ZAVPRAVA) {
										return ERR_SYNTAX;
									}
									printf("prisla prava ukoncujici zavorka\n");
									chyba = dej_token();
									if (chyba != ERR_OK){
										return chyba;
									}
									
									if (token->typ != STREDNIK){
										return ERR_SYNTAX;
									}	
									chyba = dej_token();
									if (chyba != ERR_OK){
										return chyba;
									}
									chyba = ll_prikazy();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									break;
		case TNWRITE: chyba = dej_token();						
									if (chyba!=ERR_OK){
										return chyba;
									}
									if(token->typ != ZAVLEVA){
										return ERR_SYNTAX;
									}
									chyba = ll_write();
									if (chyba!=ERR_OK){
										return chyba;
									}
									
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									printf("write: ma byt strednik je: %d\n", token->typ);
									if (token->typ != STREDNIK){
										return ERR_SYNTAX;
									}
									//další příkazy
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									
									BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                  BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op3);
                  if ( (op3 = (UkTBSPolozka) malloc( sizeof(TBSPolozka) )) == NULL){
										return ERR_INTERNI;
									}
									((UkTBSPolozka)op3)->typ = TDNIL;
									
									printf("write: pred dalsimi prikazy po write %d\n", token->typ);
									chyba = ll_prikazy();	
									printf("write: po dalsich prikazech chyba: %d\n", chyba)	;
									if (chyba!=ERR_OK){
										return chyba;
									}else
										return ERR_OK;
									break;
		case TNREAD:  chyba = dej_token();						
									if (chyba!=ERR_OK){
										return chyba;
									}
									if(token->typ != ZAVLEVA){
										return ERR_SYNTAX;
									}
									
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != RETEZEC){
										return ERR_SYNTAX;
									}
									BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                  BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op3);
									BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                  BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op2);

									if ( (op2 = malloc( sizeof(TBSPolozka) )) == NULL){
										return ERR_INTERNI;
									}

                  if(( ( (UkTBSPolozka) op2)->data.dataRet = (char *) malloc(token->delka * sizeof(char)))==NULL){
										return ERR_INTERNI;
                  }
                  strcpy(((UkTBSPolozka) op2)->data.dataRet, token->data);
									Vloz_instrukci(seznam_instrukci, IN_READ, (UkTBSPolozka) op2, NULL, (UkTBSPolozka) op3);
									
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != ZAVPRAVA){
										return ERR_SYNTAX;
									}
									
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != STREDNIK){
										return ERR_SYNTAX;
									}
									
									//další příkazy
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									chyba = ll_prikazy();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									break;
		case RSTYPE:  chyba = dej_token();						
									if (chyba!=ERR_OK){
										return chyba;
									}
									if(token->typ != ZAVLEVA){
										return ERR_SYNTAX;
									}
									
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != RETEZEC && token->typ != EXPKONEC && token->typ != DESKONEC && token->typ != INTKONEC && token->typ != IDKONEC){
										return ERR_SYNTAX;
									}
									BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                  BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op3);
									Vloz_instrukci(seznam_instrukci, IN_TYPE, token->data, NULL, op3);
									
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != ZAVPRAVA){
										return ERR_SYNTAX;
									}
									
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != STREDNIK){
										return ERR_SYNTAX;
									}
									
									//další příkazy
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									chyba = ll_prikazy();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									break;
		case RSFIND:  chyba = dej_token();						
									if (chyba!=ERR_OK){
										return chyba;
									}
									if(token->typ != ZAVLEVA){
										return ERR_SYNTAX;
									}
									//prvni parametr
									chyba = syntax_vyrazu();
									if (chyba!=ERR_OK){
										return chyba;
									}
									BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                  BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op1);
									if (token->typ != CARKA){
										return ERR_SYNTAX;
									}
									//druhy parametr
									chyba = syntax_vyrazu();
									if (chyba!=ERR_OK){
										return chyba;
									}
									BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                  BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op2);
									if (token->typ != ZAVPRAVA){
										return ERR_SYNTAX;
									}
									
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != STREDNIK){
										return ERR_SYNTAX;
									}
									BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                  BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op3);
                  Vloz_instrukci(seznam_instrukci, IN_FIND, op1, op2, op3);
									
									//další příkazy
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									chyba = ll_prikazy();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									break;		
		case RSSORT:  chyba = dej_token();						
									if (chyba!=ERR_OK){
										return chyba;
									}
									if(token->typ != ZAVLEVA){
										return ERR_SYNTAX;
									}
									//prvni parametr
									chyba = syntax_vyrazu(); // TODO: mozna jen retezec...
									if (chyba!=ERR_OK){
										return chyba;
									}
									BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                  BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op3);
                  Vloz_instrukci(seznam_instrukci, IN_SORT, token->data, NULL, op3);
									
									//další příkazy
									if (token->typ != ZAVPRAVA){
										return ERR_SYNTAX;
									}
									
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != STREDNIK){
										return ERR_SYNTAX;
									}
									
									//další příkazy
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									chyba = ll_prikazy();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									break;
		case RSSUBSTR:  chyba = dej_token();						
										if (chyba!=ERR_OK){
											return chyba;
										}
										if(token->typ != ZAVLEVA){
											return ERR_SYNTAX;
										}
										if ((pom_pole = malloc(2 * sizeof(int))) == NULL){
	  									return ERR_INTERNI;
										}
										//prvni parametr
										chyba = dej_token();
										if (chyba!=ERR_OK){
											return chyba;
										}
										BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                  	BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op1);
										if (token->typ != RETEZEC){
											Ret_alokuj(&((UkTBSPolozka) op1)->data.dataRet, token->delka);
                  		strcpy(((UkTBSPolozka) op1)->data.dataRet, token->data);
										}else{
											op1 = NULL;
										}
										chyba = dej_token();
										if (chyba!=ERR_OK){
											return chyba;
										}
										if(token->typ != CARKA){
											return ERR_SYNTAX;
										}
										//druhy parametr
										chyba = dej_token();
										if (chyba!=ERR_OK){
											return chyba;
										}
										if (token->typ == MINUSKONEC){
											chyba = dej_token();
											if (chyba!=ERR_OK){
												return chyba;
											}
											if(token->typ == INTKONEC){
												pom_pole[0] = 0 - atof(token->data);
											}else{
												free(pom_pole);
												pom_pole = NULL;
											}
										}else{
											if(token->typ == INTKONEC){
												pom_pole[0] = atof(token->data);
											}else{
												free(pom_pole);
												pom_pole = NULL;
											}
										}
										chyba = dej_token();
										if (chyba!=ERR_OK){
											return chyba;
										}
										
										if(token->typ != CARKA){
											return ERR_SYNTAX;
										}
										//treti parametr
										chyba = dej_token();
										if (chyba!=ERR_OK){
											return chyba;
										}
										if (token->typ == MINUSKONEC){
											chyba = dej_token();
											if (chyba!=ERR_OK){
												return chyba;
											}
											if(pom_pole != NULL){
												if(token->typ == INTKONEC){
													pom_pole[1] = 0 - atof(token->data);
												}else{
													free(pom_pole);
													pom_pole = NULL;
												}
											}
										}else{
											if(pom_pole != NULL){
												if(token->typ == INTKONEC){
													pom_pole[1] = atof(token->data);
												}else{
													free(pom_pole);
													pom_pole = NULL;
												}
											}
										}
										BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                  	BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op3);
                  	Vloz_instrukci(seznam_instrukci, IN_SUBSTR, op1, pom_pole, op3);
									
										//TODO: pridat funkci generovani instrukce pro prirazeni do promenne
										// Vloz_instrukci(IN_PRIRAD, op3, NULL, vyhledana_promenna);
										chyba=dej_token();
										if (chyba!=ERR_OK){
											return chyba;
										}
										if (token->typ != ZAVPRAVA){
											return ERR_SYNTAX;
										}
									
										chyba = dej_token();		
										if (chyba!=ERR_OK){
											return chyba;
										}
										if (token->typ != STREDNIK){
											return ERR_SYNTAX;
										}
									
										//další příkazy
										chyba = dej_token();		
										if (chyba!=ERR_OK){
											return chyba;
										}
										chyba = ll_prikazy();		
										if (chyba!=ERR_OK){
											return chyba;
										}
										break;
	}//switch 
	return ERR_OK;
}

int ll_volani_prvni_parametr(){
	printf("ll_volani_prvni_parametr: vstup\n");
// pravidlo 20 <volani_prvni_parametr> -> VYRAZ , VOLANI_dalsi_PARAMETR
	chyba = syntax_vyrazu();
	if (chyba == ERR_PRAZDNY_VYRAZ){
		printf("prvni parametr == nic\n");
		return ERR_OK;
	}else if(token->typ == CARKA){
		printf("prvni_parametr prijat, volam dalsi\n");
		chyba = ll_volani_dalsi_parametr();
		return chyba;
	}else{
		printf("prisel jeden parametr... zav prava: %d\n", token->typ);
		return ERR_OK;
	}
}

int ll_volani_dalsi_parametr(){
	printf("ll_volani_dalsi_parametr: vstup\n");
	chyba = syntax_vyrazu();
	printf("chyba z vyrazu %d\n", chyba);
	if (chyba != ERR_OK){
		return chyba;
	}
	if(token->typ == CARKA){
		printf("carka\n");
		chyba= ll_volani_dalsi_parametr();
		return chyba;
	}
	printf("konec\n");
	return ERR_OK;
}

int syntakticky_analyzator(){
	printf("syntakticky analyzator: vstup\n");
// rekurzivni metoda

	chyba = ll_funkce();
	return chyba;
}






// TODO ošetřit všechny výskyty chyba



















/*******************
 * Martina *********
 ******************/
 
 
 
 
 
int syntax_vyrazu() {

    TPrvek pom1, pom2, pom3, pom4;
    int znamenko;   // co se bude provadet, podle precedencni tabulky
    int uk_na_terminal;  // ukazatel na "nejvyssi" terminal
    bool novy_token_ano_ne = true;
    bool redukce = false;
    int err_chyba = ERR_OK;
    TPrvek pom_prvek;

    zasobnik_vynuluj(&zasobnik);
    
    pom_prvek.typ = DOLAR;

    if(zasobnik_push(&zasobnik, pom_prvek) == ERR_INTERNI) {
        //err_chyba = ERR_INTERNI;
        return ERR_INTERNI;
    }

    uk_na_terminal = zasobnik.top;   // nejvyssi terminal je ted $

    dej_token(soubor, token);   // otestovat navratovy kod



//////////////////////////////////////////////
// while(/*  nacitam to,co je soucasti meho mileho vyrazu */) {

    while((redukce == true) ||
        (token->typ == IDKONEC) || (token->typ == INTKONEC) || (token->typ == DESKONEC) ||
        (token->typ == EXPKONEC) || (token->typ == RETEZEC) || (token->typ == ZAVLEVA) ||
        (token->typ == ZAVPRAVA) || (token->typ == PLUS) || (token->typ == MINUSKONEC) ||
        (token->typ == KRAT) || (token->typ == DELENO) || (token->typ == MOCNINA) ||
        (token->typ == KONKATENACE) || (token->typ == POROVNANI) || (token->typ == NEROVNASE) ||
        (token->typ == VETSIROVNO) || (token->typ == VETSITKOKONEC) || (token->typ == MENSIROVNO) ||
        (token->typ == MENSITKOKONEC) || (token->typ == TNNIL) || (token->typ == TNFALSE) ||
        (token->typ == TNTRUE) || (znamenko == VETSI)) {



// testovat return funkce ZISKEJ_DALSI_TOKEN ptz by to mohla byt lex chyba !!!!!!!!

// TODO Kontrola vyrazu !!!!!!!!!!!



/////////////////////////////////////////////////////////////////



        if(redukce == true) {
            znamenko = VETSI;
        }

        else {
            pom1.typ = preved_z_tokenu(token->typ);
            znamenko = preced_tabulka[preved_z_tokenu(zasobnik.array[uk_na_terminal].typ)][pom1.typ];
        }


        if(znamenko == PTCHYBA) {
        
        		    if(zasobnik_pristup(&zasobnik, &pom2, 0) == ERR_INTERNI) {
        					//err_chyba = ERR_INTERNI;
        					return ERR_INTERNI;
    						}


    						// pokud byl vyraz prazdny, tzn ze prvni precteny token nepatril do vyrazu, tak je to OK
    							if((pom2.typ == DOLAR) && (zasobnik.top == 0)) {
        					printf("Prazdny vyraz se zavorkou\n");
        					return ERR_PRAZDNY_VYRAZ;
    							}
        
        			if(preved_z_tokenu(token->typ) == OPPRAVAZAVORKA) {
        			return ERR_OK;
        		}
        		
            return ERR_SYNTAX;
            
            break;
        }


        else if(znamenko == ROVNO) {

				    pom_prvek.typ = token->typ;
				    
				    if(token->typ == IDKONEC) {
				    	if(!BVSNajdi(pom_uzel_funkce->koren, token->data, obsah)) {
							printf("Spatna tabulka symbolu1\n");
				    		return ERR_SEMANT;
				    	}
				    	pom_prvek.uk_na_prvek_ts = obsah;
				    }
				    
				    // Vlozeni do pomocne tabulky symbolu
				    // Ulozeni typu a odkazu do tabulky symbolu na zasobnik
				    else if((token->typ == INTKONEC) || (token->typ == DESKONEC) || (token->typ == EXPKONEC)) {
        				obsah->data.dataCis = atof(token->data);
        				obsah->typ = TDCISLO;
        				BVSVloz(&pom_tab_sym, generuj_klic(0), obsah);
        				pom_prvek.uk_na_prvek_ts = obsah;		
        		}
        		
        		else if(token->typ == RETEZEC) {
        				Ret_alokuj(&(obsah->data.dataRet), token->delka);
								strcpy(obsah->data.dataRet,token->data);
        				obsah->typ = TDRETEZEC;
        				BVSVloz(&pom_tab_sym, generuj_klic(0), obsah);
        				pom_prvek.uk_na_prvek_ts = obsah;		
        		}
        		
        		else if(token->typ == TNTRUE) {
        				obsah->typ = TDBOOL;
								obsah->data.dataBool = TRUE;
								BVSVloz(&pom_tab_sym, generuj_klic(0), obsah);
								pom_prvek.uk_na_prvek_ts = obsah;
        		}
        		
        		else if(token->typ == TNFALSE) {
        				obsah->typ = TDBOOL;
								obsah->data.dataBool = FALSE;
								BVSVloz(&pom_tab_sym, generuj_klic(0), obsah);
								pom_prvek.uk_na_prvek_ts = obsah;
        		}
        		
        		if(zasobnik_push(&zasobnik, pom_prvek) == ERR_INTERNI) {
                //err_chyba = ERR_INTERNI;
                return ERR_INTERNI;
            }


            uk_na_terminal = zasobnik.top;   // pozice terminalu - je to ten na vrcholu zasobniku
            novy_token_ano_ne = true;
        }


        else if(znamenko == MENSI) {

            // za posledni terminal vloz "<"
            pom_prvek.typ = MENSI;
            
            if(zasobnik_push(&zasobnik, pom_prvek) == ERR_INTERNI) {
                //err_chyba = ERR_INTERNI;
                return ERR_INTERNI;
            }


            for(int i = zasobnik.top - 1 ;i >= 0 ;i--) {

                if(i == uk_na_terminal) {
                    break;
                }

                else {  // prohozeni "<" s predeslym (neterminalnim) znakem
                        // znak "<" musi probublat az za nejvyssi terminal
                    pom4 = zasobnik.array[i];
                    zasobnik.array[i] = zasobnik.array[i+1];
                    zasobnik.array[i+1] = pom4;
                }

            }

						pom_prvek.typ = token->typ;
				    
				    if(token->typ == IDKONEC) {
				    	if(!BVSNajdi(pom_uzel_funkce->koren, token->data, obsah)) {
							printf("Spatna tabulka symbolu2\n");
				    		return ERR_SEMANT;
				    	}
				    	pom_prvek.uk_na_prvek_ts = obsah;
				    }
				    
				    // Vlozeni do pomocne tabulky symbolu
				    // Ulozeni typu a odkazu do tabulky symbolu na zasobnik
				    else if((token->typ == INTKONEC) || (token->typ == DESKONEC) || (token->typ == EXPKONEC)) {
        				obsah->data.dataCis = atof(token->data);
        				obsah->typ = TDCISLO;
        				BVSVloz(&pom_tab_sym, generuj_klic(0), obsah);
        				pom_prvek.uk_na_prvek_ts = obsah;		
        		}
        		
        		else if(token->typ == RETEZEC) {
        				Ret_alokuj(&(obsah->data.dataRet), token->delka);
								strcpy(obsah->data.dataRet,token->data);
        				obsah->typ = TDRETEZEC;
        				BVSVloz(&pom_tab_sym, generuj_klic(0), obsah);
        				pom_prvek.uk_na_prvek_ts = obsah;		
        		}
        		
        		else if(token->typ == TNTRUE) {
        				obsah->typ = TDBOOL;
								obsah->data.dataBool = TRUE;
								BVSVloz(&pom_tab_sym, generuj_klic(0), obsah);
								pom_prvek.uk_na_prvek_ts = obsah;
        		}
        		
        		else if(token->typ == TNFALSE) {
        				obsah->typ = TDBOOL;
								obsah->data.dataBool = FALSE;
								BVSVloz(&pom_tab_sym, generuj_klic(0), obsah);
								pom_prvek.uk_na_prvek_ts = obsah;
        		}
        		
        		if(zasobnik_push(&zasobnik, pom_prvek) == ERR_INTERNI) {
                //err_chyba = ERR_INTERNI;
                return ERR_INTERNI;
            }


            uk_na_terminal = zasobnik.top;   // pozice terminalu - je to ten na vrcholu zasobniku
            novy_token_ano_ne = true;

        }


        else if(znamenko == VETSI) {    // hledame pravidlo


            if(zasobnik_pristup(&zasobnik, &pom1, 0) == ERR_INTERNI) {
                //err_chyba = ERR_INTERNI;
                return ERR_INTERNI;
            }

            pom1.typ = preved_z_tokenu(pom1.typ);

            // pokud se pri redukci prvek na zasobniku nerovna nicemu z case pod, tak to musim breaknout,
            // ptz se neda redukovat podle zadnyho pravidla !!!
            if((pom1.typ != OPIDENTIFIKATOR) && (pom1.typ != NETERMINAL) && (pom1.typ != OPPRAVAZAVORKA)) {
                printf("Neodpovida zadnemu pravidlu --> BREAK\n");
                //err_chyba = ERR_SYNTAX;
                //break;
                return ERR_SYNTAX;
            }


            switch(pom1.typ) {

                case OPIDENTIFIKATOR:    // redukce podle pravidla E -> i
                //case OPINTEGER:
                //case OPDESETINNE:
                //case OPEXPCISLO:
                //case OPRETEZEC:
                //case OPTRUE
                //case OPFALSE
                //case OPNIL


                    if(zasobnik_pristup(&zasobnik, &pom1, 1) == ERR_INTERNI) {
                        //err_chyba = ERR_INTERNI;
                        return ERR_INTERNI;
                    }


                    if(pom1.typ == MENSI) {
                    
                    		if(zasobnik_pristup(&zasobnik, &prvek_pomocny, 0)) {
                    				//err_chyba = ERR_INTERNI;
                            return ERR_INTERNI;
                        } 
                        
                        pom_prvek.uk_na_prvek_ts = prvek_pomocny.uk_na_prvek_ts;

                        if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                            //err_chyba = ERR_INTERNI;
                            return ERR_INTERNI;
                        }

                        if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                            //err_chyba = ERR_INTERNI;
                            return ERR_INTERNI;
                        }
													
												pom_prvek.typ = NETERMINAL;
													
                        if(zasobnik_push(&zasobnik, pom_prvek) == ERR_INTERNI) {
                            //err_chyba = ERR_INTERNI;
                            return ERR_INTERNI;
                        }

                    }

                    else {
                        //err_chyba = ERR_SYNTAX;
                        printf("Nestvi3\n");
                        //break;
                        return ERR_SYNTAX;
                    }


                    break;


                case OPPRAVAZAVORKA:        // redukce podle pravidla E -> (E)

                    if(zasobnik_pristup(&zasobnik, &pom1, 1) == ERR_INTERNI) {
                        //err_chyba = ERR_INTERNI;
                        return ERR_INTERNI;
                    }

                    if(zasobnik_pristup(&zasobnik, &pom2, 2) == ERR_INTERNI) {
                        //err_chyba = ERR_INTERNI;
                        return ERR_INTERNI;
                    }

                    if(zasobnik_pristup(&zasobnik, &pom3, 3) == ERR_INTERNI) {
                        //err_chyba = ERR_INTERNI;
                        return ERR_INTERNI;
                    }

                    pom2.typ = preved_z_tokenu(pom2.typ);

                    if((pom1.typ == NETERMINAL) && (pom2.typ == OPLEVAZAVORKA) && (pom3.typ == MENSI)) {
                    
                    		if(zasobnik_pristup(&zasobnik, &prvek_pomocny, 1)) {
                    				//err_chyba = ERR_INTERNI;
                            return ERR_INTERNI;
                        } 
                        
                        pom_prvek.uk_na_prvek_ts = prvek_pomocny.uk_na_prvek_ts;

                        if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                            //err_chyba = ERR_INTERNI;
                            return ERR_INTERNI;
                        }

                        if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                            //err_chyba = ERR_INTERNI;
                            return ERR_INTERNI;
                        }

                        if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                            //err_chyba = ERR_INTERNI;
                            return ERR_INTERNI;
                        }

                        if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                            //err_chyba = ERR_INTERNI;
                            return ERR_INTERNI;
                        }
                        
                        

												pom_prvek.typ = NETERMINAL;
												
													
                        if(zasobnik_push(&zasobnik, pom_prvek) == ERR_INTERNI) {
                            //err_chyba = ERR_INTERNI;
                            return ERR_INTERNI;
                        }

                    }

                    else {
                        //err_chyba = ERR_SYNTAX;
                        printf("Nestvi1\n");
                        //break;
                        return ERR_SYNTAX;
                    }

                    break;


                case NETERMINAL:            // redukce podle pravidla E -> E + E atd.

                    zasobnik_pristup(&zasobnik, &pom1, 1);
                    pom1.typ = preved_z_tokenu(pom1.typ);

                    if((pom1.typ != OPPLUS) && (pom1.typ != OPMINUS) && (pom1.typ != OPKRAT) &&
                        (pom1.typ != OPDELENO) && (pom1.typ != OPMOCNINA) && (pom1.typ != OPJEROVNO) &&
                        (pom1.typ != OPNENIROVNO) && (pom1.typ != OPMENSITKO) && (pom1.typ != OPVETSITKO) &&
                        (pom1.typ != OPMENSITKOROVNO) && (pom1.typ != OPVETSITKOROVNO) && (pom1.typ != OPKONKATENACE)) {

                        printf("Neodpovida zadnemu pravidlu --> BREAK\n");
                        //err_chyba = ERR_SYNTAX;
                        //break;
                        return ERR_SYNTAX;
                    }

                    switch(pom1.typ) {

                        case OPPLUS:
                        case OPMINUS:
                        case OPKRAT:
                        case OPDELENO:
                        case OPMOCNINA:
                        case OPJEROVNO:
                        case OPNENIROVNO:
                        case OPMENSITKO:
                        case OPVETSITKO:
                        case OPMENSITKOROVNO:
                        case OPVETSITKOROVNO:
                        case OPKONKATENACE:
                        
                        
                        		switch(pom1.typ) {
                        		
                        				case OPPLUS: typ_instrukce = IN_ADD; break;
                        				case OPMINUS: typ_instrukce = IN_SUB; break;
                        				case OPKRAT: typ_instrukce = IN_MUL; break;
                        				case OPDELENO: typ_instrukce = IN_DIV; break;
                        				case OPMOCNINA: typ_instrukce = IN_MOCN; break;
                        				case OPJEROVNO: typ_instrukce = IN_ROVNO; break;
                        				case OPNENIROVNO: typ_instrukce = IN_NEROVNO; break;
                        				case OPMENSITKO: typ_instrukce = IN_MENSI; break;
                        				case OPVETSITKO: typ_instrukce = IN_VETSI; break;
                        				case OPMENSITKOROVNO: typ_instrukce = IN_MENROV; break;
                        				case OPVETSITKOROVNO: typ_instrukce = IN_VETROV; break;
                        				case OPKONKATENACE: typ_instrukce = IN_KONK; break;
                        		
                        		}
                        		

                            if(zasobnik_pristup(&zasobnik, &pom1, 2) == ERR_INTERNI) {
                                //err_chyba = ERR_INTERNI;
                                return ERR_INTERNI;
                            }

                            if(zasobnik_pristup(&zasobnik, &pom2, 3) == ERR_INTERNI) {
                                //err_chyba = ERR_INTERNI;
                                return ERR_INTERNI;
                            }

                            if((pom1.typ == NETERMINAL) && (pom2.typ == MENSI)) {
                            
                            		//Vloz_instrukci(seznam instrukci, typ_instrukce, void *op1, void *op2, void *op3);
                            

																if(zasobnik_pristup(&zasobnik, &prvek_pomocny, 0)) {
                    								//err_chyba = ERR_INTERNI;
                            				return ERR_INTERNI;
                        				} 
                        				op2 = prvek_pomocny.uk_na_prvek_ts;
                        				
                        				if(zasobnik_pristup(&zasobnik, &prvek_pomocny, 2)) {
                    								//err_chyba = ERR_INTERNI;
                            				return ERR_INTERNI;
                        				} 
                        				op1 = prvek_pomocny.uk_na_prvek_ts;
                        				
                        				
                        				
                        				                       				
                        				BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
                        				BVSNajdi(pom_tab_sym, generuj_klic(1), (UkTBSPolozka) op3);
                        				
                        				
                        				
                        				Vloz_instrukci(seznam_instrukci, typ_instrukce, &op1, &op2, &op3);
                        				
                        				
                        

                                if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                                    //err_chyba = ERR_INTERNI;
                                    return ERR_INTERNI;
                                }

                                if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                                    //err_chyba = ERR_INTERNI;
                                    return ERR_INTERNI;
                                }

                                if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                                    //err_chyba = ERR_INTERNI;
                                    return ERR_INTERNI;
                                }

                                if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                                    //err_chyba = ERR_INTERNI;
                                    return ERR_INTERNI;
                                }

																pom_prvek.typ = NETERMINAL;
																pom_prvek.uk_na_prvek_ts = op3;        
													
								                if(zasobnik_push(&zasobnik, pom_prvek) == ERR_INTERNI) {
                                    //err_chyba = ERR_INTERNI;
                                    return ERR_INTERNI;
                                }

                            }

                            else {
                                //err_chyba = ERR_SYNTAX;
                                printf("Nestvi1\n");
                                //break;
                                return ERR_SYNTAX;
                            }

                            break;

                    }

                    break;

            }   // konec vetsiho switch

            // pokud je err_chyba, break; z while
            if(err_chyba != ERR_OK) {
                printf("NASTALA CHYBA\n");
                return err_chyba;
            }

            // nastavime novy nejvyssi terminal
            for(int j = 0; j <= zasobnik.top; j++) {
                zasobnik_pristup(&zasobnik, &pom1, j);
                pom1.typ = preved_z_tokenu(pom1.typ);

                if((pom1.typ != NETERMINAL) && (pom1.typ != MENSI)) {
                    uk_na_terminal = zasobnik.top - j;
                    break;
                }
            }


            novy_token_ano_ne = false;

        }   // konec else if(znamenko == VETSI)


        if(novy_token_ano_ne == true) {
            dej_token(soubor, token);   // otestovat navratovy kod
        }

        // PSAT KOMENTY K PRAVIDLUM


        // pokud je vse u konce,  breaknem to
        if(((token->typ != IDKONEC) && (token->typ != INTKONEC) && (token->typ != DESKONEC) &&
            (token->typ != EXPKONEC) && (token->typ != RETEZEC) && (token->typ != ZAVLEVA) &&
            (token->typ != ZAVPRAVA) && (token->typ != PLUS) && (token->typ != MINUSKONEC) &&
            (token->typ != KRAT) && (token->typ != DELENO) && (token->typ != MOCNINA) &&
            (token->typ != KONKATENACE) && (token->typ != POROVNANI) && (token->typ != NEROVNASE) &&
            (token->typ != VETSIROVNO) && (token->typ != VETSITKOKONEC) && (token->typ != MENSIROVNO) &&
            (token->typ != MENSITKOKONEC) && (token->typ != TNNIL) && (token->typ != TNFALSE) &&
            (token->typ != TNTRUE)) &&
            (zasobnik.top == 1) && (zasobnik.array[1].typ == NETERMINAL) && (zasobnik.array[0].typ == DOLAR)) {

            printf("OK\n");
            return ERR_OK;
        }


        redukce = false;

        // POKUD DALSI NACTENY ZNAK NAM UZ NEPATRI,
        // ALE JA POTREBUJI REDUKOVAT PODLE PRAVIDEL,
        // MUSI BYT PODMINKA
        if(((token->typ != IDKONEC) && (token->typ != INTKONEC) && (token->typ != DESKONEC) &&
            (token->typ != EXPKONEC) && (token->typ != RETEZEC) && (token->typ != ZAVLEVA) &&
            (token->typ != ZAVPRAVA) && (token->typ != PLUS) && (token->typ != MINUSKONEC) &&
            (token->typ != KRAT) && (token->typ != DELENO) && (token->typ != MOCNINA) &&
            (token->typ != KONKATENACE) && (token->typ != POROVNANI) && (token->typ != NEROVNASE) &&
            (token->typ != VETSIROVNO) && (token->typ != VETSITKOKONEC) && (token->typ != MENSIROVNO) &&
            (token->typ != MENSITKOKONEC) && (token->typ != TNNIL) && (token->typ != TNFALSE) &&
            (token->typ != TNTRUE)) && (zasobnik.top > 1)) {

            redukce = true;

        }



    }   // konec while



    if(zasobnik_pristup(&zasobnik, &pom2, 0) == ERR_INTERNI) {
        //err_chyba = ERR_INTERNI;
        return ERR_INTERNI;
    }


    // pokud byl vyraz prazdny, tzn ze prvni precteny token nepatril do vyrazu, tak je to OK
    if((pom2.typ == DOLAR) && (zasobnik.top == 0)) {
        printf("Prazdny vyraz\n");
        return ERR_PRAZDNY_VYRAZ;
    }
    
    if(zasobnik_pristup(&zasobnik, &pom2, 1) == ERR_INTERNI) {
        return ERR_INTERNI;
    }

    if(zasobnik_pristup(&zasobnik, &pom1, 0) == ERR_INTERNI) {
        return ERR_INTERNI;
    }
    

		// pokud uz na vstupni pasce nic neni, ale na zasobniku je neco jineho nez $E,
    // tak je to synt chyba
    if(!((pom1.typ == NETERMINAL) && (pom2.typ == DOLAR) && (zasobnik.top == 1))) {
        //err_chyba = ERR_SYNTAX;
        printf("CHYBA\n");
        return ERR_SYNTAX;
    }


   return err_chyba;

}  // konec funkce syntax_vyrazu()
