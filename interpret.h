//interpret.h

//DEFINICE ------------------------------------------------

#define KONEC_OK    0
#define KONEC_CHYBA 1
#define TRUE        0
#define FALSE       1

//VYCTOVE TYPY --------------------------------------------

// typy instrukci
typedef enum nazvyInstrukci {
    IN_READ,     // read        | /      | /   | cil  0
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
    IN_NVSTI,    // navesti     | navesti| /   | /   15
    //---------------------------------------------
    IN_TYPE,     // type()      |                    16
    IN_FIND,     // find()      | op     | op  | cil 17
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
typedef struct {
  struct plzkaSez *prvni;    // prvni polozka
  struct plzkaSez *posledni; // posledni polozka
  struct plzkaSez *aktivni;  // aktivni polozka
} TSezInstr, *UkTSezInstr;

//FUNKCE --------------------------------------------------

//funkce seznamu
void Sez_init(UkTSezInstr L);
void Sez_zrus(UkTSezInstr L);
void Sez_vloz(UkTSezInstr L, UkTInstr instr);
void Sez_prvni(UkTSezInstr L);
void *Sez_hodnota_aktivniho(UkTSezInstr L);
void Sez_dalsi(UkTSezInstr L);
void Sez_nastav_aktivni(UkTSezInstr L, UkTPlzkaSez instrukce);

//funkce interpretu
int Vloz_instrukci(UkTSezInstr seznam, int typ, void *op1, void *op2, void *op3);
void Uvolni_instrukci(UkTSezInstr polozka);
int Interpret(UkTSezInstr list);
