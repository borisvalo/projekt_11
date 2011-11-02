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
LL_LITERAL, // stav čekání na literál - číslo nebo řetězec
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
	
	ZasobnikInit(&zasobnik);
	ZasobnikPush(&zasobnik, ENDOFFILE); // konec souboru
	ZasobnikPush(&zasobnik, LL_FUNKCE); // počáteční stav
	
	while (!ZasobnikEmpty(&zasobnik)){
	
		if (novy_token == true){
			token = dej_mi_token();
			if (token.typ == IDKONEC){ // identifikatory testuji na klicova a rezervovana slova
				kontrola_identifikatoru(&token);
			}
		}
		switch (ZasobnikTop(&zasobnik)){
			case LL_FUNKCE:	if (token.typ == TNFUNCTION){ // pravidlo 1
												ZasobnikPop(&zasobnik);
												ZasobnikPush(&zasobnik,LL_FUNKCE);
												ZasobnikPush(&zasobnik,STREDNIK);
												ZasobnikPush(&zasobnik,TNEND);
												ZasobnikPush(&zasobnik,LL_PŘÍKAZY);
												ZasobnikPush(&zasobnik,LL_DEKLARACE);
												ZasobnikPush(&zasobnik,ZAVPRAVA);
												ZasobnikPush(&zasobnik,LL_PARAMETR);
												ZasobnikPush(&zasobnik,ZAVLEVA);
												ZasobnikPush(&zasobnik,IDKONEC);
												ZasobnikPush(&zasobnik,TNFUNCTION);    
											}else if (token.typ == ENDOFFILE){//pravidlo 2
												ZasobnikPop(&zasobnik);
											}else {
												chyba = true;
											}
											break;
			case LL_PARAMETR: if (token.typ == INTKONEC || token.typ == DESKONEC || token.typ == RETEZEC || token.typ == EXPKONEC || token.typ == TNTRUE || token.typ == TNFALSE || token.typ == TNNIL || token.typ == ZAVLEVA || token.typ == IDKONEC){// pravidlo 3
													ZasobnikPop(&zasobnik);
													ZasobnikPush(&zasobnik,LL_DALSI_PARAMETR);
													ZasobnikPush(&zasobnik,LL_VYRAZ);
												}else if(token.typ == ZAVPRAVA){// pravidlo 4
													ZasobnikPop(&zasobnik);
												}else {
													chyba = true;
												}
												break;
			case LL_DALSI_PARAMETR: if (token.typ == ZAVPRAVA){ // pravidlo 6
																ZasobnikPop(&zasobnik);
															} else if(token.typ == CARKA){ //pravidlo 5
																ZasobnikPop(&zasobnik);
																ZasobnikPush(&zasobnik,LL_DALSI_PARAMETR);
																ZasobnikPush(&zasobnik,LL_VYRAZ);
																ZasobnikPush(&zasobnik,CARKA);	
															}else {
																chyba = true;
															}
															break;
			case LL_DEKLARACE:	if (token.typ == IDKONEC || token.typ == TNIF || token.typ == TNRETURN || token.typ == TNWHILE){ // pravidlo 8
														ZasobnikPop(&zasobnik);
													}else if (token.typ == TNLOCAL){ // pravidlo 7
														ZasobnikPop(&zasobnik);
														ZasobnikPush(&zasobnik,LL_DEKLARACE);
														ZasobnikPush(&zasobnik,STREDNIK);
														ZasobnikPush(&zasobnik,LL_INICIALIZACE);
														ZasobnikPush(&zasobnik,IDKONEC);
														ZasobnikPush(&zasobnik,TNLOCAL);
													}else {
														chyba = true;
													}
													break;
			case LL_INICIALIZACE:	if (token.typ == ROVNASEKONEC) { // pravidlo 9
															ZasobnikPop(&zasobnik);
															ZasobnikPush(&zasobnik,LL_VYRAZ);
															ZasobnikPush(&zasobnik,ROVNASEKONEC);
														}else if (token.typ == STREDNIK){ // pravidlo 10
															ZasobnikPop(&zasobnik);
														}else {
															chyba = true;
														}
														break;
			case LL_PRIKAZY:	if (token.typ == IDKONEC) { // pravidlo 12
													ZasobnikPop(&zasobnik);
													ZasobnikPush(&zasobnik,LL_PRIKAZY);
													ZasobnikPush(&zasobnik,STREDNIK);
													ZasobnikPush(&zasobnik,LL_PRIKAZ_S_ID);
													ZasobnikPush(&zasobnik,IDKONEC);
												}else if (token.typ == TNIF){ // pravidlo 11
													ZasobnikPop(&zasobnik);
													ZasobnikPush(&zasobnik,LL_PRIKAZY);
													ZasobnikPush(&zasobnik,STREDNIK);
													ZasobnikPush(&zasobnik,TNEND);
													ZasobnikPush(&zasobnik,LL_PRIKAZY);
													ZasobnikPush(&zasobnik,TNELSE);
													ZasobnikPush(&zasobnik,LL_PRIKAZY);
													ZasobnikPush(&zasobnik,TNELSE);
													ZasobnikPush(&zasobnik,LL_PRIKAZY);
													ZasobnikPush(&zasobnik,TNTHEN);
													ZasobnikPush(&zasobnik,LL_VYRAZ);
													ZasobnikPush(&zasobnik,TNIF);
												}else if (token.typ == TNRETURN) { // pravidlo 13
													ZasobnikPop(&zasobnik);
													ZasobnikPush(&zasobnik,LL_PRIKAZY);
													ZasobnikPush(&zasobnik,STREDNIK);
													ZasobnikPush(&zasobnik,LL_VYRAZ);
													ZasobnikPush(&zasobnik,TNRETURN);
												}else if (token.typ == TNWHILE) { // pravidlo 14
													ZasobnikPop(&zasobnik);
													ZasobnikPush(&zasobnik,LL_PRIKAZY);
													ZasobnikPush(&zasobnik,STREDNIK);
													ZasobnikPush(&zasobnik,TNEND);
													ZasobnikPush(&zasobnik,TNRETURN);
													ZasobnikPush(&zasobnik,LL_PRIKAZY);
													ZasobnikPush(&zasobnik,TNDO);
													ZasobnikPush(&zasobnik,LL_VYRAZ);
													ZasobnikPush(&zasobnik,TNWHILE);
												}
												}else if (token.typ == TNEND) { // pravdidlo 15
													ZasobnikPop(&zasobnik);
												}else {
													chyba = true;
												}
												break;
			case LL_PRIKAZ_S_ID:	if(token.typ == ROVNASEKONEC) { // pravidlo 17
													ZasobnikPop(&zasobnik);
													ZasobnikPush(&zasobnik,LL_PRIKAZY_S_ID_A_ROVNASE);
													ZasobnikPush(&zasobnik,ROVNASEKONEC);
												}else if (token.typ == ZAVLEVA) { // pravidlo 16
													ZasobnikPop(&zasobnik);
													ZasobnikPush(&zasobnik,ZAVPRAVA);
													ZasobnikPush(&zasobnik,LL_PARAMETR);
													ZasobnikPush(&zasobnik,ZAVLEVA);
												}else {
													chyba = true;
												}
												break;
			case LL_PRIKAZ_S_ID_A_ROVNASE: if (token.typ == IDKONEC){ // pravidlo 18
																		ZasobnikPop(&zasobnik);
																		ZasobnikPush(&zasobnik,ZAVPRAVA);
																		ZasobnikPush(&zasobnik,LL_PARAMETR);
																		ZasobnikPush(&zasobnik,ZAVLEVA);
																		ZasobnikPush(&zasobnik,IDKONEC);	
																	}else if (token.typ == INTKONEC || token.typ == DESKONEC || token.typ == RETEZEC || token.typ == EXPKONEC || token.typ == TNTRUE || token.typ == TNFALSE || token.typ == TNNIL || token.typ == ZAVLEVA || token.typ == IDKONEC ) { // pravidlo 19
																		//vyraz
																		ZasobnikPop(&zasobnik);
																		ZasobnikPush(&zasobnik, LL_LITERAL);
																	} else {
																		chyba = true;
																	}
																	break;
	// zruseno - misto toho prijde LL_VYRAZ
			case LL_LITERAL:	if (token.typ == EXPKONEC){ // pravidlo 20
													ZasobnikPop(&zasobnik);
													ZasobnikPush(&zasobnik, EXPKONEC);
												}else if (token.typ == DESKONEC){ // pravidlo 21
													ZasobnikPop(&zasobnik);
													ZasobnikPush(&zasobnik, DESKONEC);
												}else if (token.typ == INTKONEC){ // pravidlo 22
													ZasobnikPop(&zasobnik);
													ZasobnikPush(&zasobnik, INTKONEC);
												}else if (token.typ == RETEZEC){ // pravidlo 23
													ZasobnikPop(&zasobnik);
													ZasobnikPush(&zasobnik, RETEZEC);
												} else if (token.typ == RETEZEC){ // pravidlo 23
													ZasobnikPop(&zasobnik);
													ZasobnikPush(&zasobnik, TNTRUE);
												} else if (token.typ == RETEZEC){ // pravidlo 23
													ZasobnikPop(&zasobnik);
													ZasobnikPush(&zasobnik, TNFALSE);
												} else if (token.typ == TNNIL){ // pravidlo 23
													ZasobnikPop(&zasobnik);
													ZasobnikPush(&zasobnik, RETEZEC);
												} else {
													chyba = true;
												}
												break;
				case LL_VYRAZ:	ZasobnikPop(&zasobnik);
				
												break;
		}//switch
		
		if (chyba==true){
			kod_Chyby = ERR_SYNTAX;
			break;
		}
		
		if (ZasobnikTop(&zasobnik) == token.typ) {
			ZasobnikPop(&zasobnik);
			if (token.typ == ENDOFFILE){
				kod_Chyby = ERR_OK;
				break;
			}
			//tady se bude generovat tříadresný kód
			novy_token = true;
		}else{
			novy_token = false;
		}
		
	}//while


	return kod_Chyby;
}
