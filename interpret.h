//interpret.h

//DEFINICE ------------------------------------------------

#define KONEC_OK    0
#define KONEC_CHYBA 1
#define TRUE        0
#define FALSE       1
#define MAX_ARR     20

//---------------- struktury pro BVS funkci ------------------
typedef struct bsfunkpol {
int pocet_param; // promenna pro ulozeni poctu parametru funkce
struct bsuzel *koren; // koren tabulky symbolu dane fce
struct sezPar *zasobnik;
struct plzkaSez *adresa; // inception
} TBSFunkPol, *UkTBSFunkPol;

typedef struct bsfunkce {
char *klic; //retezec slouzici jako klic - nazev funkce
struct bsfunkpol *data; //ukazatel na strukturu dat

struct bsfunkce *luk; //ukazatel na leveho potomka
struct bsfunkce *puk; //ukazatel na praveho potomka
} TBSFunkce, *UkTBSFunkce;



//samotna data
typedef union unidata {
    double   dataCis;     //datovy typ number
    char    *dataRet;     //datovy typ string
    int      dataBool;    //datovy typ boolean
} TBSData;

//vyctovy typ datovych typu
typedef enum enTTDat {
    TDCISLO,              //cislo
    TDRETEZEC,            //retezec
    TDBOOL,               //boolean //2
    TDNIL,                //nil
} TTDat;

//struktura dat
typedef struct bsdata {
    int typ;          //typ dat
    TBSData data;         //union s daty
} TBSPolozka, *UkTBSPolozka;

//struktura uzlu
typedef struct bsuzel {
	char           *klic; //retezec slouzici jako klic
	struct bsdata   data; //ukazatel na strukturu dat
    
	struct bsuzel   *luk; //ukazatel na leveho potomka
	struct bsuzel   *puk; //ukazatel na praveho potomka
} TBSUzel, *UkTBSUzel;

//FUNKCE
void BVSInit (UkTBSUzel *Kor);
int  BVSNajdi (UkTBSUzel Kor, char *K, UkTBSPolozka *obsah);
void BVSVloz (UkTBSUzel* Kor, char *K, UkTBSPolozka obsah);
void BVSNahradZaPraveho(UkTBSUzel PtrReplaced, UkTBSUzel *Kor);
void BVSVymaz (UkTBSUzel *Kor, char *K);
void BVSZrus (UkTBSUzel *Kor);
void BVSVypisStrom (UkTBSUzel *Kor);


//dalsi by Paulie
int Pole_realokuj(UkTBSUzel *ret, int delka);
void Pole_uvolni(UkTBSUzel ret);
int Pole_alokuj(UkTBSUzel *ret, int pocet);

void BVSFunkceInit (UkTBSFunkce *Kor);
int BVSFunkceNajdi (UkTBSFunkce Kor, char *K, UkTBSFunkPol *obsah);
void BVSFunkceVloz (UkTBSFunkce *Kor, char *K, UkTBSFunkPol obsah);
void BVSFunkceZrus (UkTBSFunkce *Kor);











// typy instrukci
typedef enum nazvyInstrukci {
    IN_READ = 0,     // read    | cil     | /   | op  0 //puvodni: read        | op     | /   | cil
    IN_WRITE,    // write       | /     | /   | op    1
    //---------------------------------------------
    IN_ADD,      // +           | op     | op  | cil  2
    IN_SUB,      // -           | op     | op  | cil  3
    IN_MUL,      // *           | op     | op  | cil  4
    IN_DIV,      // /           | op     | op  | cil  5
    IN_MOCN,     // mocnina     | op     | op  | cil  6
    //---------------------------------------------
    IN_MENSI,    // <           | op     | op  | cil  7
    IN_MENROV,   // <=          | op     | op  | cil  8
    IN_VETSI,    // >           | op     | op  | cil  9
    IN_VETROV,   // >=          | op     | op  | cil 10
    IN_ROVNO,    // ==          | op     | op  | cil 11
    IN_NEROVNO,  // ~=          | op     | op  | cil 12
    IN_KONK,     // ..          | op     | op  | cil 13
    //---------------------------------------------
    IN_GOTO,     // goto        | navesti| /   | /   14
    IN_PGOTO,    // podminene   | op     |nvsti| /   15
    IN_NVSTI,    // navesti     | navesti| /   | /   16
    //---------------------------------------------
    IN_TYPE,     // type()      | op     | /   | cil 17
    IN_FIND,     // find()      | ret    | hl  | cil 18  hl - hledany podretezec
    IN_SORT,     // heapsort()  | op     | /   | cil 19
    IN_SUBSTR_1,   // substr()    | ret | /    | /   20
    IN_SUBSTR_2,   // substr()    | cis | cis  | cil 21
    //---------------------------------------------
    IN_PRIRAD,   // prirazeni   | zas    | kam | co  22
    //---------------------------------------------
    IN_POP,      // POP adrr    | funkce | cil | /   23
    IN_PUSH,     // PUSH adrr   | funkce | adr | /   24
    IN_HLEDEJ,   // hledani     | kde    | co  | cil 25
    //---------------------------------------------
    IN_KONEC,    // konec       |                    26
} TNazInstr;

//STRUKTURY -----------------------------------------------

//struktura pro jednotlive instrukce
//ukazatel na void se pozdeji pretypuje
typedef struct {
  int typInstr;              // typ instrukce
  UkTBSPolozka op1;                 // adresa 1
  UkTBSPolozka op2;                 // adresa 2
  UkTBSPolozka op3;                 // adresa 3
} TInstr, *UkTInstr;

