/* ========================================================================== */
/*                                                                            */
/*   vyrazy.c                                                            */
/*   Author: Martina Stodolova xstodo04                                       */
/*                                                                            */
/* ========================================================================== */


#include "scaner.h"   // kvuli tokenum
#include "zasobnik.c"   // pro praci se zasobnikem
#include "obecne.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>


// UDELAT SI TESTY !!!
// VYRESIT PROBLEMY S MALLOCEM --->>> uz snad dobre :-)
// ZLEPSIT KOD
// KOMENTY
// LEPSI POJMENOVANI
// CO S TIMHLE, KDYZ MI TO TAKY UZNAVA ZA DOBRE :
//                  pom +( i == (((3 + 2)) ^ 4))
// OSETRIT Chybne souradnice precedencni tabulky!!!

// ZMENY V ONDROVE KODU
// DODELAT TODO U ZASOBNIKU


// pokud je IDKONEC - zavolat Pavlovu kontrola_identifikatoru !!! Kvuli FALSE, TRUE, NIL

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
    DOLAR = 15         // $

} typ_operace;



// Funkce na prevedeni Ondrova tokenu na muj OP:
int preved_z_tokenu(int dalsi_token) {

    switch(dalsi_token) {

        case INTKONEC:
            return OPINTEGER;
        case DESKONEC:
            return OPDESETINNE;
        case EXPKONEC:
            return OPEXPCISLO;
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




// musime otestovat vsechny zasob. funkce pro pripad jaderne havarie -> err_chyba = ERR_INTERNI
//                                                                jinak err_chyba = ERR_SYNTAX

//
int syntax_vyrazu(FILE *soubor, int* pom_pavel1, int* pom_pavel2) {
//int syntax_vyrazu(TZasobnik* zasobnik, int co_je_za_znak_za_vyrazem) {


    int pom1, pom2, pom3, pom4;
    int znamenko;   // co se bude provadet, podle precedencni tabulky
    int uk_na_terminal;  // ukazatel na "nejvyssi" terminal
    int dalsi_token;
    bool novy_token_ano_ne = true;
    bool redukce = false;
    int err_chyba = ERR_OK;
    UkTToken strukt;

    //int pom_pavel1, pom_pavel2;

    TZasobnik zasobnik;

    if(zasobnik_init(&zasobnik) == ERR_INTERNI) {   // inicializace zasobniku pro vyrazy
        err_chyba = ERR_INTERNI;
    }

    if(zasobnik_push(&zasobnik, DOLAR) == ERR_INTERNI) {
        err_chyba = ERR_INTERNI;
    }

    uk_na_terminal = zasobnik.top;   // nejvyssi terminal je ted $

    ziskej_dalsi_token(soubor, strukt);   // otestovat navratovy kod
    dalsi_token = strukt->typ;



//////////////////////////////////////////////
// while(/*  nacitam to,co je soucasti meho mileho vyrazu */) {

    while((redukce == true) ||
        (dalsi_token == IDKONEC) || (dalsi_token == INTKONEC) || (dalsi_token == DESKONEC) ||
        (dalsi_token == EXPKONEC) || (dalsi_token == RETEZEC) || (dalsi_token == ZAVLEVA) ||
        (dalsi_token == ZAVPRAVA) || (dalsi_token == PLUS) || (dalsi_token == MINUSKONEC) ||
        (dalsi_token == KRAT) || (dalsi_token == DELENO) || (dalsi_token == MOCNINA) ||
        (dalsi_token == KONKATENACE) || (dalsi_token == POROVNANI) || (dalsi_token == NEROVNASE) ||
        (dalsi_token == VETSIROVNO) || (dalsi_token == VETSITKOKONEC) || (dalsi_token == MENSIROVNO) ||
        (dalsi_token == MENSITKOKONEC) || (dalsi_token == TNNIL) || (dalsi_token == TNFALSE) ||
        (dalsi_token == TNTRUE) || (znamenko == VETSI)) {



// JAK TO MA ONDRA S LEX ANALYZOU KLICOVYCH A REZERVOVANYCH SLOV !?!??!?!?!?



// testovat return funkce ZISKEJ_DALSI_TOKEN ptz by to mohla byt lex chyba !!!!!!!!




//////////////////////////////////////////////////////////////////////////////
        // MUSIME ZKONTROLOVAT, JESTLI ZA VYRAZEM NENI ZAVORKA
        //       JESTLI TO NEVYPADA TREBA TAKHLE:
        //            write(retezec, 4+2);
        //     --> ME UZ TA ZAVORKA NEMA PATRIT - POZNAM PODLE TOKENU,KTERY JE ZA NI

        if(zasobnik_pristup(&zasobnik, &pom1, 0) == ERR_INTERNI) {
            err_chyba = ERR_INTERNI;
        }

        if(zasobnik_pristup(&zasobnik, &pom2, 1) == ERR_INTERNI) {
            err_chyba = ERR_INTERNI;
        }


        //if(na zasobniku je pouze $E && na vstupni pasce je ")" )
        if((pom2 == DOLAR) && (pom1 == NETERMINAL) && (dalsi_token == ZAVPRAVA) && (zasobnik.top == 1)) {

            *pom_pavel1 = dalsi_token;

            ziskej_dalsi_token(soubor, strukt);   // otestovat navratovy kod
            dalsi_token = strukt->typ;

            if((dalsi_token != ZAVPRAVA) && (dalsi_token != PLUS) && (dalsi_token != MINUSKONEC) &&
                (dalsi_token != KRAT) && (dalsi_token != DELENO) && (dalsi_token != KONKATENACE) &&
                (dalsi_token != MOCNINA) && (dalsi_token != POROVNANI) && (dalsi_token != NEROVNASE) &&
                (dalsi_token != VETSIROVNO) && (dalsi_token != VETSITKOKONEC) && (dalsi_token != MENSIROVNO) &&
                (dalsi_token != MENSITKOKONEC)) {

                *pom_pavel2 = dalsi_token;

                printf("OK - vracim tokeny SA \n");

                break;   // musi se breaknout while a vratit SA posledni dva tokeny
                        //  , ktere patri jemu
            }

            else {
                err_chyba = ERR_SYNTAX;
                break;
            }


        }


/////////////////////////////////////////////////////////////////



        if(redukce == true) {
            znamenko = VETSI;
        }

        else {
            pom1 = preved_z_tokenu(dalsi_token);
            znamenko = preced_tabulka[preved_z_tokenu(zasobnik.array[uk_na_terminal])][pom1];
        }


        if(znamenko == PTCHYBA) {
            err_chyba = ERR_SYNTAX;
            break;
        }


        else if(znamenko == ROVNO) {

            if(zasobnik_push(&zasobnik, dalsi_token) == ERR_INTERNI) {
                err_chyba = ERR_INTERNI;
            }


            uk_na_terminal = zasobnik.top;   // pozice terminalu - je to ten na vrcholu zasobniku
            novy_token_ano_ne = true;
        }


        else if(znamenko == MENSI) {

            // za posledni terminal vloz "<"
            if(zasobnik_push(&zasobnik, MENSI) == ERR_INTERNI) {
                err_chyba = ERR_INTERNI;
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


            if(zasobnik_push(&zasobnik, dalsi_token) == ERR_INTERNI) {
                err_chyba = ERR_INTERNI;
            }


            uk_na_terminal = zasobnik.top;   // pozice terminalu - je to ten na vrcholu zasobniku
            novy_token_ano_ne = true;

        }


        else if(znamenko == VETSI) {    // hledame pravidlo


            if(zasobnik_pristup(&zasobnik, &pom1, 0) == ERR_INTERNI) {
                err_chyba = ERR_INTERNI;
            }

            pom1 = preved_z_tokenu(pom1);

            // pokud se pri redukci prvek na zasobniku nerovna nicemu z case pod, tak to musim breaknout,
            // ptz se neda redukovat podle zadnyho pravidla !!!
            if((pom1 != OPIDENTIFIKATOR) && (pom1 != NETERMINAL) && (pom1 != OPPRAVAZAVORKA)) {
                printf("Neodpovida zadnemu pravidlu --> BREAK\n");
                err_chyba = ERR_SYNTAX;
                break;
            }


            switch(pom1) {

                case OPIDENTIFIKATOR:
                //case OPINTEGER:
                //case OPDESETINNE:
                //case OPEXPCISLO:
                //case OPRETEZEC:


                    if(zasobnik_pristup(&zasobnik, &pom1, 1) == ERR_INTERNI) {
                        err_chyba = ERR_INTERNI;
                    }


                    if(pom1 == MENSI) {

                        if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                            err_chyba = ERR_INTERNI;
                        }

                        if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                            err_chyba = ERR_INTERNI;
                        }

                        if(zasobnik_push(&zasobnik, NETERMINAL) == ERR_INTERNI) {
                            err_chyba = ERR_INTERNI;
                        }

                    }

                    else {
                        err_chyba = ERR_SYNTAX;
                        break;
                    }


                    break;


                case OPPRAVAZAVORKA:

                    if(zasobnik_pristup(&zasobnik, &pom1, 1) == ERR_INTERNI) {
                        err_chyba = ERR_INTERNI;
                    }

                    if(zasobnik_pristup(&zasobnik, &pom2, 2) == ERR_INTERNI) {
                        err_chyba = ERR_INTERNI;
                    }

                    if(zasobnik_pristup(&zasobnik, &pom3, 3) == ERR_INTERNI) {
                        err_chyba = ERR_INTERNI;
                    }

                    pom2 = preved_z_tokenu(pom2);

                    if((pom1 == NETERMINAL) && (pom2 == OPLEVAZAVORKA) && (pom3 == MENSI)) {

                        if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                            err_chyba = ERR_INTERNI;
                        }

                        if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                            err_chyba = ERR_INTERNI;
                        }

                        if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                            err_chyba = ERR_INTERNI;
                        }

                        if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                            err_chyba = ERR_INTERNI;
                        }

                        if(zasobnik_push(&zasobnik, NETERMINAL) == ERR_INTERNI) {
                            err_chyba = ERR_INTERNI;
                        }

                    }

                    else {
                        err_chyba = ERR_SYNTAX;
                        break;
                    }

                    break;


                case NETERMINAL:

                    zasobnik_pristup(&zasobnik, &pom1, 1);
                    pom1 = preved_z_tokenu(pom1);

                    if((pom1 != OPPLUS) && (pom1 != OPMINUS) && (pom1 != OPKRAT) &&
                        (pom1 != OPDELENO) && (pom1 != OPMOCNINA) && (pom1 != OPJEROVNO) &&
                        (pom1 != OPNENIROVNO) && (pom1 != OPMENSITKO) && (pom1 != OPVETSITKO) &&
                        (pom1 != OPMENSITKOROVNO) && (pom1 != OPVETSITKOROVNO) && (pom1 != OPKONKATENACE)) {

                        printf("Neodpovida zadnemu pravidlu --> BREAK\n");
                        err_chyba = ERR_SYNTAX;
                        break;
                    }

                    switch(pom1) {

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

                            if(zasobnik_pristup(&zasobnik, &pom1, 2) == ERR_INTERNI) {
                                err_chyba = ERR_INTERNI;
                            }

                            if(zasobnik_pristup(&zasobnik, &pom2, 3) == ERR_INTERNI) {
                                err_chyba = ERR_INTERNI;
                            }

                            if((pom1 == NETERMINAL) && (pom2 == MENSI)) {

                                if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                                    err_chyba = ERR_INTERNI;
                                }

                                if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                                    err_chyba = ERR_INTERNI;
                                }

                                if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                                    err_chyba = ERR_INTERNI;
                                }

                                if(zasobnik_pop(&zasobnik) == ERR_INTERNI) {
                                    err_chyba = ERR_INTERNI;
                                }

                                if(zasobnik_push(&zasobnik, NETERMINAL) == ERR_INTERNI) {
                                    err_chyba = ERR_INTERNI;
                                }

                            }

                            else {
                                err_chyba = ERR_SYNTAX;
                                break;
                            }

                            break;

                    }

                    break;

            }   // konec vetsiho switch

            // pokud je err_chyba, break; z while
            if((err_chyba == ERR_SYNTAX) || (err_chyba == ERR_INTERNI)) {
                printf("NASTALA CHYBA\n");
                break;
            }

            // nastavime novy nejvyssi terminal
            for(int j = 0; j <= zasobnik.top; j++) {
                zasobnik_pristup(&zasobnik, &pom1, j);
                pom1 = preved_z_tokenu(pom1);

                if((pom1 != NETERMINAL) && (pom1 != MENSI)) {
                    uk_na_terminal = zasobnik.top - j;
                    break;
                }
            }


            novy_token_ano_ne = false;

        }   // konec else if(znamenko == VETSI)


        if(novy_token_ano_ne == true) {
            ziskej_dalsi_token(soubor, strukt);   // otestovat navratovy kod
            dalsi_token = strukt->typ;
        }

        // PSAT KOMENTY K PRAVIDLUM


        // pokud je vse u konce,  breaknem to
        if(((dalsi_token != IDKONEC) && (dalsi_token != INTKONEC) && (dalsi_token != DESKONEC) &&
            (dalsi_token != EXPKONEC) && (dalsi_token != RETEZEC) && (dalsi_token != ZAVLEVA) &&
            (dalsi_token != ZAVPRAVA) && (dalsi_token != PLUS) && (dalsi_token != MINUSKONEC) &&
            (dalsi_token != KRAT) && (dalsi_token != DELENO) && (dalsi_token != MOCNINA) &&
            (dalsi_token != KONKATENACE) && (dalsi_token != POROVNANI) && (dalsi_token != NEROVNASE) &&
            (dalsi_token != VETSIROVNO) && (dalsi_token != VETSITKOKONEC) && (dalsi_token != MENSIROVNO) &&
            (dalsi_token != MENSITKOKONEC) && (dalsi_token != TNNIL) && (dalsi_token != TNFALSE) &&
            (dalsi_token != TNTRUE)) &&
            (zasobnik.top == 1) && (zasobnik.array[1] == NETERMINAL) && (zasobnik.array[0] == DOLAR)) {

            printf("OK\n");
                break;
        }


        redukce = false;

        // POKUD DALSI NACTENY ZNAK NAM UZ NEPATRI,
        // ALE JA POTREBUJI REDUKOVAT PODLE PRAVIDEL,
        // MUSI BYT PODMINKA
        if(((dalsi_token != IDKONEC) && (dalsi_token != INTKONEC) && (dalsi_token != DESKONEC) &&
            (dalsi_token != EXPKONEC) && (dalsi_token != RETEZEC) && (dalsi_token != ZAVLEVA) &&
            (dalsi_token != ZAVPRAVA) && (dalsi_token != PLUS) && (dalsi_token != MINUSKONEC) &&
            (dalsi_token != KRAT) && (dalsi_token != DELENO) && (dalsi_token != MOCNINA) &&
            (dalsi_token != KONKATENACE) && (dalsi_token != POROVNANI) && (dalsi_token != NEROVNASE) &&
            (dalsi_token != VETSIROVNO) && (dalsi_token != VETSITKOKONEC) && (dalsi_token != MENSIROVNO) &&
            (dalsi_token != MENSITKOKONEC) && (dalsi_token != TNNIL) && (dalsi_token != TNFALSE) &&
            (dalsi_token != TNTRUE)) && (zasobnik.top > 1)) {

            redukce = true;

        }



    }   // konec while



