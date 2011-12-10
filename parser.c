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


int prisel_main = FALSE;
extern UkTToken token;
UkTToken token2;
extern int chyba;
extern FILE *soubor;
extern TZasobnikInt zas_int;
extern TZasobnikUk zas_uk;
extern UkTBSUzel pom_tab_sym;
extern UkTBSUzel nazvy_funkci; // kam odkazuje navesti
extern UkTBSPolozka obsah;  // promenna pro vkladani do TS
UkTBSPolozka navratova_hodnota; // vyuzije se pro ukladani ukazatele pri hledani v tabulce symbolu
extern UkTSezInstr seznam_instrukci;
extern unsigned int klic_cislo;
extern int typ_instrukce;
extern UkTBSPolozka op1; 
extern UkTBSPolozka op2; 
extern UkTBSPolozka op3;
bool uz_nacteno=FALSE;
UkTPlzkaSez adresa_konce;

// ---- nove pridano
extern UkTBSFunkce strom_funkci;
extern UkTBSFunkPol uzel_aktualni_funkce;
extern UkTBSFunkPol pom_uzel_funkce;
extern char *pom_token_data;
extern char * gen_klic;
// --- konec novyho


UkTBSUzel pole_stromu;
int delka_pole_stromu;

extern UkTZasAdr zas_navr_adres;
extern UkTSezPar zas_zpracovani;

extern UkTBSPolozka *uk_na_zasobnik;


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
void generuj_klic(int posun, char **klic) {
		free(*klic);
    *klic = malloc(sizeof(char) * 20); // 20 je cislo, ktere pokryje cely rozsah klic_cislo
    sprintf(*klic, "pom_%u", (klic_cislo++)-posun);
    //return klic;
}

// funkce pro vycisteni promenne typu UkTBSPolozka

void obnov(UkTBSPolozka *promenna){
	if((*promenna)->typ == TDRETEZEC){
		free((*promenna)->data.dataRet);
	}
	free(*promenna);
	
	*promenna = malloc(sizeof(struct bsdata));
	
	(*promenna)->typ = TDNIL;
	
	return;
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
	if (token!= NULL){
		token_uvolni(token);
	}
	chyba=token_alokuj(&token);
	if (chyba!=ERR_OK){
		return chyba;
	}
	
	chyba = ziskej_dalsi_token(soubor,token);
	if(chyba != ERR_OK){
		return chyba;
	}
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
	}else	if (strcmp(token->data, "main") == 0) {
		token->typ = RSMAIN;
	}
}

