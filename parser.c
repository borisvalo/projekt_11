/*
//tokeny - konecne stavy
typedef enum {
CHYBA, //vrati v pripade chybove situace

//standartni tokeny
INTKONEC, // integer cislo
DESKONEC, // desetinne cislo
EXPKONEC, // cislo s 'e' nebo 'E'
RETEZEC, // retezec
CARKA, // ,
ZAVLEVA, // (
ZAVPRAVA, // )
STREDNIK, // ;
PLUS, // +
KRAT, // *
DELENO, // :
KONKATENACE, // ..
ROVNASEKONEC, // =
POROVNANI, // ==
MOCNINA, // ^
VETSIROVNO, // >=
VETSITKOKONEC, // >
MENSIROVNO, // <=
MENSITKOKONEC, // <
NEROVNASE, // ~=
IDKONEC, // identifikator
MINUSKONEC, // -
ENDOFFILE, // EOF

//klicova slova
TNDO, TNELSE, TNEND, TNFALSE, TNFUNCTION, TNIF, TNLOCAL,
TNNIL, TNREAD, TNRETURN, TNTHEN, TNTRUE, TNWHILE, TNWRITE,

REZSL, //rezervovana slova
RSAND, RSBREAK, RSELSEIF, RSFOR, RSIN, RSNOT, RSOR,
RSREPEAT, RSUNTIL,
} TOKENY;
*/
typedef enum{
LL_FUNKCE, //očekávám funkci, nebo EOF
LL_PARAMETR, //první parametr fce
LL_DALSI_PARAMETR, //další
LL_DEKLARACE, //deklarace proměnných
LL_INICIALIZACE, // nepovinná inicializace
//LL_LITERAL, // stav čekání na literál - číslo nebo řetězec
LL_PRIKAZY, //blok příkazů ve funkci
LL_PRIKAZ_S_ID, // výsledek faktorizace - příkaz typu id NĚCO
LL_PRIKAZ_S_ID_A_ROVNASE, //  = NĚCO
LL_TERMINAL, // terminál
LL_VYRAZ
}LL_stavy;

void kontrola_identifikatoru(*token){
	assert(token->typ == IDKONEC);
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
	}else	if (strcmp(token->data, "break;") == 0) {
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
	}
}

int ll_funkce (){
	token = dej_mi_token();
	if(token.typ == ENDOFFILE){
// pravidlo 2	<FUNKCE> -> Eps
		return ERR_OK;
	}else if (token.typ != TNFUNCTION){
// pravidlo 1	<FUNKCE> -> function id ( PARAMETR ) DEKLARACE PŘÍKAZY end ; FUNKCE 
		return ERR_SYNTAX;
	}
	// id
	token = dej_mi_token();
	if (token.typ != IDKONEC) {
		return ERR_SYNTAX;
	}
	// (
	token = dej_mi_token();
	if (token.typ != ZAVLEVA) {
		return ERR_SYNTAX;
	}
	// PARAMETR
	token = dej_mi_token();
	chyba=ll_parametr();
	// )
	if (token.typ != ZAVPRAVA)
		return ERR_SYNTAX;
	}
	// DEKLARACE
	token = dej_mi_token();
	chyba=ll_deklarace();
	// PRIKAZY
	chyba=ll_prikazy();
	
	if (token.typ != TNEND){
		return ERR_SYNTAX;
	}
	token = dej_mi_token();
	if (token.typ != STREDNIK){
		return ERR_SYNTAX;
	}
	chyba=ll_funkce();
}

int ll_parametr(){
	// pravidlo 3	<PARAMETR> -> ID DALŠÍ_PARAMETR
	//nepovinne, tzn negeneruji errory
	if (token.typ == IDKONEC){
		chyba = ll_dalsi_parametr();
	}
	return chyba;
}

int ll_dalsi_parametr(){
	// pravidlo 5	<DALŠÍ_PARAMETR> -> , ID DALŠÍ_PARAMETR
	// nepovinne, tzn negeneruji errory
	token = dej_mi_token();
	if (token.typ == CARKA){
		token=dej_mi_dalsi_token();
		if (token.typ == IDKONEC){
			chyba = ll_dalsi_parametr();
		}else{
			return ERR_SYNTAX;
		}
	}
	return chyba;
}

int ll_deklarace(){
	// pravidlo 7	<DEKLARACE> -> local id INICIALIZACE ; DEKLARACE
	if (token.typ == TNLOCAL){
	
		token = dej_mi_token();
		if (token.typ == IDKONEC){
			chyba = ll_inicializace();
		}else {
			return ERR_SYNTAX;
		}
		
		if (token.typ != STREDNIK) {
			return ERR_SYNTAX;
		}
		token = dej_mi_token();
		chyba=ll_deklarace();
	}
	return chyba;
}