// ALE TAK STEJNE MUSIM SA VRATIT TEN POSLEDNI TOKEN, KTERY JSME NACETLI

    if(*pom_pavel1 == 0) {
        *pom_pavel1 = dalsi_token;
    }


    // pokud uz na vstupni pasce nic neni, ale na zasobniku je neco jineho nez $E,
    // tak je to synt chyba
    if(zasobnik_pristup(&zasobnik, &pom2, 0) == ERR_INTERNI) {
        err_chyba = ERR_INTERNI;
    }


    // pokud byl vyraz prazdny, tzn ze prvni precteny token nepatril do vyrazu, tak je to OK
    if((pom2 == DOLAR) && (zasobnik.top == 0)) {
        printf("Prazdny vyraz\n");
        return err_chyba;
    }

    if(zasobnik_pristup(&zasobnik, &pom2, 1) == ERR_INTERNI) {
        err_chyba = ERR_INTERNI;
    }

    if(zasobnik_pristup(&zasobnik, &pom1, 0) == ERR_INTERNI) {
        err_chyba = ERR_INTERNI;
    }

    if(!((pom1 == NETERMINAL) && (pom2 == DOLAR) && (zasobnik.top == 1))) {
        err_chyba = ERR_SYNTAX;
        printf("CHYBA\n");
    }


   return err_chyba;

}  // konec funkce syntax_vyrazu()



/*
// Ted je tu jen za ucelem testovani
int main() {


    int pom_pavel1, pom_pavel2;

    char nazevsouboru[10] = "test1";
    FILE *soubor;

    soubor = fopen(nazevsouboru, "r");

    //otevreni(nazevsouboru, soubor);

    syntax_vyrazu(soubor, &pom_pavel1, &pom_pavel2);

    zavreni(soubor);


    return 0;
}
*/

