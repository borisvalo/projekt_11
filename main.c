#include <stdio.h>
#include "main.h"
/*
 * Autor: Pavel Slabý, xslaby00
 * 				a další
 * Datum odevzdání: 11.12.2011
 */
 
#define ERR_OK 0
#define ERR_LEX 1
#define ERR_SYNTAX 2
#define ERR_SEMANT 3
#define ERR_INTERPRET 4
#define ERR_INTERNI 5


int main(int argc, char** argv)){
	FILE *soubor;
	if (argc == 1)
	{
		printf("Nezadan argument\n");
		return FILE_ERROR;
	}
	if ((f = fopen(argv[1], "r")) == NULL)
	{
		printf("Nepodarilo se otevrit soubor\n");
		return FILE_ERROR;
	}
	
	tTabulkaSymbolu TabulkaSymbolu;
	tabInit(&TabulkaSymbolu);
	
	tVnitrni vnitrni;//TODO:
	
	kodChyby = syntakticky_analyzator(&TabulkaSymbolu, &vnitrni);
	
	
	
	//(TODO: )odalokování zdrojů
	fclose(soubor);
	tableDispose(&TabulkaSymbolu);
	
	if(kodChyby!=ERR_OK){
		//TODO: odalokuj vnitrni
		return kodChyby;
	}
	
	interpretuj(&vnitrni);
	
	return ERR_OK;
}
