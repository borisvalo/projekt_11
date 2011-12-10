/*
 * Soubor obsahující funkce do předmětu IAL
 * Autor: Pavel Slabý, xslaby00
 * 				a další
 * Datum odevzdání: 11.12.2011
 */


typedef TPolozka {
	int hodnota;
	TPolozka *nasledujici;
}TPolozka

typedef TSeznam {
	TPolozka *prvni;
}

void siftdown(char *ret, int left, int right);
void heapsort(char *ret);
int KMP_hledani(char *ret, int delka_ret, char *vzorek, int delka_vz);
void fail(char *vzorek, int dv, int *p)
