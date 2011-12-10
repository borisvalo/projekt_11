/*
 *   Nazev projektu: Implementace interpretu imperativniho jazyka IFJ11
 * 
 *   Autori:   	Boris Valo, xvalob00
 * 				Pavel Slaby, xslaby00
 * 				Ondrej Vohanka, xvohan00
 * 				Matej Stepanek, xstepa43
 * 				Martina Stodolova, xstodo04 
 * 
 *   Datum odevzdani: 11.12.2011
 */


//potrebne knihovny:
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "string.h"
#include "obecne.h"
#include "scaner.h"
#include "parser.h"

//funkce pro alokovani pameti tokenu
int token_alokuj(UkTToken *strukt) {
    if (((*strukt) = (UkTToken) malloc(sizeof(TToken))) == NULL) {
        printf("chyba mallocu!!");
        return ERR_INTERNI;
    }
    (*strukt)->data = NULL;
    (*strukt)->delka = MIN_ARR;
    return ERR_OK;
}

//funkce pro uvolneni tokenu
void token_uvolni(UkTToken strukt) {
    switch (strukt->typ){
    	case IDKONEC:
			case RETEZEC:
      case INTKONEC:
      case EXPKONEC:
      case DESKONEC:
      case TNDO:
			case TNELSE:
			case TNEND: 
			case TNFALSE:
			case TNFUNCTION:
			case TNIF:
			case TNLOCAL:
			case TNNIL:
			case TNREAD: 
			case TNRETURN: 
			case TNTHEN:
			case TNTRUE:
			case TNWHILE:
			case TNWRITE:
			case RSAND:
			case RSBREAK:
			case RSELSEIF: 
			case RSFOR:
			case RSIN:
			case RSNOT:
			case RSOR:
			case RSREPEAT: 
			case RSUNTIL:
			case RSSORT:
			case RSFIND:
			case RSTYPE:
			case RSSUBSTR:
			case RSMAIN:
				Ret_uvolni(strukt->data);
    }
   
    free(strukt);
}

//-------------------------------------------------------
//popis:.................................................
//  funkce cte znaky a pomoci konecneho automatu rozpozna
//  tokeny a upravi zadanou strukturu, kde je urcen typ
//  tokenu a jeho pripadna data.
//parametry:.............................................
//  FILE *f          - soubor ze ktereho cteme
//  UkTToken strukt  - struktura tokenu
//navratove hodnoty:.....................................
//  KONEC_OK         - vse v poradku
//  KONEC_CHYBA      - nastala chyba
//-------------------------------------------------------