int ll_funkce (){
	printf("ll_funkce: vstup\n\n\n\n");
	chyba = dej_token();
	if (chyba != ERR_OK){
		return chyba;
	}
	if(token->typ == ENDOFFILE && prisel_main == TRUE){
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
	if (token->typ == RSMAIN){
		prisel_main = TRUE;
	}else if (token->typ != IDKONEC) {
		return ERR_SYNTAX;
	}
	printf("prisel id - nazev fce\n");
	// vlozeni id funkce do TBS funkci
	
	printf("vlozena instrukce\n");
	
	BVSFunkceVloz(&strom_funkci, token->data, NULL);
	if (BVSFunkceNajdi(strom_funkci, token->data, &uzel_aktualni_funkce)){
		printf("redeklarace: ERR_SEMANT\n");
	}
	
	Vloz_instrukci(seznam_instrukci, IN_NVSTI, NULL, NULL, NULL ); // navesti pro dalsi cyklus
	uzel_aktualni_funkce->adresa = Sez_vrat_uk_posledni(seznam_instrukci);	
	
	if(prisel_main == TRUE){
		Vloz_instrukci(seznam_instrukci, IN_PUSH, uzel_aktualni_funkce, adresa_konce, NULL);
		
	}
	
	if ((uzel_aktualni_funkce->zasobnik = malloc(sizeof(struct sezPar)))== NULL){
		return ERR_INTERNI;
	}
	Sez_init_funkce(uzel_aktualni_funkce->zasobnik);
/*
	if ((uzel_aktualni_funkce->koren = malloc(sizeof(struct bsuzel)))== NULL){
		return ERR_INTERNI;
	}*/
	BVSInit(&uzel_aktualni_funkce->koren);
	BVSInit(&pom_tab_sym);
	//printf("%d\n", *uzel_aktualni_funkce);
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
	if (prisel_main == TRUE){
		chyba = dej_token();
		if (chyba != ERR_OK){
			return chyba;
		}
		if (token->typ != STREDNIK){
			return ERR_SYNTAX;
		}
		Vloz_instrukci(seznam_instrukci, IN_GOTO, adresa_konce, NULL, NULL);
		printf("ll_funkce: prijato: strednik\n");
	}
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
		printf("identifikator: %d\n", token->typ);
		if (!najdi_prvek_lok(uzel_aktualni_funkce->zasobnik, token->data)){ // pokud již je v tabulce -> redeklarace -> err
			return ERR_SEMANT;
		}else{
			printf("pred_vlozenim\n");
			insert_last(uzel_aktualni_funkce->zasobnik, token->data);
			printf("vlozeno do zasobniku\n");
			(uzel_aktualni_funkce->pocet_param)++;
			//BVSVloz(&(strom_funkci->data.koren), token->data, NULL);
			printf("vlozeno do bvs\n");
		}
	
			printf("za podminkou bvsnajdi\n");
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
			if (!najdi_prvek_lok(uzel_aktualni_funkce->zasobnik, token->data)){ // pokud již je v tabulce -> redeklarace -> err
				return ERR_SEMANT;
			}else{
				printf("pred_vlozenim\n");
				insert_last(uzel_aktualni_funkce->zasobnik, token->data);
				(uzel_aktualni_funkce->pocet_param)++;
				BVSVloz(&(strom_funkci->data->koren), token->data, NULL);
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
			if (!BVSNajdi(uzel_aktualni_funkce->koren, token->data, NULL)){ // pokud již je v tabulce -> redeklarace -> err
				return ERR_SEMANT;
			}else{
				printf("pred_vlozenim\n");
				BVSVloz(&(uzel_aktualni_funkce->koren), token->data, NULL);
			}
			printf("ll_deklarace: v tabulce\n");
			
			char *pom_nazev;
			if((pom_nazev = malloc((strlen(token->data)+1) * sizeof(char))) == NULL){
				free(pom_nazev);
				return ERR_INTERNI;
			}
			strcpy(pom_nazev, token->data);
			chyba = ll_inicializace(pom_nazev);	
			free(pom_nazev);
				
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
	printf("-------------------------vypsani cele tabulky symbolu: \n");
									BVSVypisStrom(&uzel_aktualni_funkce->koren);
	return chyba;
}

int ll_inicializace(char *nazev){
	

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

	obnov(&obsah);
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
	/*
	if ((obsah = malloc(sizeof(struct bsdata)))==NULL){
		return ERR_INTERNI;
	}*/
	
	BVSVloz(&(uzel_aktualni_funkce->koren), nazev, obsah);
	chyba = dej_token();	
	if (chyba!=ERR_OK){
		return chyba;
	}	
	return chyba;
}

int ll_prikazy(){
	printf("ll_prikazy: vstup %d \n", token->typ);
	int *pom_pole;
	UkTPlzkaSez adresa1 = NULL;
	UkTPlzkaSez adresa2 = NULL;
	
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
								
								Vloz_instrukci(seznam_instrukci, IN_PGOTO, (UkTBSPolozka) op3, NULL, NULL );
								adresa1 = Sez_vrat_uk_posledni(seznam_instrukci);
								
								
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
														
								Vloz_instrukci(seznam_instrukci, IN_GOTO, NULL, NULL, NULL ); // skok za podminku
								adresa2 = Sez_vrat_uk_posledni(seznam_instrukci);
								
								Vloz_instrukci(seznam_instrukci, IN_NVSTI, NULL, NULL, NULL ); // generuje se navesti pro else
								adresa1->instrukce.op2 = Sez_vrat_uk_posledni(seznam_instrukci); // na ktere se bude (mozna) skakat
								
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
								
								Vloz_instrukci(seznam_instrukci, IN_NVSTI, NULL, NULL, NULL ); // generuje se navesti pro end
								adresa2->instrukce.op1 = Sez_vrat_uk_posledni(seznam_instrukci); // na ktere se bude skakat na konci bloku if
								
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
										
										//op3
										navratova_hodnota = op3;
										// TODO: zjistit jestli je v tehle chvili uzel_aktualni funkce spravne nastaven
										Vloz_instrukci(seznam_instrukci, IN_POP, uzel_aktualni_funkce, adresa2, NULL ); // navesti pro dalsi cyklus//TODO: doplnit parametry
										Vloz_instrukci(seznam_instrukci, IN_GOTO, adresa2, NULL, NULL ); // navesti pro dalsi cyklus//TODO: doplnit parametry
										
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
		case TNWHILE:	Vloz_instrukci(seznam_instrukci, IN_NVSTI, NULL, NULL, NULL ); // navesti pro dalsi cyklus
									adresa1 = Sez_vrat_uk_posledni(seznam_instrukci);
									
									chyba = syntax_vyrazu();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									if (token->typ != TNDO){
										return ERR_SYNTAX;
									}
								
									Vloz_instrukci(seznam_instrukci, IN_PGOTO, (UkTBSPolozka) op3, NULL, NULL );
									adresa2 = Sez_vrat_uk_posledni(seznam_instrukci);
						
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
																	
									Vloz_instrukci(seznam_instrukci, IN_GOTO, adresa1, NULL, NULL ); // skok na zacatek									
									
									Vloz_instrukci(seznam_instrukci, IN_NVSTI, NULL, NULL, NULL ); // navesti pro konec cyklu
									adresa2->instrukce.op2 = Sez_vrat_uk_posledni(seznam_instrukci);
									
									chyba = ll_prikazy();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									break;
		// pravidlo 14	<PŘÍKAZY> -> repeat PŘÍKAZY until VÝRAZ ; PŘÍKAZY
		case RSREPEAT:	Vloz_instrukci(seznam_instrukci, IN_NVSTI, NULL, NULL, NULL ); // navesti pro dalsi cyklus
										adresa1 = Sez_vrat_uk_posledni(seznam_instrukci);

										chyba = dej_token();
										if (chyba!=ERR_OK){
											return chyba;
										}
										chyba = ll_prikazy();
										if (chyba!=ERR_OK){
											return chyba;
										}

										if (token->typ != RSUNTIL){
											return ERR_SYNTAX;
										}
										
										chyba = syntax_vyrazu();
										if (chyba!=ERR_OK){
											return chyba;
										}
										
										Vloz_instrukci(seznam_instrukci, IN_PGOTO, (UkTBSPolozka) op3, NULL, NULL ); // skoc, pokud podminka
										
										if (token->typ != STREDNIK){
											return ERR_SYNTAX;
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
									generuj_klic(0, &gen_klic);
									BVSVloz(&pom_tab_sym, gen_klic, NULL);
                  BVSNajdi(pom_tab_sym, gen_klic, &op3);
									generuj_klic(0, &gen_klic);
									BVSVloz(&pom_tab_sym, gen_klic, NULL);
                  BVSNajdi(pom_tab_sym, gen_klic, &op2);

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
											
									obnov(&obsah);					
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
										case TNNIL: break; 
										case IDKONEC: Vloz_instrukci(seznam_instrukci, IN_HLEDEJ, zas_zpracovani, token->data, obsah);
																	break;
										default: return ERR_SYNTAX;
									}
									
									generuj_klic(0, &gen_klic);
									BVSVloz(&pom_tab_sym, gen_klic, obsah);
                  BVSNajdi(pom_tab_sym, gen_klic, &op2);
									
									generuj_klic(0, &gen_klic);
									BVSVloz(&pom_tab_sym, gen_klic, NULL);
                  BVSNajdi(pom_tab_sym, gen_klic, &op3);
									Vloz_instrukci(seznam_instrukci, IN_TYPE, op2, NULL, op3);
									
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
									generuj_klic(0, &gen_klic);
									BVSVloz(&pom_tab_sym, gen_klic, NULL);
                  BVSNajdi(pom_tab_sym, gen_klic, &op1);
									if (token->typ != CARKA){
										return ERR_SYNTAX;
									}
									//druhy parametr
									chyba = syntax_vyrazu();
									if (chyba!=ERR_OK){
										return chyba;
									}
									generuj_klic(0, &gen_klic);
									BVSVloz(&pom_tab_sym, gen_klic, NULL);
                  BVSNajdi(pom_tab_sym, gen_klic, &op2);
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
									generuj_klic(0, &gen_klic);
									BVSVloz(&pom_tab_sym, gen_klic, NULL);
                  BVSNajdi(pom_tab_sym, gen_klic, &op3);
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
									generuj_klic(0, &gen_klic);
									BVSVloz(&pom_tab_sym, gen_klic, NULL);
                  BVSNajdi(pom_tab_sym, gen_klic, &op3);
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
										generuj_klic(0, &gen_klic);
										BVSVloz(&pom_tab_sym, gen_klic, NULL);
                  	BVSNajdi(pom_tab_sym, gen_klic, &op1);
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
										generuj_klic(0, &gen_klic);
										BVSVloz(&pom_tab_sym, gen_klic, NULL);
                  	BVSNajdi(pom_tab_sym, gen_klic, &op3);
                  	Vloz_instrukci(seznam_instrukci, IN_SUBSTR, op1, pom_pole, op3);
									
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
	}//switch v
	printf("vracim prikazy %d \n", chyba);
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
	UkTPlzkaSez adresa1;
	// UkTPlzkaSez adresa2; // neni potreba v tuto chvili
	
	
	if ((kam_priradit=malloc((token->delka+1) * sizeof(char)))==NULL){
		return ERR_INTERNI;
	}
	strcpy(kam_priradit,token->data); //identifikator nazvu fce nebo kam priradit
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
		case ZAVLEVA: 				// pouzita promenna uzel_aktualni_funkce, zjistit, jestli to nebude delat problemy
									if (BVSFunkceNajdi (strom_funkci, kam_priradit, &uzel_aktualni_funkce)){ // if nenalezeno		//kam_priradit == identifikator
										printf("volani nedeklarovane fce %s\n", kam_priradit);							
										free(kam_priradit);
										return ERR_SEMANT;
									}
									free(kam_priradit);
									
									Vloz_instrukci(seznam_instrukci, IN_PUSH, uzel_aktualni_funkce, NULL, NULL); // vlozim instrukci vlozeni adresy na zasobnik (zatim bez adresy)
									adresa1 = Sez_vrat_uk_posledni(seznam_instrukci);	
									
														
									chyba=ll_volani_prvni_parametr();
									if (chyba!=ERR_OK){
										free(kam_priradit);
										return chyba;
									}
									
									
									Vloz_instrukci(seznam_instrukci, IN_GOTO, uzel_aktualni_funkce->adresa, NULL, NULL); // vlozim instrukci skoku na adresu fce
									Vloz_instrukci(seznam_instrukci, IN_NVSTI, NULL, NULL, NULL);
									adresa1->instrukce.op2 = Sez_vrat_uk_posledni(seznam_instrukci);	
									// )
									if (token->typ != ZAVPRAVA){
										return ERR_SYNTAX;
									}
									
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									// ;
									if (token->typ != STREDNIK){
										return ERR_SYNTAX;
									}
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									break;
		// pravidlo 17	<PŘÍKAZ_S_ID> -> = PŘÍKAZ_S_ID_A_ROVNASE
		case ROVNASEKONEC:	printf("ll_prikaz_s_id: prislo = %d\n", token->typ);
												chyba = ll_prikaz_s_id_a_rovnase(kam_priradit);
												printf("posrane kam_priradit = %s\n", kam_priradit);
												
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

int ll_prikaz_s_id_a_rovnase(char *kam_priradit){
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
		//volani uzivatelske fce nebo prirazeni vyrazu
		case IDKONEC:	printf("zkousim fce\n");
									if (!BVSFunkceNajdi (strom_funkci, token->data, &uzel_aktualni_funkce)){ // if nalezeno
										printf("byla to fce\n");
										chyba = dej_token();
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
										chyba = dej_token();
										if (chyba != ERR_OK){
											return chyba;
										}
										printf("prisla prava ukoncujici zavorka\n");
									}else{ // neni to funkce, zkusime vyraz
										printf("nebyla to fce %s\n", token->data);
										uz_nacteno=TRUE;
										chyba = syntax_vyrazu();
										uz_nacteno = FALSE;
										if (chyba == ERR_PRAZDNY_VYRAZ){
											return ERR_SYNTAX;
										}else if (chyba != ERR_OK){
											return chyba;
										}
									}
								
									printf("cekam strednik %d\n", token->typ);
									if (token->typ != STREDNIK){
										return ERR_SYNTAX;
									}	
									
									/*if(!najdi_prom(zas_zpracovani, kam_priradit, op1)){
										return ERR_SEMANT; 
									}*/
									
									generuj_klic(0, &gen_klic);
									BVSVloz(&pom_tab_sym, gen_klic, NULL);
									BVSNajdi(pom_tab_sym, gen_klic, &op1);
									
									obnov(&obsah);
									
									obsah->typ = TDRETEZEC;
									obsah->data.dataRet = malloc((strlen(kam_priradit)+1) * sizeof(char));
									strcpy(obsah->data.dataRet, kam_priradit);
									
									generuj_klic(0, &gen_klic);
									BVSVloz(&pom_tab_sym, gen_klic, obsah);
									BVSNajdi(pom_tab_sym, gen_klic, &navratova_hodnota);
									
									printf("1------- obsah na novem miste: %s ---------------------\n", (char*)obsah->data.dataRet);
									//Vloz_instrukci(seznam_instrukci, IN_HLEDEJ, zas_zpracovani, navratova_hodnota, op1);
									//printf("2--------------- operand 3 ma hodnotu: %f \n", op3->data.dataCis);
									// v opreandu op3 je hodnota vypocitaneho vyrazu
									Vloz_instrukci(seznam_instrukci, IN_PRIRAD, op3, NULL, op1);
									
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
									
									generuj_klic(0, &gen_klic);
									BVSVloz(&pom_tab_sym, gen_klic, NULL);
                  BVSNajdi(pom_tab_sym, gen_klic, &op3);
                  if ( (op3 = (UkTBSPolozka) malloc( sizeof(TBSPolozka) )) == NULL){
										return ERR_INTERNI;
									}
									((UkTBSPolozka)op3)->typ = TDNIL;
									
									if(!BVSNajdi (pom_tab_sym, kam_priradit, &op1)){
										return ERR_SEMANT; 
									}
									Vloz_instrukci(seznam_instrukci, IN_PRIRAD, op3, NULL, op1);
									
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
									generuj_klic(0, &gen_klic);
									BVSVloz(&pom_tab_sym, gen_klic, NULL);
                  BVSNajdi(pom_tab_sym, gen_klic, &op3);
									generuj_klic(0, &gen_klic);
									BVSVloz(&pom_tab_sym, gen_klic, NULL);
                  BVSNajdi(pom_tab_sym, gen_klic, &op2);

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
									
									//prirazeni vysledku
									if(!BVSNajdi (pom_tab_sym, kam_priradit, &op1)){
										return ERR_SEMANT; 
									}
									Vloz_instrukci(seznam_instrukci, IN_PRIRAD, op3, NULL, op1);
										
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
											
									obnov(&obsah);					
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
										case TNNIL: break; 
										case IDKONEC: Vloz_instrukci(seznam_instrukci, IN_HLEDEJ, zas_zpracovani, token->data, obsah);
																	break;
										default: return ERR_SYNTAX;
									}
									
									generuj_klic(0, &gen_klic);
									BVSVloz(&pom_tab_sym, gen_klic, obsah);
                  BVSNajdi(pom_tab_sym, gen_klic, &op2);
									
									generuj_klic(0, &gen_klic);
									BVSVloz(&pom_tab_sym, gen_klic, NULL);
                  BVSNajdi(pom_tab_sym, gen_klic, &op3);
									Vloz_instrukci(seznam_instrukci, IN_TYPE, op2, NULL, op3);
									
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
									generuj_klic(0, &gen_klic);
									BVSVloz(&pom_tab_sym, gen_klic, NULL);
                  BVSNajdi(pom_tab_sym, gen_klic, &op1);
									if (token->typ != CARKA){
										return ERR_SYNTAX;
									}
									//druhy parametr
									chyba = syntax_vyrazu();
									if (chyba!=ERR_OK){
										return chyba;
									}
									generuj_klic(0, &gen_klic);
									BVSVloz(&pom_tab_sym, gen_klic, NULL);
                  BVSNajdi(pom_tab_sym, gen_klic, &op2);
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
									generuj_klic(0, &gen_klic);
									BVSVloz(&pom_tab_sym, gen_klic, NULL);
                  BVSNajdi(pom_tab_sym, gen_klic, &op3);
                  Vloz_instrukci(seznam_instrukci, IN_FIND, op1, op2, op3);
									
									//prirazeni vysledku
									if(!BVSNajdi (pom_tab_sym, kam_priradit, &op1)){
										return ERR_SEMANT; 
									}
									Vloz_instrukci(seznam_instrukci, IN_PRIRAD, op3, NULL, op1);
									
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
									printf("///////////////////vstup sort %d \n", token->typ);
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
									printf("dalsi krok sort %d \n", token->typ);
									generuj_klic(0, &gen_klic);
									BVSVloz(&pom_tab_sym, gen_klic, NULL);
                  BVSNajdi(pom_tab_sym, gen_klic, &op3);
                  Vloz_instrukci(seznam_instrukci, IN_SORT, token->data, NULL, op3);
									
									//prirazeni vysledku
									if(!BVSNajdi (pom_tab_sym, kam_priradit, &op1)){
										return ERR_SEMANT; 
									}
									Vloz_instrukci(seznam_instrukci, IN_PRIRAD, op3, NULL, op1);
									
									if (token->typ != ZAVPRAVA){
										return ERR_SYNTAX;
									}
									
									chyba = dej_token();		
									if (chyba!=ERR_OK){
										return chyba;
									}
									printf("a sme skoro u konce sort %d \n", token->typ);
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
										generuj_klic(0, &gen_klic);
										BVSVloz(&pom_tab_sym, gen_klic, NULL);
										BVSNajdi(pom_tab_sym, gen_klic, &navratova_hodnota);
										//char *pokusnej_kralik = NULL;	
										if (token->typ != RETEZEC){
											//Ret_alokuj(&(((UkTBSPolozka) op1)->data.dataRet), token->delka);
											Ret_alokuj(&(navratova_hodnota->data.dataRet), token->delka);
											//strcpy(((UkTBSPolozka) op1)->data.dataRet, token->data);
											strcpy(navratova_hodnota->data.dataRet, token->data);
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
										generuj_klic(0, &gen_klic);
										BVSVloz(&pom_tab_sym, gen_klic, NULL);
                  	BVSNajdi(pom_tab_sym, gen_klic, &op3);
                  	Vloz_instrukci(seznam_instrukci, IN_SUBSTR, op1, pom_pole, op3);
									
										//prirazeni vysledku
										if(!BVSNajdi (pom_tab_sym, kam_priradit, &op1)){
											return ERR_SEMANT; 
										}
										Vloz_instrukci(seznam_instrukci, IN_PRIRAD, op3, NULL, op1);
										
										
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
		default:uz_nacteno=TRUE; 
				syntax_vyrazu();
				uz_nacteno=FALSE;
				/*if(!najdi_prom(zas_zpracovani, kam_priradit, op1)){
					return ERR_SEMANT; 
				}*/	
				
				obnov(&obsah);
				obsah->typ = TDRETEZEC;
				obsah->data.dataRet = malloc((strlen(kam_priradit)+1) * sizeof(char));
				strcpy(obsah->data.dataRet, kam_priradit);
				generuj_klic(0, &gen_klic);
				BVSVloz(&pom_tab_sym, gen_klic, obsah);
				BVSNajdi(pom_tab_sym, gen_klic, &navratova_hodnota);
				printf("---- =konstanta: op3: (typ: %d)   kam?priradit>  %s\n",(int)op3->typ,
				(char*) navratova_hodnota->data.dataRet);

				printf("1------- obsah na novem miste: %s ---------------------\n", (char*)obsah->data.dataRet);
				//printf("2------- adresa ktera se vyhledala v zasobniku: %d ---------------------\n", (int)op1);
				Vloz_instrukci(seznam_instrukci, IN_HLEDEJ, zas_zpracovani, navratova_hodnota, uk_na_zasobnik);
				//printf("2--------------- operand 3 ma hodnotu: %f \n", op3->data.dataCis);
				// v opreandu op3 je hodnota vypocitaneho vyrazu
				Vloz_instrukci(seznam_instrukci, IN_PRIRAD, zas_zpracovani, navratova_hodnota, op3);
				////////////////////////////////////////////////////////////////////////////
				/*
				printf("---- =konstanta: op3: %f (typ: %d)\n",op3->data.dataCis, op3->typ);
				BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
				BVSNajdi(pom_tab_sym, generuj_klic(1), &navratova_hodnota);

				BVSVloz(&pom_tab_sym, generuj_klic(0), NULL);
				BVSNajdi(pom_tab_sym, generuj_klic(1), &op1);
				
				navratova_hodnota->typ = TDRETEZEC;
				navratova_hodnota->data.dataRet = malloc((strlen(kam_priradit)+1) * sizeof(char));
				
				strcpy(navratova_hodnota->data.dataRet, kam_priradit);
				printf("3--------------- operand 3 ma hodnotu: %f \n", op3->data.dataCis);
				//Vloz_instrukci(seznam_instrukci, IN_HLEDEJ, zas_zpracovani, kam_priradit, op1);
				//Vloz_instrukci(seznam_instrukci, IN_PRIRAD, op3, NULL, op1);
				
				Vloz_instrukci(seznam_instrukci, IN_HLEDEJ, zas_zpracovani, navratova_hodnota, op1);
				// v opreandu op3 je hodnota vypocitaneho vyrazu
				Vloz_instrukci(seznam_instrukci, IN_PRIRAD, op3, NULL, op1);
				*/
	printf("ekam strednik %d\n", token->typ);
				if (token->typ != STREDNIK){
					return ERR_SYNTAX;
				}
				chyba = dej_token();		
				if (chyba!=ERR_OK){
					return chyba;
				}
				//další příkazy
	printf("za strednikem %d\n", token->typ);
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
	op3 = NULL;
	chyba = syntax_vyrazu();
	
	if (chyba == ERR_PRAZDNY_VYRAZ){
		printf("prvni parametr == nic\n");
		return ERR_OK;
	}else if (chyba != ERR_OK){
		return chyba;
	}
	
	// param je v op3
	
	if(token->typ == CARKA){
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
	printf("ll_dalsi parametr: konec\n");
	return ERR_OK;
}

int syntakticky_analyzator(){
	printf("syntakticky analyzator: vstup\n");
// rekurzivni metoda
	Vloz_instrukci(seznam_instrukci, IN_KONEC, NULL, NULL, NULL);
	adresa_konce = Sez_vrat_uk_posledni(seznam_instrukci);
	
//	printf("/-/-/-/-/-/-/- adresa konce by mela byt: %d \n", adresa_konce);
	chyba = ll_funkce();
	
	set_first(zas_zpracovani);

	printf("*************Vypis zasobniku**********\n");
	while(zas_zpracovani->aktivni != NULL){
		printf("-> ->Klic: %s, dataCis: %f, typ: %d\n", zas_zpracovani->aktivni->parametr.klic, 
		zas_zpracovani->aktivni->parametr.data->data.dataCis, 
		zas_zpracovani->aktivni->parametr.data->typ);
		set_nasl(zas_zpracovani);
	}
	printf("**************************************\n");
	//BVSVypisStrom (&pom_tab_sym);
	return chyba;
}














/*******************
 * Martina *********
 ******************/
 
 
 
 

int syntax_vyrazu() {

    //TPrvek pom1, pom2, pom3, pom4;
    int znamenko;   // co se bude provadet, podle precedencni tabulky
    int uk_na_terminal;  // ukazatel na "nejvyssi" terminal
    bool novy_token_ano_ne = true;
    bool redukce = false;
    int err_chyba = ERR_OK;
    int prom1, prom2, prom3, prom4;
    UkTBSPolozka ukprom;
    //TPrvek *pom_prvek;
    //TPrvek prvek_pomocny;
    
    
    //pom_prvek = malloc(sizeof(struct stTPrvek));
    /* 
    prvek_pomocny = malloc(sizeof(struct stTPrvek));
    pom1 = malloc(sizeof(struct stTPrvek));
    pom2 = malloc(sizeof(struct stTPrvek));
    pom3 = malloc(sizeof(struct stTPrvek));
    pom4 = malloc(sizeof(struct stTPrvek));
    */
		int kolik_neterminalu = 0;

    zasobnik_vynuluj(&zas_uk, &zas_int);
    
    //pom_prvek->typ = DOLAR;
    //pom_prvek->uk_na_prvek_ts = NULL;

    //if(zasobnik_push(&zasobnik, *pom_prvek) == ERR_INTERNI) {
    //    return ERR_INTERNI;
    //}
    
    
    if(zasobnik_push(&zas_uk, DOLAR, &zas_int, NULL) == ERR_INTERNI) {
        return ERR_INTERNI;
    }

    uk_na_terminal = zas_int.top;   // nejvyssi terminal je ted $

    if(uz_nacteno == FALSE) {
			dej_token(soubor, token); // otestovat navratovy kod
		}


    while((redukce == true) ||
        (token->typ == IDKONEC) || (token->typ == INTKONEC) || (token->typ == DESKONEC) ||
        (token->typ == EXPKONEC) || (token->typ == RETEZEC) || (token->typ == ZAVLEVA) ||
        (token->typ == ZAVPRAVA) || (token->typ == PLUS) || (token->typ == MINUSKONEC) ||
        (token->typ == KRAT) || (token->typ == DELENO) || (token->typ == MOCNINA) ||
        (token->typ == KONKATENACE) || (token->typ == POROVNANI) || (token->typ == NEROVNASE) ||
        (token->typ == VETSIROVNO) || (token->typ == VETSITKOKONEC) || (token->typ == MENSIROVNO) ||
        (token->typ == MENSITKOKONEC) || (token->typ == TNNIL) || (token->typ == TNFALSE) ||
        (token->typ == TNTRUE) || (znamenko == VETSI)) {



        if(redukce == true) {
            znamenko = VETSI;
        }

        else {
            prom1 = preved_z_tokenu(token->typ);
            znamenko = preced_tabulka[preved_z_tokenu(zas_int.array[uk_na_terminal])][prom1];
        }


        if(znamenko == PTCHYBA) {
        
			if(zasobnik_pristup_int(&zas_int, &prom2, 0) == ERR_INTERNI) {
				return ERR_INTERNI;
			}
				
			if((prom2 == DOLAR) && (zas_int.top == 0)) {
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

				    //pom_prvek->typ = token->typ;
					//pom_prvek->uk_na_prvek_ts = NULL;
				    
				    if(token->typ == IDKONEC) {
				    	obsah->typ = TDRETEZEC;
				    	Ret_alokuj(&obsah->data.dataRet, token->delka + 1);
				    	strcpy (obsah->data.dataRet, token->data);
							generuj_klic(0, &gen_klic);
      				BVSVloz(&pom_tab_sym, gen_klic, obsah);
      				BVSNajdi(pom_tab_sym, gen_klic, &navratova_hodnota);
      				
							generuj_klic(0, &gen_klic);
      				BVSVloz(&pom_tab_sym, gen_klic, NULL);
      				BVSNajdi(pom_tab_sym, gen_klic, &op3);
      				
				    	Vloz_instrukci(seznam_instrukci, IN_HLEDEJ, zas_zpracovani, navratova_hodnota, &op3);
				    	//printf("U ID: op3->typ: %d\n", op3->typ);
				    	navratova_hodnota = op3;
				    	//pom_prvek->uk_na_prvek_ts = navratova_hodnota;
				    }
				    
				    // Vlozeni do pomocne tabulky symbolu
				    // Ulozeni typu a odkazu do tabulky symbolu na zasobnik
				    else if((token->typ == INTKONEC) || (token->typ == DESKONEC) || (token->typ == EXPKONEC)) {
						obnov(&obsah);
        				obsah->data.dataCis = atof(token->data);
        				obsah->typ = TDCISLO;
								generuj_klic(0, &gen_klic);
        				BVSVloz(&pom_tab_sym, gen_klic, obsah);
        				BVSNajdi(pom_tab_sym, gen_klic, &navratova_hodnota);
        				//pom_prvek->uk_na_prvek_ts = navratova_hodnota;		
        		}
        		
        		else if(token->typ == RETEZEC) {
						obnov(&obsah);
        				Ret_alokuj(&(obsah->data.dataRet), token->delka);
								strcpy(obsah->data.dataRet,token->data);
        				obsah->typ = TDRETEZEC;
								generuj_klic(0, &gen_klic);
        				BVSVloz(&pom_tab_sym, gen_klic, obsah);
        				BVSNajdi(pom_tab_sym, gen_klic, &navratova_hodnota);
        				//pom_prvek->uk_na_prvek_ts = navratova_hodnota;	
        		}
        		
        		else if(token->typ == TNTRUE) {
        				obnov(&obsah);
        				obsah->typ = TDBOOL;
						obsah->data.dataBool = TRUE;
								generuj_klic(0, &gen_klic);
        				BVSVloz(&pom_tab_sym, gen_klic, obsah);
        				BVSNajdi(pom_tab_sym, gen_klic, &navratova_hodnota);
        				//pom_prvek->uk_na_prvek_ts = navratova_hodnota;	
        		}
        		
        		else if(token->typ == TNFALSE) {
        				obnov(&obsah);
        				obsah->typ = TDBOOL;
						obsah->data.dataBool = FALSE;
								generuj_klic(0, &gen_klic);
        				BVSVloz(&pom_tab_sym, gen_klic, obsah);
        				BVSNajdi(pom_tab_sym, gen_klic, &navratova_hodnota);
        				//pom_prvek->uk_na_prvek_ts = navratova_hodnota;	
        		}
        		
        		//pro operatory ... TO MI TAM JAKO VAZNE CHYBELO, NEBO UZ MI HRABE ?!?!?!?!??!?
        		else {
					navratova_hodnota = NULL;
				}
        		
        		//if(zasobnik_push(&zasobnik, *pom_prvek) == ERR_INTERNI) {
					//return ERR_INTERNI;
				//}
				
				if(zasobnik_push(&zas_uk, token->typ, &zas_int, navratova_hodnota) == ERR_INTERNI) {
					return ERR_INTERNI;
				}
				
				if(navratova_hodnota != NULL)
					printf("PUSHNUuuuuuuuuuuu typ: %d, je tam %s\n", navratova_hodnota->typ, token->data);
				else
					printf("PUSHNUuuuuuuuuuuu typ: nothing, je tam %s\n", token->data);



            uk_na_terminal = zas_int.top;   // pozice terminalu - je to ten na vrcholu zasobniku
            novy_token_ano_ne = true;
        }


        else if(znamenko == MENSI) {

            // za posledni terminal vloz "<"
            //pom_prvek->typ = MENSI;
            //pom_prvek->uk_na_prvek_ts = NULL;   // pridano
            
            if(zasobnik_push(&zas_uk, MENSI, &zas_int, NULL) == ERR_INTERNI) {
				return ERR_INTERNI;
			}
            
            //if(zasobnik_push(&zasobnik, *pom_prvek) == ERR_INTERNI) {
              //  return ERR_INTERNI;
            //}


            for(int i = zas_int.top - 1 ;i >= 0 ;i--) {

                if(i == uk_na_terminal) {
                    break;
                }

                else {  // prohozeni "<" s predeslym (neterminalnim) znakem
                        // znak "<" musi probublat az za nejvyssi terminal
                    prom4 = zas_int.array[i];
                    ukprom = zas_uk.array[i];
                    zas_int.array[i] = zas_int.array[i+1];
                    zas_uk.array[i] = zas_uk.array[i+1];
                    zas_int.array[i+1] = prom4;
                    zas_uk.array[i+1] = ukprom;
                }

            }

						//pom_prvek->typ = token->typ;
						//pom_prvek->uk_na_prvek_ts = NULL;
				    
				    if(token->typ == IDKONEC) {
				    	obsah->typ = TDRETEZEC;
				    	Ret_alokuj(&obsah->data.dataRet, token->delka + 1);
				    	strcpy (obsah->data.dataRet, token->data);
							generuj_klic(0, &gen_klic);
      				BVSVloz(&pom_tab_sym, gen_klic, obsah);
      				BVSNajdi(pom_tab_sym, gen_klic, &navratova_hodnota);
      				
							generuj_klic(0, &gen_klic);
      				BVSVloz(&pom_tab_sym, gen_klic, NULL);
      				BVSNajdi(pom_tab_sym, gen_klic, &op3);
      				
				    	Vloz_instrukci(seznam_instrukci, IN_HLEDEJ, zas_zpracovani, navratova_hodnota, &op3);
				    	//printf("U ID: op3->typ: %d\n", op3->typ);
				    	navratova_hodnota = op3;
				    	//printf("U ID: navratova_hodnota->typ: %d\n", navratova_hodnota->typ);
				    	//pom_prvek->uk_na_prvek_ts = navratova_hodnota;
				    	printf("------------------------------------------------navratova hodnota pred %d\n", (int) navratova_hodnota);
				    }
				    
				    // Vlozeni do pomocne tabulky symbolu
				    // Ulozeni typu a odkazu do tabulky symbolu na zasobnik
				    else if((token->typ == INTKONEC) || (token->typ == DESKONEC) || (token->typ == EXPKONEC)) {
						obnov(&obsah);
        				obsah->data.dataCis = atof(token->data);
        				obsah->typ = TDCISLO;
								generuj_klic(0, &gen_klic);
        				BVSVloz(&pom_tab_sym, gen_klic, obsah);
        				BVSNajdi(pom_tab_sym, gen_klic, &navratova_hodnota);
        				//pom_prvek->uk_na_prvek_ts = navratova_hodnota;	
        				//printf(">>>>>>>>> obsah->data.dataCis: %f\n", obsah->data.dataCis);
        				//printf(">>>>>>>>> obsah->typ: %d\n", obsah->typ);
        				//printf(">>>>>>>>> navratova_hodnota->data.dataCis: %f\n", navratova_hodnota->data.dataCis);
        				//printf(">>>>>>>>> navratova_hodnota->typ: %d\n", navratova_hodnota->typ);
        				
        		}
        		
        		else if(token->typ == RETEZEC) {
						obnov(&obsah);
        				Ret_alokuj(&(obsah->data.dataRet), token->delka);
						strcpy(obsah->data.dataRet,token->data);
        				obsah->typ = TDRETEZEC;
								generuj_klic(0, &gen_klic);
        				BVSVloz(&pom_tab_sym, gen_klic, obsah);
        				BVSNajdi(pom_tab_sym, gen_klic, &navratova_hodnota);
        				//pom_prvek->uk_na_prvek_ts = navratova_hodnota;
        				//printf(">>>>>>>>> obsah->data.dataRet: %s\n", obsah->data.dataRet);
        				//printf(">>>>>>>>> obsah->typ: %d\n", obsah->typ);
        				//printf(">>>>>>>>> navratova_hodnota->data.dataRet: %s\n", navratova_hodnota->data.dataRet);
        				//printf(">>>>>>>>> navratova_hodnota->typ: %d\n", navratova_hodnota->typ);

        		}
        		
        		else if(token->typ == TNTRUE) {
						obnov(&obsah);
        				obsah->typ = TDBOOL;
						obsah->data.dataBool = TRUE;
								generuj_klic(0, &gen_klic);
        				BVSVloz(&pom_tab_sym, gen_klic, obsah);
        				BVSNajdi(pom_tab_sym, gen_klic, &navratova_hodnota);
        				//pom_prvek->uk_na_prvek_ts = navratova_hodnota;	
        		}
        		
        		else if(token->typ == TNFALSE) {
						obnov(&obsah);
        				obsah->typ = TDBOOL;
						obsah->data.dataBool = FALSE;
								generuj_klic(0, &gen_klic);
        				BVSVloz(&pom_tab_sym, gen_klic, obsah);
        				BVSNajdi(pom_tab_sym, gen_klic, &navratova_hodnota);
        				//pom_prvek->uk_na_prvek_ts = navratova_hodnota;	
        		}
        		
        		//pro operatory ... TO MI TAM JAKO VAZNE CHYBELO, NEBO UZ MI HRABE ?!?!?!?!??!?
        		else {
					navratova_hodnota = NULL;
				}
        		
        		/*printf(">>>>>>>>> pom_prvek->uk_na_ts.typ: %d\n", navratova_hodnota->typ);
        		if(navratova_hodnota->typ == TDRETEZEC) {
					printf(">>>>>>>>> pom_prvek->uk_na_prvek_ts->data->dataRet: %s\n", pom_prvek->uk_na_prvek_ts->data.dataRet);
				}if(navratova_hodnota->typ == TDCISLO){		
					printf(">>>>>>>>> pom_prvek->uk_na_prvek_ts->data->dataCis: %f\n", pom_prvek->uk_na_prvek_ts->data.dataCis);
				}*/
        		
        		//if(zasobnik_push(&zasobnik, *pom_prvek) == ERR_INTERNI) {
					//return ERR_INTERNI;
				//}
				
				if(zasobnik_push(&zas_uk, token->typ, &zas_int, navratova_hodnota) == ERR_INTERNI) {
					return ERR_INTERNI;
				}
				
				if(navratova_hodnota != NULL)
					printf("PUSHNUuuuuuuuuuuu typ: %d, je tam %s\n", navratova_hodnota->typ, token->data);
				else
					printf("PUSHNUuuuuuuuuuuu typ: nothing, je tam %d\n", token->typ);
            
            

            uk_na_terminal = zas_int.top;   // pozice terminalu - je to ten na vrcholu zasobniku
            novy_token_ano_ne = true;

        }


        else if(znamenko == VETSI) {    // hledame pravidlo


            if(zasobnik_pristup_int(&zas_int, &prom1, 0) == ERR_INTERNI) {
                return ERR_INTERNI;
            }

            prom1 = preved_z_tokenu(prom1);

            // pokud se pri redukci prvek na zasobniku nerovna nicemu z case pod, tak to musim breaknout,
            // ptz se neda redukovat podle zadnyho pravidla !!!
            if((prom1 != OPIDENTIFIKATOR) && (prom1 != NETERMINAL) && (prom1 != OPPRAVAZAVORKA)) {
                printf("Neodpovida zadnemu pravidlu --> BREAK\n");
                return ERR_SYNTAX;
            }


            switch(prom1) {

                case OPIDENTIFIKATOR:    // redukce podle pravidla E -> i
                //case OPINTEGER:
                //case OPDESETINNE:
                //case OPEXPCISLO:
                //case OPRETEZEC:
                //case OPTRUE
                //case OPFALSE
                //case OPNIL


                    if(zasobnik_pristup_int(&zas_int, &prom1, 1) == ERR_INTERNI) {
                        return ERR_INTERNI;
                    }


                    if(prom1 == MENSI) {
                    
						if(zasobnik_pristup_uk(&zas_uk, &ukprom, 0)) {
							return ERR_INTERNI;
						} 
                        
                        //pom_prvek->uk_na_prvek_ts = ukprom;
					
                        if(zasobnik_pop(&zas_uk, &zas_int) == ERR_INTERNI) {
                            return ERR_INTERNI;
                        }

                        if(zasobnik_pop(&zas_uk, &zas_int) == ERR_INTERNI) {
                            return ERR_INTERNI;
                        }
													
						//pom_prvek->typ = NETERMINAL;
						//pom_prvek->uk_na_prvek_ts = NULL;
													
                        //if(zasobnik_push(&zasobnik, *pom_prvek) == ERR_INTERNI) {
                          //  return ERR_INTERNI;
                        //}
                        
                        if(zasobnik_push(&zas_uk, NETERMINAL, &zas_int, ukprom) == ERR_INTERNI) {
							return ERR_INTERNI;
						}

						printf("==========\nKolik_neterminalu = %d\n==========\n", kolik_neterminalu);

						if(kolik_neterminalu == 0) {
							/*BVSVloz(&pom_tab_sym, generuj_klic(0), prvek_pomocny->uk_na_prvek_ts);
							BVSNajdi(pom_tab_sym, generuj_klic(1), &op3);*/
							op3 = ukprom;
						}

						kolik_neterminalu++;

                    }

                    else {
                        return ERR_SYNTAX;
                    }


                    break;


                case OPPRAVAZAVORKA:        // redukce podle pravidla E -> (E)

                    if(zasobnik_pristup_int(&zas_int, &prom1, 1) == ERR_INTERNI) {
                        return ERR_INTERNI;
                    }

                    if(zasobnik_pristup_int(&zas_int, &prom2, 2) == ERR_INTERNI) {
                        return ERR_INTERNI;
                    }

                    if(zasobnik_pristup_int(&zas_int, &prom3, 3) == ERR_INTERNI) {
                        return ERR_INTERNI;
                    }

                    prom2 = preved_z_tokenu(prom2);

                    if((prom1 == NETERMINAL) && (prom2 == OPLEVAZAVORKA) && (prom3 == MENSI)) {
                    
                    		if(zasobnik_pristup_uk(&zas_uk, &ukprom, 1)) {
								return ERR_INTERNI;
							} 
                        
                        //pom_prvek->uk_na_prvek_ts = ukprom;

                        if(zasobnik_pop(&zas_uk, &zas_int) == ERR_INTERNI) {
                            return ERR_INTERNI;
                        }

                        if(zasobnik_pop(&zas_uk, &zas_int) == ERR_INTERNI) {
                            return ERR_INTERNI;
                        }

                        if(zasobnik_pop(&zas_uk, &zas_int) == ERR_INTERNI) {
                            return ERR_INTERNI;
                        }

                        if(zasobnik_pop(&zas_uk, &zas_int) == ERR_INTERNI) {
                            //err_chyba = ERR_INTERNI;
                            return ERR_INTERNI;
                        }
                        
                        
						//pom_prvek->typ = NETERMINAL;
						//pom_prvek->uk_na_prvek_ts = NULL;
													
                        //if(zasobnik_push(&zasobnik, *pom_prvek) == ERR_INTERNI) {
                           // return ERR_INTERNI;
                        //}
                        
                        if(zasobnik_push(&zas_uk, NETERMINAL, &zas_int, ukprom) == ERR_INTERNI) {
							return ERR_INTERNI;
						}

						if(kolik_neterminalu == 0) {
							//BVSVloz(&pom_tab_sym, generuj_klic(0), ukprom);
							//BVSNajdi(pom_tab_sym, generuj_klic(1), &op3);
							op3 = ukprom;
						}

						kolik_neterminalu++;

                    }

                    else {
                        return ERR_SYNTAX;
                    }

                    break;


                case NETERMINAL:            // redukce podle pravidla E -> E + E atd.

                    zasobnik_pristup_int(&zas_int, &prom1, 1);
                    prom1 = preved_z_tokenu(prom1);

                    if((prom1 != OPPLUS) && (prom1 != OPMINUS) && (prom1 != OPKRAT) &&
                        (prom1 != OPDELENO) && (prom1 != OPMOCNINA) && (prom1 != OPJEROVNO) &&
                        (prom1 != OPNENIROVNO) && (prom1 != OPMENSITKO) && (prom1 != OPVETSITKO) &&
                        (prom1 != OPMENSITKOROVNO) && (prom1 != OPVETSITKOROVNO) && (prom1 != OPKONKATENACE)) {

                        printf("Neodpovida zadnemu pravidlu --> BREAK\n");
                        return ERR_SYNTAX;
                    }

                    switch(prom1) {

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
                        
                        
                        		switch(prom1) {
                        		
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
                        		

                            if(zasobnik_pristup_int(&zas_int, &prom1, 2) == ERR_INTERNI) {
                                return ERR_INTERNI;
                            }

                            if(zasobnik_pristup_int(&zas_int, &prom2, 3) == ERR_INTERNI) {
                                return ERR_INTERNI;
                            }

                            if((prom1 == NETERMINAL) && (prom2 == MENSI)) {
                            
						
								ukprom = NULL;

								if(zasobnik_pristup_uk(&zas_uk, &ukprom, 0)) {
									return ERR_INTERNI;
								} 

								op2 = ukprom;
								
								ukprom = NULL;
								
								if(zasobnik_pristup_uk(&zas_uk, &ukprom, 2)) {
									return ERR_INTERNI;
								} 
								
								op1 = ukprom;
								generuj_klic(0, &gen_klic);					
								BVSVloz(&pom_tab_sym, gen_klic, NULL);
								BVSNajdi(pom_tab_sym, gen_klic, &op3);
								
								Vloz_instrukci(seznam_instrukci, typ_instrukce, op1, op2, op3);
								BVSVypisStrom(&pom_tab_sym);
								//printf("Cislo adresy znovu: %d\n", (int) op3);
                        				
                        

                                if(zasobnik_pop(&zas_uk, &zas_int) == ERR_INTERNI) {
																	return ERR_INTERNI;
                                }

                                if(zasobnik_pop(&zas_uk, &zas_int) == ERR_INTERNI) {
                                    return ERR_INTERNI;
                                }

                                if(zasobnik_pop(&zas_uk, &zas_int) == ERR_INTERNI) {
                                    return ERR_INTERNI;
                                }

                                if(zasobnik_pop(&zas_uk, &zas_int) == ERR_INTERNI) {
                                    return ERR_INTERNI;
                                }

								//pom_prvek->typ = NETERMINAL;
								//pom_prvek->uk_na_prvek_ts = op3;        
													
				                //if(zasobnik_push(&zasobnik, *pom_prvek) == ERR_INTERNI) {
                                  //  return ERR_INTERNI;
                                //}
                                
                                if(zasobnik_push(&zas_uk, NETERMINAL, &zas_int, op3) == ERR_INTERNI) {
									return ERR_INTERNI;
								}
								
								//printf("PUSHNU op3 typ: %d\n", op3->typ);

                            }

                            else {
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
            for(int j = 0; j <= zas_int.top; j++) {
                zasobnik_pristup_int(&zas_int, &prom1, j);
                prom1 = preved_z_tokenu(prom1);

                if((prom1 != NETERMINAL) && (prom1 != MENSI)) {
                    uk_na_terminal = zas_int.top - j;
                    break;
                }
            }


            novy_token_ano_ne = false;

        }   // konec else if(znamenko == VETSI)


        if(novy_token_ano_ne == true) {
            chyba = dej_token();   // otestovat navratovy kod
            if(chyba != ERR_OK){
				return ERR_LEX;
			}
        }



        // pokud je vse u konce,  breaknem to
        if(((token->typ != IDKONEC) && (token->typ != INTKONEC) && (token->typ != DESKONEC) &&
            (token->typ != EXPKONEC) && (token->typ != RETEZEC) && (token->typ != ZAVLEVA) &&
            (token->typ != ZAVPRAVA) && (token->typ != PLUS) && (token->typ != MINUSKONEC) &&
            (token->typ != KRAT) && (token->typ != DELENO) && (token->typ != MOCNINA) &&
            (token->typ != KONKATENACE) && (token->typ != POROVNANI) && (token->typ != NEROVNASE) &&
            (token->typ != VETSIROVNO) && (token->typ != VETSITKOKONEC) && (token->typ != MENSIROVNO) &&
            (token->typ != MENSITKOKONEC) && (token->typ != TNNIL) && (token->typ != TNFALSE) &&
            (token->typ != TNTRUE)) &&
            (zas_int.top == 1) && (zas_int.array[1] == NETERMINAL) && (zas_int.array[0] == DOLAR)) {

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
            (token->typ != TNTRUE)) && (zas_int.top > 1)) {

            redukce = true;

        }
        
        int m=0;
        while (m<=zas_int.top) {
			zasobnik_pristup_int(&zas_int, &prom1, m);
			printf("Moje oznaceni: %d\n", prom1);
			zasobnik_pristup_uk(&zas_uk, &ukprom, m);
			if(ukprom != NULL)
				printf("Moje typ: %d\n", ukprom->typ);
			m++;
		}



    }   // konec while



    if(zasobnik_pristup_int(&zas_int, &prom2, 0) == ERR_INTERNI) {
        //err_chyba = ERR_INTERNI;
        return ERR_INTERNI;
    }


    // pokud byl vyraz prazdny, tzn ze prvni precteny token nepatril do vyrazu, tak je to OK
    if((prom2 == DOLAR) && (zas_int.top == 0)) {
        printf("Prazdny vyraz\n");
        return ERR_PRAZDNY_VYRAZ;
    }
    
    if(zasobnik_pristup_int(&zas_int, &prom2, 1) == ERR_INTERNI) {
        return ERR_INTERNI;
    }

    if(zasobnik_pristup_int(&zas_int, &prom1, 0) == ERR_INTERNI) {
        return ERR_INTERNI;
    }
    

		// pokud uz na vstupni pasce nic neni, ale na zasobniku je neco jineho nez $E,
    // tak je to synt chyba
    if(!((prom1 == NETERMINAL) && (prom2 == DOLAR) && (zas_int.top == 1))) {
        //err_chyba = ERR_SYNTAX;
        printf("CHYBA\n");
        return ERR_SYNTAX;
    }


   return err_chyba;

}  // konec funkce syntax_vyrazu()
