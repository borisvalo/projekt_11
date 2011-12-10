// scaner.h

//pomocne definice:
#define MIN_ARR     10
#define NAS_DEL     2
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
    POROVNANI,     // ==
    MOCNINA,       // ^
    ROVNASEKONEC,  // = //15
    VETSIROVNO,    // >=
    VETSITKOKONEC, // >
    MENSIROVNO,    // <=
    MENSITKOKONEC, // <
    NEROVNASE,     // ~=
    IDKONEC, // identifikator
		MINUSKONEC, // -
		MYIDKONEC, // pomocny identifikator pro Martinu
		ENDOFFILE, // EOF //24
    
    //klicova slova
    TNDO, TNELSE, TNEND, TNFALSE, TNFUNCTION, TNIF, TNLOCAL,
    TNNIL, TNREAD, TNRETURN, TNTHEN, TNTRUE, TNWHILE, TNWRITE,
    
    //rezervovana slova
    RSAND, RSBREAK, RSELSEIF, RSFOR, RSIN, RSNOT, RSOR,
    RSREPEAT, RSUNTIL, RSSORT, RSFIND, RSTYPE, RSSUBSTR,RSMAIN,
} TOKENY;

// struktura tokenu
typedef struct stTToken {
    int   typ;     //typ tokenu
    char *data;    //data tokenu
    int   delka;   //delka retezce
} TToken, *UkTToken;

// FUNKCE:
int token_alokuj(UkTToken *strukt);
void token_uvolni(UkTToken strukt);
int ziskej_dalsi_token(FILE *f, UkTToken strukt);
