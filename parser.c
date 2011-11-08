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

int syntakticky_analyzator(tTabulkaSymbolu *TabulkaSymbolu, tVnitrni *vnitrni){
//zasobníkový automat
	int chyba=ERR_OK;
	int pom;
	
	zasobnik_init(&zasobnik);
	zasobnik_push(&zasobnik, ENDOFFILE); // konec souboru
	zasobnik_push(&zasobnik, LL_FUNKCE); // počáteční stav
	
	while (!zasobnik_empty(&zasobnik)){
	
		if (novy_token == true){
			token = dej_mi_token();
			if (token.typ == IDKONEC){ // identifikatory testuji na klicova a rezervovana slova
				kontrola_identifikatoru(&token);
			}
		}
		switch (zasobnik_top(&zasobnik)){
			case LL_FUNKCE:	if (token.typ == TNFUNCTION){ // pravidlo 1
												zasobnik_pop(&zasobnik);
												zasobnik_push(&zasobnik,LL_FUNKCE);
												zasobnik_push(&zasobnik,STREDNIK);
												zasobnik_push(&zasobnik,TNEND);
												zasobnik_push(&zasobnik,LL_PŘÍKAZY);
												zasobnik_push(&zasobnik,LL_DEKLARACE);
												zasobnik_push(&zasobnik,ZAVPRAVA);
												zasobnik_push(&zasobnik,LL_PARAMETR);
												zasobnik_push(&zasobnik,ZAVLEVA);
												zasobnik_push(&zasobnik,IDKONEC);
												zasobnik_push(&zasobnik,TNFUNCTION);    
											}else if (token.typ == ENDOFFILE){//pravidlo 2
												zasobnik_pop(&zasobnik);
											}else {
												chyba = ERR_SYNTAX;
											}
											break;
			case LL_PARAMETR: if (token.typ == INTKONEC || token.typ == DESKONEC || token.typ == RETEZEC || token.typ == EXPKONEC || token.typ == TNTRUE || token.typ == TNFALSE || token.typ == TNNIL || token.typ == ZAVLEVA || token.typ == IDKONEC){// pravidlo 3
													zasobnik_pop(&zasobnik);
													zasobnik_push(&zasobnik,LL_DALSI_PARAMETR);
													zasobnik_push(&zasobnik,LL_VYRAZ);
												}else if(token.typ == ZAVPRAVA){// pravidlo 4
													zasobnik_pop(&zasobnik);
												}else {
													chyba = ERR_SYNTAX;
												}
												break;
			case LL_DALSI_PARAMETR: if (token.typ == ZAVPRAVA){ // pravidlo 6
																zasobnik_pop(&zasobnik);
															} else if(token.typ == CARKA){ //pravidlo 5
																zasobnik_pop(&zasobnik);
																zasobnik_push(&zasobnik,LL_DALSI_PARAMETR);
																zasobnik_push(&zasobnik,LL_VYRAZ);
																zasobnik_push(&zasobnik,CARKA);	
															}else {
																chyba = ERR_SYNTAX;
															}
															break;
			case LL_DEKLARACE:	if (token.typ == IDKONEC || token.typ == TNIF || token.typ == TNRETURN || token.typ == TNWHILE){ // pravidlo 8
														zasobnik_pop(&zasobnik);
													}else if (token.typ == TNLOCAL){ // pravidlo 7
														zasobnik_pop(&zasobnik);
														zasobnik_push(&zasobnik,LL_DEKLARACE);
														zasobnik_push(&zasobnik,STREDNIK);
														zasobnik_push(&zasobnik,LL_INICIALIZACE);
														zasobnik_push(&zasobnik,IDKONEC);
														zasobnik_push(&zasobnik,TNLOCAL);
													}else {
														chyba = ERR_SYNTAX;
													}
													break;
			case LL_INICIALIZACE:	if (token.typ == ROVNASEKONEC) { // pravidlo 9
															zasobnik_pop(&zasobnik);
															zasobnik_push(&zasobnik,LL_VYRAZ);
															zasobnik_push(&zasobnik,ROVNASEKONEC);
														}else if (token.typ == STREDNIK){ // pravidlo 10
															zasobnik_pop(&zasobnik);
														}else {
															chyba = ERR_SYNTAX;
														}
														break;
			case LL_PRIKAZY:	if (token.typ == IDKONEC) { // pravidlo 12
													zasobnik_pop(&zasobnik);
													zasobnik_push(&zasobnik,LL_PRIKAZY);
													zasobnik_push(&zasobnik,STREDNIK);
													zasobnik_push(&zasobnik,LL_PRIKAZ_S_ID);
													zasobnik_push(&zasobnik,IDKONEC);
												}else if (token.typ == TNIF){ // pravidlo 11
													zasobnik_pop(&zasobnik);
													zasobnik_push(&zasobnik,LL_PRIKAZY);
													zasobnik_push(&zasobnik,STREDNIK);
													zasobnik_push(&zasobnik,TNEND);
													zasobnik_push(&zasobnik,LL_PRIKAZY);
													zasobnik_push(&zasobnik,TNELSE);
													zasobnik_push(&zasobnik,LL_PRIKAZY);
													zasobnik_push(&zasobnik,TNTHEN);
													zasobnik_push(&zasobnik,LL_VYRAZ);
													zasobnik_push(&zasobnik,TNIF);
												}else if (token.typ == TNRETURN) { // pravidlo 13
													zasobnik_pop(&zasobnik);
													zasobnik_push(&zasobnik,LL_PRIKAZY);
													zasobnik_push(&zasobnik,STREDNIK);
													zasobnik_push(&zasobnik,LL_VYRAZ);
													zasobnik_push(&zasobnik,TNRETURN);
												}else if (token.typ == TNWHILE) { // pravidlo 14
													zasobnik_pop(&zasobnik);
													zasobnik_push(&zasobnik,LL_PRIKAZY);
													zasobnik_push(&zasobnik,STREDNIK);
													zasobnik_push(&zasobnik,TNEND);
													zasobnik_push(&zasobnik,LL_PRIKAZY);
													zasobnik_push(&zasobnik,TNDO);
													zasobnik_push(&zasobnik,LL_VYRAZ);
													zasobnik_push(&zasobnik,TNWHILE);
												}
												}else if (token.typ == TNEND) { // pravdidlo 15
													zasobnik_pop(&zasobnik);
												}else {
													chyba = ERR_SYNTAX;
												}
												break;
			case LL_PRIKAZ_S_ID:	if(token.typ == ROVNASEKONEC) { // pravidlo 17
															zasobnik_pop(&zasobnik);
															zasobnik_push(&zasobnik,LL_PRIKAZY_S_ID_A_ROVNASE);
															zasobnik_push(&zasobnik,ROVNASEKONEC);
														}else if (token.typ == ZAVLEVA) { // pravidlo 16
															zasobnik_pop(&zasobnik);
															zasobnik_push(&zasobnik,ZAVPRAVA);
															zasobnik_push(&zasobnik,LL_PARAMETR);
															zasobnik_push(&zasobnik,ZAVLEVA);
														}else {
															chyba = ERR_SYNTAX;
														}
														break;
			case LL_PRIKAZ_S_ID_A_ROVNASE: if (token.typ == IDKONEC){ // pravidlo 18
																				zasobnik_pop(&zasobnik);
																				zasobnik_push(&zasobnik,ZAVPRAVA);
																				zasobnik_push(&zasobnik,LL_PARAMETR);
																				zasobnik_push(&zasobnik,ZAVLEVA);
																				zasobnik_push(&zasobnik,IDKONEC);	
																			}else if (token.typ == INTKONEC || token.typ == DESKONEC || token.typ == RETEZEC || token.typ == EXPKONEC || token.typ == TNTRUE || token.typ == TNFALSE || token.typ == TNNIL || token.typ == ZAVLEVA || token.typ == IDKONEC ) { // pravidlo 19
																				//vyraz
																				zasobnik_pop(&zasobnik);
																				zasobnik_push(&zasobnik, LL_VYRAZ);
																			} else {
																				chyba = ERR_SYNTAX;
																			}
																			break;
				case LL_VYRAZ:	syntax_vyrazu(&zasobnik, &dalsi_znak);
												zasobnik_pop(&zasobnik);
												break;
				case IDKONEC: // semanticke akce
											break;
				case TNIF:	generuj_instrukci_if();
		}//switch
		
		
		if (chyba!=ERR_OK){ //chyba ze switche
			break;
		}
		if (zasobnik_top(&zasobnik, &pom)!=ERR_OK){
			chyba = ERR_INTERNI;
			break;
		}
		if (pom == token.typ) {
			zasobnik_pop(&zasobnik);
			if (token.typ == ENDOFFILE){
				chyba = ERR_OK;
				break;
			}
			novy_token = true;
		}else{
			novy_token = false;
		}
		
	}//while


	return chyba;
}
// fce vloz_instrukci