int ziskej_dalsi_token(FILE *f, UkTToken strukt) {
    int stav = INIT; //prvni stav je automaticky INIT
    int symbol;      //nacitany znak
    int citac = 0;   //pocita kolikaty znak jsme nacetli
    int cislo;       //cislo v \ddd
    
    while (1) {
        symbol = getc(f); //precteni znaku ze souboru
        //AUTOMAT ---------------------------------------
        switch (stav) {
            //...........................................
            //zakladni vstupni stav
            case INIT:
                if (symbol == '"') {
                    stav = Literal;
                    Ret_alokuj(&(strukt->data), MIN_ARR);
                    //strukt->data[citac] = symbol;
                    //citac++; //bude se cist dalsi znak
                }
                else if (symbol == ',') {
                    strukt->typ = CARKA;
                    return KONEC_OK;
                }
                else if (symbol == '(') {
                    strukt->typ = ZAVLEVA;
                    return KONEC_OK;
                }
                else if (symbol == ')') {
                    strukt->typ = ZAVPRAVA;
                    return KONEC_OK;
                }
                else if (symbol == ';') {
                    strukt->typ =  STREDNIK;
                    return KONEC_OK;
                }
                else if (symbol == '+') {
                    strukt->typ = PLUS;
                    return KONEC_OK;
                }
                else if (symbol == '*') {
                    strukt->typ = KRAT;
                    return KONEC_OK;
                }
                else if (symbol == '/') {
                    strukt->typ = DELENO;
                    return KONEC_OK;
                }
                else if (symbol == '.') {
                    stav = Tecka;
                }
                else if (symbol == '=') {
                    stav = RovnaSe;
                }
                else if (symbol == '^') {
					// dodelano a opraveno na MOCNINA
                    strukt->typ = MOCNINA;
                    return KONEC_OK;
                }
                else if (symbol == '>') {
                    stav = Vetsitko;
                }
                else if (symbol == '<') {
                    stav = Mensitko;
                }
                else if (symbol == '~') {
                    stav = Vlnovka;
                }
                else if (symbol == '-') {
                    stav = Minus;
                }
                //cislo
                else if (isdigit(symbol) != 0) {
                    stav = Int;
                    Ret_alokuj(&(strukt->data), MIN_ARR);
                    strukt->data[citac] = symbol;
                    citac++; //bude se cist dalsi znak
                }
                //prazdne znaky
                else if (isspace(symbol) != 0) {
                    stav = INIT;
                }
                //identifikator
                else if (isalpha(symbol) != 0 ||
                         symbol == '_') {
                    stav = Id;
                    Ret_alokuj(&(strukt->data), MIN_ARR);
                    strukt->data[citac] = symbol;
                    citac++; //bude se cist dalsi znak
                }
                //konec souboru
                else if (symbol == EOF) {
                    strukt->typ = ENDOFFILE;
                    return KONEC_OK;
                }
                //neznamy znak znamena chybu
                else {
                    strukt->typ = CHYBA;
                    return KONEC_CHYBA;
                }
                break;
            //...........................................
            //cislo
            case Int:
                if (symbol == '.') {
                    stav = Des;
                }
                else if (symbol == 'e' ||
                         symbol == 'E') {
                    stav = Exp;
                    if ((citac + 1) > strukt->delka) { //realokace
                        strukt->delka = strukt->delka * NAS_DEL;
                        Ret_realokuj(&(strukt->data), strukt->delka);
                    }
                    strukt->data[citac] = symbol;
                    citac++; //bude se cist dalsi znak
                }
                else if (isdigit(symbol) != 0) {
                    //stejny stav
                    stav = Int;
                    if ((citac + 1) > strukt->delka) { //realokace
                        strukt->delka = strukt->delka * NAS_DEL;
                        Ret_realokuj(&(strukt->data), strukt->delka);
                    }
                    strukt->data[citac] = symbol;
                    citac++; //bude se cist dalsi znak
                }
                else {
                    ungetc(symbol, f);
                    if ((citac + 1) > strukt->delka) { //realokace
                        strukt->delka = strukt->delka * NAS_DEL;
                        Ret_realokuj(&(strukt->data), strukt->delka);
                    }
                    strukt->data[citac] = '\0'; //ukonceni retezce
                    strukt->typ = INTKONEC;
                    return KONEC_OK;
                    
                }
                break;
            //...........................................
            //desetinne cislo
            case Des:
                if (symbol == 'e' ||
                    symbol == 'E') {
                    stav = Exp;
                    if ((citac + 1) > strukt->delka) { //realokace
                        strukt->delka = strukt->delka * NAS_DEL;
                        Ret_realokuj(&(strukt->data), strukt->delka);
                    }
                    strukt->data[citac] = symbol;
                    citac++; //bude se cist dalsi znak
                }
                else if (isdigit(symbol) != 0) {
                    //stejny stav
                    stav = Des;
                    if ((citac + 1) > strukt->delka) { //realokace
                        strukt->delka = strukt->delka * NAS_DEL;
                        Ret_realokuj(&(strukt->data), strukt->delka);
                    }
                    strukt->data[citac] = symbol;
                    citac++; //bude se cist dalsi znak
                }
                else {
                    ungetc(symbol, f);
                    if ((citac + 1) > strukt->delka) { //realokace
                        strukt->delka = strukt->delka * NAS_DEL;
                        Ret_realokuj(&(strukt->data), strukt->delka);
                    }
                    strukt->data[citac] = '\0'; //ukonceni retezce
                    strukt->typ = DESKONEC;
                    return KONEC_OK;
                    
                }
                break;
            //...........................................
            //cislo s exp
            case Exp:
                if (symbol == '+' ||
                    symbol == '-') {
                    stav = ExpPlusMinus;
                    if ((citac + 1) > strukt->delka) { //realokace
                        strukt->delka = strukt->delka * NAS_DEL;
                        Ret_realokuj(&(strukt->data), strukt->delka);
                    }
                    strukt->data[citac] = symbol;
                    citac++; //bude se cist dalsi znak
                }
                else if (isdigit(symbol) != 0) {
                    stav = Exp2;
                    if ((citac + 1) > strukt->delka) { //realokace
                        strukt->delka = strukt->delka * NAS_DEL;
                        Ret_realokuj(&(strukt->data), strukt->delka);
                    }
                    strukt->data[citac] = symbol;
                    citac++; //bude se cist dalsi znak
                }
                else {
                    strukt->typ = CHYBA;
                    return KONEC_CHYBA;
                }
                break;
            //...........................................
            //cislo s exp - pomocny
            case Exp2:
                if (isdigit(symbol) != 0) {
                    //stejny stav
                    stav = Exp2;
                    if ((citac + 1) > strukt->delka) { //realokace
                        strukt->delka = strukt->delka * NAS_DEL;
                        Ret_realokuj(&(strukt->data), strukt->delka);
                    }
                    strukt->data[citac] = symbol;
                    citac++; //bude se cist dalsi znak
                }
                else {
                    ungetc(symbol, f);
                    if ((citac + 1) > strukt->delka) { //realokace
                        strukt->delka = strukt->delka * NAS_DEL;
                        Ret_realokuj(&(strukt->data), strukt->delka);
                    }
                    strukt->data[citac] = '\0'; //ukonceni retezce
                    strukt->typ = EXPKONEC;
                    return KONEC_OK;
                }
                break;
            //...........................................
            //exp muze byt i zaporny
            case ExpPlusMinus:
                if (isdigit(symbol) != 0) {
                    stav = Exp2;
                    if ((citac + 1) > strukt->delka) { //realokace
                        strukt->delka = strukt->delka * NAS_DEL;
                        Ret_realokuj(&(strukt->data), strukt->delka);
                    }
                    strukt->data[citac] = symbol;
                    citac++; //bude se cist dalsi znak
                }
                else {
                    strukt->typ = CHYBA;
                    return KONEC_CHYBA;
                }
                break;
            
            //...........................................
            //strukt->data v uvozovkach
            case Literal:
                if (symbol == '\\') {
                    stav = Escape;
                }
                else if (symbol == '"') {
                    if ((citac + 1) > strukt->delka) { //realokace
                        strukt->delka = strukt->delka * NAS_DEL;
                        Ret_realokuj(&(strukt->data), strukt->delka);
                    }
                    strukt->data[citac] = '\0'; //ukonceni retezce
                    strukt->typ = RETEZEC;
                    return KONEC_OK;
                }
                else {
                    if ((citac + 1) > strukt->delka) { //realokace
                        strukt->delka = strukt->delka * NAS_DEL;
                        Ret_realokuj(&(strukt->data), strukt->delka);
                    }
                    strukt->data[citac] = symbol;
                    citac++; //bude se cist dalsi znak
                }
                break;
                
            //...........................................
            //osetreni escape sekvence
            case Escape:
                if (symbol == 't' ||
                    symbol == 'n' ||
                    symbol == '\\' ||
                    symbol == '"') {
                    if ((citac + 1) > strukt->delka) { //realokace
                        strukt->delka = strukt->delka * NAS_DEL;
                        Ret_realokuj(&(strukt->data), strukt->delka);
                    }
                    //osetreni escape sekvenci
                    if (symbol == 't') { //tabulator
                        strukt->data[citac] = '\t';
                    }
                    else if (symbol == 'n') { //novy radek
                        strukt->data[citac] = '\n';
                    }
                    else { //v ostatnich pripadech staci nakopirovat znak za '\'
                        strukt->data[citac] = symbol;
                    }
                    citac++; //bude se cist dalsi znak
                    stav = Literal;
                }
                else if (isdigit(symbol)) {
                    //escape sekvence \ddd - precteme 3 cisla
                    cislo = (symbol - '0') * 100; //prvni jsou stovky
                    if (!isdigit(symbol = getc(f))) {
                        strukt->typ = CHYBA;
                        return KONEC_CHYBA;
                    }
                    cislo += (symbol - '0') * 10; //druhe desitky
                    if (!isdigit(symbol = getc(f))) {
                        strukt->typ = CHYBA;
                        return KONEC_CHYBA;
                    }
                    cislo += (symbol - '0'); //treti jednotky
                    if (cislo > 255) { //cislo je moc velke -> chyba
                        strukt->typ = CHYBA;
                        return KONEC_CHYBA;
                    }
                    
                    if ((citac + 1) > strukt->delka) { //realokace
                        strukt->delka = strukt->delka * NAS_DEL;
                        Ret_realokuj(&(strukt->data), strukt->delka);
                    }
                    strukt->data[citac] = (char) cislo;
                    citac++; //bude se cist dalsi znak
                    stav = Literal;
                }
                else {
                    strukt->typ = CHYBA;
                    return KONEC_CHYBA;
                }
                
                break;
                
            //...........................................
            //minus
            case Minus:
                if (symbol == '-') {
                    stav = Kom;
                }
                else {
                    ungetc(symbol, f);
                    strukt->typ = MINUSKONEC;
                    return KONEC_OK;
                }
                break;
                
            //...........................................
            //radkovy komentar
            case Kom:
                if (symbol == '[') {
                    stav = KomBlok;
                }
                else if (symbol == '\n') {
                    stav = INIT;
                }
                else {
                    stav = KomRadk;
                }
                break;
                
            //...........................................
            //blokovy komentar
            case KomBlok:
                if (symbol == '[') {
                    stav = KomBlok2;
                }
                else {
                    stav = KomRadk;
                }
                break;
                
            //...........................................
            case KomBlok2:
                if (symbol == ']') {
                    stav = KomBlokK;
                }
                else {
                    //zustavame ve stejnem stavu
                    stav = KomBlok2;
                }
                break;
                
            //...........................................
            //konec blokoveho komentare
            case KomBlokK:
                if (symbol == ']') {
                    stav = INIT;
                }
                else {
                    stav = KomBlok2;
                }
                break;
                
            //...........................................
            //cteni znaku radkoveho komentare
            case KomRadk:
                if (symbol == '\n') {
                    stav = INIT;
                }
                else {
                    //stejny stav
                    stav = KomRadk;
                }
                break;
                
            //...........................................
            //tecka
            case Tecka:
                if (symbol == '.') {
                    strukt->typ = KONKATENACE;
                    return KONEC_OK;
                }
                else {
                    strukt->typ = CHYBA;
                    return KONEC_CHYBA;
                }
                break;
                
            //...........................................
            case RovnaSe:
                if (symbol == '=') {
                    strukt->typ = POROVNANI;
                    return KONEC_OK;
                }
                else {
                    ungetc(symbol, f);
                    strukt->typ = ROVNASEKONEC;
                    return KONEC_OK;
                }
                break;
                
            //...........................................
            case Vetsitko:
                if (symbol == '=') {
                    strukt->typ = VETSIROVNO;
                    return KONEC_OK;
                }
                else {
                    ungetc(symbol, f);
                    strukt->typ = VETSITKOKONEC;
                    return KONEC_OK;
                }
                break;
                
            //...........................................
            case Mensitko:
                if (symbol == '=') {
                    strukt->typ = MENSIROVNO;
                    return KONEC_OK;
                }
                else {
                    ungetc(symbol, f);
                    strukt->typ = MENSITKOKONEC;
                    return KONEC_OK;
                }
                break;
                
            //...........................................
            case Vlnovka:
                if (symbol == '=') {
                    strukt->typ = NEROVNASE;
                    return KONEC_OK;
                }
                else {
                    strukt->typ = CHYBA;
                    return KONEC_CHYBA;
                }
                break;
                
            //...........................................
            //identifikator
            case Id:
                if (isalpha(symbol) != 0 ||
                    isdigit(symbol) != 0 ||
                    symbol == '_') {
                    stav = Id; //stejny stav
                    if ((citac + 2) > strukt->delka) { //realokace
                        strukt->delka = strukt->delka * NAS_DEL;
                        Ret_realokuj(&(strukt->data), strukt->delka);
                    }
                    strukt->data[citac] = symbol;
                    citac++; //bude se cist dalsi znak
                }
                else { //konec retezce
                    ungetc(symbol, f);
                    if ((citac + 2) > strukt->delka) { //realokace
                        strukt->delka = strukt->delka * NAS_DEL;
                        Ret_realokuj(&(strukt->data), strukt->delka);
                    }
                    strukt->data[citac] = '\0'; //ukonceni retezce
                    strukt->typ = IDKONEC;
                    return KONEC_OK;
                }
                break;
                
            //...........................................
                
            default:
                break;
        }
    }
}
