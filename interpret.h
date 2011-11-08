//interpret.h

//DEFINICE ------------------------------------------------

#define KONEC_OK    0
#define KONEC_CHYBA 1
#define TRUE        0
#define FALSE       1

//VYCTOVE TYPY --------------------------------------------

// typy instrukci
typedef enum nazvyInstrukci {
    IN_NOOP,             // zadna operace
    IN_READ,             // read            -       -       out
    IN_WRITE,            // write           in      -       -
    IN_ADD,              // +               in      in      out
    IN_SUB,              // -               in      in      out
    IN_MUL,              // *               in      in      out
    IN_DIV,              // /               in      in      out
    IN_GOTO,             // goto            label   -       -
    IN_LABEL,            // label           label
    IN_MENSI,            // <               in      in      out
    IN_MENROV,           // <=              in      in      out
    IN_VETSI,            // >               in      in      out
    IN_VETROV,           // >=              in      in      out
    IN_ROVNO,            // ==              in      in      out
    IN_KONK,             // ..              in      in      out
    IN_KONEC,            //konec interpretu

    IN_TYPE,             // type()
    IN_FIND,             // find()          in      in      out
    IN_SORT,             // sort()          in      -       out
    IN_SUBSTR,           // substr()
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
void Sez_nastav_aktivni(UkTSezInstr L, void *instrukce);

//funkce interpretu
void Vloz_instrukci(UkTSezInstr seznam, int typ, void *op1, void *op2, void *op3);
