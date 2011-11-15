//potrebne definice
#define MAX_ARR     20
#define TRUE         0
#define FALSE        1
#define KONEC_OK     0
#define KONEC_CHYBA  1

//samotna data
typedef union {
    double   dataCis;     //datovy typ number
    char    *dataRet;     //datovy typ string
    int      dataBool;    //datovy typ boolean
} TBSData;

//vyctovy typ datovych typu
typedef enum enTTDat {
    TDCISLO,              //cislo
    TDRETEZEC,            //retezec
    TDBOOL,               //boolean
    TDNIL,                //nil
} TTDat;

//struktura dat
typedef struct bsdata {
    TTDat   typ;          //typ dat
    TBSData data;         //union s daty
} TBSPolozka, *UkTBSPolozka;

//struktura uzlu
typedef struct bsuzel {
	char           *klic; //retezec slouzici jako klic
	TBSPolozka      data; //ukazatel na strukturu dat
    
	struct bsuzel   *luk; //ukazatel na leveho potomka
	struct bsuzel   *puk; //ukazatel na praveho potomka
} TBSUzel, *UkTBSUzel;

//FUNKCE
void BVSInit (UkTBSUzel *Kor);
int  BVSNajdi (UkTBSUzel Kor, char *K, UkTBSPolozka obsah);
void BVSVloz (UkTBSUzel* Kor, char *K, UkTBSPolozka obsah);
void BVSNahradZaPraveho(UkTBSUzel PtrReplaced, UkTBSUzel *Kor);
void BVSVymaz (UkTBSUzel *Kor, char *K);
void BVSZrus (UkTBSUzel *Kor);
