//string.h

//typ string - retezec
typedef char *TRetezec;

//FUNKCE
void Ret_alokuj(TRetezec *ret, int pocet);
void Ret_vloz(TRetezec *ret, TRetezec novy);
void Ret_uvolni(TRetezec ret);
void Ret_realokuj(TRetezec *ret, int delka);
void Ret_konkatenace(TRetezec ret1, TRetezec ret2, TRetezec *ret3);