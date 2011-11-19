//interpret.h

//DEFINICE ------------------------------------------------

#define KONEC_OK    0
#define KONEC_CHYBA 1
#define TRUE        0
#define FALSE       1

//VYCTOVE TYPY --------------------------------------------

// typy instrukci
typedef enum nazvyInstrukci {
    IN_READ,     // read        | op     | /   | cil  0
    IN_WRITE,    // write       | op     | /   | /    1
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
    IN_PGOTO,    // podminene   | op     |nvsti| / 
    IN_NVSTI,    // navesti     | navesti| /   | /   15
    //---------------------------------------------
    IN_TYPE,     // type()      | op     | /   | cil 16
    IN_FIND,     // find()      | ret    | hl  | cil 17  hl - hledany podretezec
    IN_SORT,     // heapsort()  | op     | /   | cil 18
    IN_SUBSTR,   // substr()    |                    19
    //---------------------------------------------
    IN_PRIRAD,   // prirazeni   | op     | /   | cil 20
    //---------------------------------------------
    IN_KONEC,    // konec       |                    21
} TNazInstr;

//STRUKTURY -----------------------------------------------

//struktura pro jednotlive instrukce
//ukazatel na void se pozdeji pretipuje
typedef struct {
  int typInstr;              // typ instrukce
  void *op1;                 // adresa 1
  void *op2;                 // adresa 2
  void *op3;                 // adresa 3
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
  struct plzkaSezPar *posledni;  // aktivni polozka
} TSezPar, *UkTSezPar;



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

// funkce na seznamem/zasobnikem parametru funkci
void Sez_init_funkce(UkTSezPar L);
void Sez_zrus_funkce(UkTSezPar L);
int insert_last(UkTSezPar L, char *ret);
void set_first(UkTSezPar L);
void set_nasl(UkTSezPar L);
void *hodnota_aktivniho(UkTSezPar L);
int zmen_data_par(UkTSezPar L, char *ret, int typ);
int najdi_prvek(UkTSezPar L, char *K);
