#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#define ERR_OK 0
#define ERR_LEX 1
#define ERR_SYNTAX 2
#define ERR_SEMANT 3
#define ERR_INTERPRET 4
#define ERR_INTERNI 5
#define ERR_PRAZDNY_VYRAZ 6


void kontrola_identifikatoru();
int ll_funkce ();
int ll_parametr();
int ll_dalsi_parametr();
int ll_deklarace();
int ll_inicializace();
int ll_prikazy();
int ll_prikaz_s_id();
int ll_prikaz_s_id_a_rovnase();
int ll_volani_prvni_parametr();
int ll_volani_dalsi_parametr();
int syntakticky_analyzator();
int syntax_vyrazu();
#endif
