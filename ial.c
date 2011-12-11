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
	
	p[0] = 0;
	
	for(k = 1; k < dv; k++){
	  r = p[k-1];	
	  while((r>0) && (p[r]!=p[k-1])){
		r = p[r];
	  }
	  p[k] = r+1;
	}
}

int KMP_hledani(char *ret, int delka_ret, char *vzorek, int delka_vz){
	int i, j;// s;
	int *pole;
	i = 0;
	j = 0;
	
	pole = malloc((delka_ret+delka_vz)*sizeof(int));
	fail(vzorek, delka_vz, pole);
	/*
	for(s = 0; s < (delka_ret + delka_vz); s++){
	  printf("%d \n", pole[s]);	
	} 
	*/
	while((i <= delka_ret) && (j <= delka_vz)){
	  if((j == 0) || (ret[i] == vzorek[j])){
			i++;
			j++;  
	  }else{
			j = pole[j];
	  }
	}
	
	free(pole);
	if(j <= delka_vz){
	  return i-delka_vz;	
	}else{
	  return -1;	
	}
	

}


