/*
 *   Nazev projektu: Implementace interpretu imperativniho jazyka IFJ11
 * 
 *   Autori:   	Boris Valo, xvalob00
 * 				Pavel Slaby, xslaby00
 * 				Ondrej Vohanka, xvohan00
 * 				Matej Stepanek, xstepa43
 * 				Martina Stodolova, xstodo04 
 * 
 *   Datum odevzdani: 11.12.2011
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


// Řazení pole metodou heapsort
void siftdown(char *ret, int left, int right){
  int i, j;
  bool cont = false ;
  int temp;
  
  
  i = left;
  j = 2*i;
  temp = ret[i-1];
  cont = (j <= right);
  
  while(cont){
	if(j < right){
	  if(ret[j-1] < ret[j]){
		j++;  
	  }	
	}
	
	if(temp >= ret[j-1]){
	  cont = false;	
	}else{
	  ret[i-1] = ret[j-1];
	  i = j;
	  j = 2*i;
	  cont = (j<=right);	
	}  
  }
  
  ret[i-1] = temp;	
}

void heapsort(char *ret){
  int i;
  char pom;
  int left, right;
	int length=strlen(ret);
  left = length/2;
  right = length;
  
  for(i = left;i >= 1; i--){
	siftdown(ret, i, right);  
  }
  
  for(right = length; right >= 2; right--){
	pom = ret[0];
	ret[0] = ret[right-1];
	ret[right-1] = pom;
	siftdown(ret, 1, right-1);  
  }
}

// Knuth-Moris-Prattův algoritmus pro vyhledávání podřetězce v řetězci
void fail(char *vzorek, int dv, int *p){
    int k, r;
    k = 0;
    r = -1;
    p[0] = -1;

    while (k < dv) {
        while ((r > -1) &&  (vzorek[k] != vzorek[r])){
            r = p[r];
 }
        k++;
        r++;

        if (vzorek[k] == vzorek[r]){
  p[k] = p[r];
 }else{
  p[k] = r;
        }
    }
}

int KMP_hledani(char *ret, int delka_ret, char *vzorek, int delka_vz){
 int i, j;
 int *pole;
 i = 0;
 j = 0;
 
 pole = malloc((delka_ret+delka_vz)*sizeof(int));
 fail(vzorek, delka_vz, pole);
 
 while (j < delka_vz) {
         while ((i > -1) && (vzorek[i] != ret[j])){
              i = pole[i];
  }
         i++;
         j++;

         if (i >= delka_ret) {      
             free(pole);
                 return j-i + 1;

         }
     }
     free(pole);
     return -1;

}