int ll_inicializace(){		
// pravidlo 9	<INICIALIZACE> -> =VÝRAZ
// pravidlo 10	<INICIALIZACE> -> Eps
	token = dej_mi_token();
	if (token.typ == ROVNASEKONEC){
		chyba = syntaxe_vyrazu();
	}
	return chyba;
}

int ll_prikazy(){
	switch(token.typ){
		case TNIF:	
		// pravidlo 11	<PŘÍKAZY> -> if VÝRAZ then PŘÍKAZY else PŘÍKAZY end ; PŘÍKAZY
								chyba = syntaxe_vyrazu();
								
								if (token.typ != TNTHEN){
									return ERR_SYNTAX;
								}
								
								token = dej_mi_token();
								chyba = ll_prikazy();
								
								if (token.typ != TNELSE){
									return ERR_SYNTAX;
								}
								token = dej_mi_token();
								chyba = ll_prikazy();
								
								if (token.typ != TNEND){
									return ERR_SYNTAX;
								}
								
								token = dej_mi_token();
								if (token.typ != STREDNIK){
									return ERR_SYNTAX;
								}
								
								token = dej_mi_token();
								chyba = ll_prikazy();
								break;
								
		// pravidlo 12	<PŘÍKAZY> -> id PŘÍKAZ_S_ID ; PŘÍKAZY
		case IDKONEC:	chyba = ll_prikaz_s_id();
									if (token.typ != STREDNIK){
										return ERR_SYNTAX;
									}
									chyba = ll_prikazy();
									break;
		// pravidlo 13	<PŘÍKAZY> -> return VÝRAZ ; PŘÍKAZY
		case TNRETURN:	chyba = syntaxe_vyrazu();
										
										if (token.typ != STREDNIK){
											return ERR_SYNTAX;
										}
										token = dej_mi_token();
										chyba = ll_prikazy();
										break;
		// pravidlo 14	<PŘÍKAZY> -> while VÝRAZ do PŘÍKAZY end ; PŘÍKAZY
		case TNWHILE:	chyba = syntaxe_vyrazu();
		
									if (token.typ != TNDO){
										return ERR_SYNTAX;
									}
									
									token = dej_mi_token();
									chyba = ll_prikazy();
									
									if (token.typ != TNEND){
										return ERR_SYNTAX;
									}
									token = dej_mi_token();
									if (token.typ != STREDNIK){
										return ERR_SYNTAX;
									}
									token = dej_mi_token();
									chyba = ll_prikazy();
									break;
	}//switch
	return ERR_OK;
}//fce


int ll_prikaz_s_id(){
	// pravidlo 16	<PŘÍKAZ_S_ID> -> ( VOLANI_PARAMETR )
	// pravidlo 17	<PŘÍKAZ_S_ID> -> = PŘÍKAZ_S_ID_A_ROVNASE
	token = dej_mi_token();
	switch (token.typ){
		// pravidlo 16	<PŘÍKAZ_S_ID> -> ( PARAMETR )
		case ZAVLEVA: ll_volani_parametr();
		
									if (token.typ != ZAVPRAVA){
										return ERR_SYNTAX;
									}
									break;
		// pravidlo 17	<PŘÍKAZ_S_ID> -> = PŘÍKAZ_S_ID_A_ROVNASE
		case ROVNASEKONEC:	ll_prikaz_s_id_a_rovnase();
												break;
		default: return ERR_SYNTAX;
	}
	return ERR_OK;
}
int ll_prikaz_s_id_a_rovnase(){
	// pravidlo 18	<PŘÍKAZ_S_ID_A_ROVNASE> -> id ( VOLANI_PARAMETR )
	// pravidlo 19	<PŘÍKAZ_S_ID_A_ROVNASE> -> VYRAZ
	token = dej_mi_token();
	if (token.typ == IDKONEC){
		//volani fce
		token = dej_mi_token();
		if (token.typ != ZAVLEVA){
			return ERR_SYNTAX;
		}
		
		ll_volani_parametr();
		
		if (token.typ != ZAVPRAVA){
			return ERR_SYNTAX;
		}
	}else {
		chyba = syntaxe_vyrazu();
	}
	return chyba
}

int ll_volani_parametr(){
// pravidlo 20 <VOLANI_PARAMETR> -> VYRAZ , VOLANI_PARAMETR
	chyba = syntaxe_vyrazu();
	
	if (token.typ == CARKA){
		chyba=ll_volani_parametr();
	}
	return chyba;
}

int syntakticky_analyzator(tTabulkaSymbolu *TabulkaSymbolu, tVnitrni *vnitrni){
//zasobníkový automat - misto stavu jsou volany fce - zasobnik neni potreba
	int chyba=ERR_OK;
	int pom;
	
	while (true){
		chyba = ll_funkce();
		if (chyba != ERR_OK){
			return chyba;
		}
		// TODO: osetrit endoffile
	}// while
} 