//struktura pro polozky seznamu
typedef struct plzkaSez {
  TInstr instrukce;          //ukazatel na strukturu instrukce
  struct plzkaSez *ukdalsi;  //ukazatel na dalsi prvek seznamu
} TPlzkaSez, *UkTPlzkaSez;

//struktura celeho seznamu
typedef struct seznamInstr{
  struct plzkaSez *prvni;    // prvni polozka
  struct plzkaSez *posledni; // posledni polozka
  struct plzkaSez *aktivni;  // aktivni polozka
} TSezInstr, *UkTSezInstr;

/* SEZNAM paramtrů fcí */

//struktura uzlu
typedef struct promennaPar {
	char           *klic; //retezec slouzici jako klic
	struct bsdata      *data; //ukazatel na strukturu dat
} TPromennaPar, *UkTPromennaPar;

//struktura pro polozky seznamu
typedef struct plzkaSezPar {
  struct promennaPar parametr;          //ukazatel na strukturu instrukce
  struct plzkaSezPar *ukdalsi;  //ukazatel na dalsi prvek seznamu
} TPlzkaSezPar, *UkTPlzkaSezPar;

//struktura celeho seznamu
typedef struct sezPar{
  struct plzkaSezPar *prvni;    // prvni polozka
  struct plzkaSezPar *aktivni;  // aktivni polozka
  struct plzkaSezPar *posledni;  // posledni polozka
} TSezPar, *UkTSezPar;

/* Zasobnik navratovych adres */

//struktura pro polozky seznamu
typedef struct plzkaZas {
  TInstr *adresa;          //ukazatel na strukturu instrukce
  struct plzkaZas *ukdalsi;  //ukazatel na dalsi prvek seznamu
} TPlzkaZas, *UkTPlzkaZas;

//struktura celeho seznamu
typedef struct zasAdr{
  struct plzkaZas *prvni;    // prvni polozka
} TZasAdr, *UkTZasAdr;

//FUNKCE --------------------------------------------------

//funkce seznamu
void Sez_init(UkTSezInstr L);
void Sez_zrus(UkTSezInstr L);
void Sez_vloz(UkTSezInstr L, UkTInstr instr);
void Sez_prvni(UkTSezInstr L);
void *Sez_hodnota_aktivniho(UkTSezInstr L);
void Sez_dalsi(UkTSezInstr L);
void Sez_nastav_aktivni(UkTSezInstr L, UkTPlzkaSez instrukce);
void *Sez_vrat_uk_posledni(UkTSezInstr L);

//funkce interpretu
int Vloz_instrukci(UkTSezInstr seznam, int typ, void *op1, void *op2, void *op3);
int Interpret(UkTSezInstr list);

// funkce na seznamem/zasobnikem parametru funkci
void Sez_init_funkce(UkTSezPar L);
void Sez_zrus_funkce(UkTSezPar L);
int insert_last(UkTSezPar L, char *ret);
void set_first(UkTSezPar L);
void set_nasl(UkTSezPar L);
void *hodnota_aktivniho(UkTSezPar L);
int zmen_data_par(UkTSezPar L, void *ret, int typ);
int najdi_prvek_lok(UkTSezPar L, char *K);
void kopiruj_parametry(UkTSezPar zas_zpracovani, UkTSezPar zasobnik);
void kopiruj_promenne(UkTSezPar zas_zpracovani, UkTBSUzel *UkKor);
int najdi_prom(UkTSezPar L, char *K, UkTBSPolozka *ukazatel);
void vymaz_promenne(UkTSezPar L);


// funkce pro praci se zasobnikem navratovych adres

void zas_adres_in(UkTZasAdr L);
void zas_adres_zrus(UkTZasAdr L);
int Push_adr(UkTZasAdr L, UkTInstr adresa);
void Pop_adr(UkTZasAdr L, UkTInstr adresa);



// --------------------------- z zasobnik.h --------------



#define REALLOC_SOUSTO 4 // TODO: zmenit na vetsi pred odevzdanim
                         // ZMENIT ZPATKY NA 2 - KVULI TESTOVANI ALOKACE

/*typedef struct stTPrvek {
	int typ;   // typ tokenu
	//UkTBSPolozka uk_na_prvek_ts;
	struct bsdata *uk_na_prvek_ts;
} TPrvek;*/

typedef struct stTZasobnikUk {
    int top; //vrchol zasobniku
    int velikost; //velikost alokovaneho prostoru
    UkTBSPolozka * array; //data zasobniku
} TZasobnikUk;

typedef struct stTZasobnikInt {
    int top; //vrchol zasobniku
    int velikost; //velikost alokovaneho prostoru
    int * array; //data zasobniku
} TZasobnikInt;


int zasobnik_init (TZasobnikUk *zas_uk, TZasobnikInt *zas_int);
void zasobnik_free(TZasobnikUk *zas_uk, TZasobnikInt *zas_int);
int zasobnik_pop (TZasobnikUk *zas_uk, TZasobnikInt *zas_int);
int zasobnik_push (TZasobnikUk *zas_uk, int hodnota, TZasobnikInt *zas_int, UkTBSPolozka ukazatel_do_ts);

//int zasobnik_pristup (TZasobnik *zas, TPrvek * hodn, int posun);
//int zasobnik_top (TZasobnik *zas, TPrvek * hodn);    // NEPOUZIVA SE
void zasobnik_vynuluj (TZasobnikUk *zas_uk, TZasobnikInt *zas_int);

int zasobnik_pristup_uk (TZasobnikUk *zas, UkTBSPolozka * hodn, int posun);
int zasobnik_pristup_int (TZasobnikInt *zas, int * hodn, int posun);
