// scaner.h

//pomocne definice:
#define MAX_ARR     20
#define KONEC_OK    0
#define KONEC_CHYBA 1

//vyctovy typ se vsemi stavy automatu
typedef enum {
    INIT, Int, Des, Exp, Exp2, ExpPlusMinus, Literal,
    Escape, Minus, Kom, KomBlok, KomBlok2, KomBlokK,
    KomRadk, Tecka, Konkatenace, RovnaSe, Vetsitko,
    VetsitkoKonec, Mensitko, MensiRovno, Vlnovka, Id,
} STAVY;

//tokeny - konecne stavy
typedef enum {
    CHYBA, //vrati v pripade chybove situace
    
    //standartni tokeny
    INTKONEC,      // integer cislo
    DESKONEC,      // desetinne cislo
    EXPKONEC,      // cislo s 'e' nebo 'E'
    RETEZEC,       // retezec
    CARKA,         // ,
    ZAVLEVA,       // (
    ZAVPRAVA,      // )
    STREDNIK,      // ;
    PLUS,          // +
    KRAT,          // *
    DELENO,        // :
    KONKATENACE,   // ..
    ROVNASEKONEC,  // =
    POROVNANI,     // ==
    MOCNINA,       // ^
    VETSIROVNO,    // >=
    VETSITKOKONEC, // >
    MENSIROVNO,    // <=
    MENSITKOKONEC, // <
    NEROVNASE,     // ~=
    IDKONEC,       // identifikator
    MINUSKONEC,    // -
    ENDOFFILE,     // EOF
    
    //klicova slova
    TNDO, TNELSE, TNEND, TNFALSE, TNFUNCTION, TNIF, TNLOCAL,
    TNNIL, TNREAD, TNRETURN, TNTHEN, TNTRUE, TNWHILE, TNWRITE,
    
    REZSL, //rezervovana slova
    RSAND, RSBREAK, RSELSEIF, RSFOR, RSIN, RSNOT, RSOR,
    RSREPEAT, RSUNTIL,
} TOKENY;

// struktura tokenu
typedef struct stTToken {
    int typ;                //typ tokenu
    char data[MAX_ARR];     //data tokenu
    int radek;              //radek v souboru
} TToken, *UkTToken;

// FUNKCE:
int ziskej_dalsi_token(FILE *f, UkTToken strukt);
//int test_klicovych_slov(char *retezec);