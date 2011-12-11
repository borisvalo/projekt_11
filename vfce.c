#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "interpret.h"
#include "str.h"

#define DELKA_PROMENNE 8

char *type(UkTBSPolozka muj_odkaz){
	if(muj_odkaz->typ==TDCISLO){
		return "number";
	}
	else if(muj_odkaz->typ==TDRETEZEC){
		return "string";
	}	
	else if(muj_odkaz->typ==TDBOOL){
		return "boolean";
	}
	
    return NULL;
}

void write(UkTBSPolozka muj_odkaz){
	if(muj_odkaz->typ==TDCISLO){
		printf("%g\n", muj_odkaz->data.dataCis);	
	}
	else if(muj_odkaz->typ==TDRETEZEC){
		printf("%s\n", muj_odkaz->data.dataRet);
	}
	else{
		//chyba
	}
}

void substr(UkTBSPolozka muj_odkaz, char **string, int cislo1, int cislo2){
	int delka_promenne=DELKA_PROMENNE;
	Ret_alokuj(string, delka_promenne);
    
    if(muj_odkaz->typ==TDRETEZEC){ 
		
        
        if( (cislo2 == 0) || ((cislo1) > (int)strlen(muj_odkaz->data.dataRet)) || (cislo2<cislo1) ){
            (*string)[0]='\0';
            return;
        }
        if(cislo2<0){
            //printf("%d\n", strlen(muj_odkaz->data.dataRet));
            cislo2 = (((int)strlen(muj_odkaz->data.dataRet))+cislo2)+1;
            //printf("%d\n",cislo2);
			
        }
        
        int i=cislo1-1;
        int j = 0;			
        while(i<cislo2){
            
            if(i>=0){
                if(j+1 > delka_promenne){
                    delka_promenne *=2;
                    Ret_realokuj(string,delka_promenne);	
                }
				
                (*string)[j]=(muj_odkaz->data.dataRet)[i];
                j++;
            }
            i++;
        }
        
        //printf("%s\n", (*string));
    }
}

void read(UkTBSPolozka muj_odkaz, UkTBSPolozka ukazatel){
	float pom; 
	int delka_promenne=DELKA_PROMENNE;
	int znak;
	int citac=0;
	printf("instrukce read %d\n", muj_odkaz->typ);
	if (ukazatel == NULL){
		printf("neujkloadam, vystup\n");
		scanf("%*s");
		return;
	}
	Ret_alokuj(&ukazatel->data.dataRet, delka_promenne);
	
	printf("read: alokovano\n");
    //jedna-li se o KLADNE CISLO - nacte dany pocet znaku - navratovy typ string
	if(muj_odkaz->typ==TDCISLO){ 
        
		znak=getchar();
		//testujeme jestli je to mensi nez pocet nactenych znaku
		while(znak !='\n' && znak !=EOF && citac<muj_odkaz->data.dataCis){
			if(citac+1 > delka_promenne){
			 	delka_promenne *=2;
                Ret_realokuj(&ukazatel->data.dataRet,delka_promenne);	
			}
			ukazatel->data.dataRet[citac] = znak;
			citac++;
            znak=getchar();
		}
		if(citac+1 > delka_promenne){
			delka_promenne *=2;
			Ret_realokuj(&ukazatel->data.dataRet,delka_promenne);	
		}
		ukazatel->data.dataRet[citac] = '\0';
		
	}
    // jedna-li se o retezec a zaroven o format "*n" - nacita cislo s navratovym typek number
	else if(muj_odkaz->typ==TDRETEZEC){
        
		if(strcmp(muj_odkaz->data.dataRet,"*n")==0){
			
			if(scanf("%f\n",&pom)==0){
				//return ;
			}
			ukazatel->data.dataCis =  pom;
			//printf("%f\n",ukazatel->dataCis);
            return;
		}	
        // jedna-li se o retezec a zaroven o format "*l" - nacita znaky po konec radku-navrat.typ string
		else if(strcmp(muj_odkaz->data.dataRet,"*l")==0){
			while((znak=getchar())!='\n' && znak !=EOF){		
				if(citac+1 > delka_promenne){
					delka_promenne *=2;
                    Ret_realokuj(&ukazatel->data.dataRet,delka_promenne);		
				}
				ukazatel->data.dataRet[citac] = znak;
				citac++;
                
			}
			if(citac+1 > delka_promenne){
                delka_promenne *=2;
                Ret_realokuj(&ukazatel->data.dataRet,delka_promenne);	
                
			}
			ukazatel->data.dataRet[citac] = '\0';
            
		}
        // jedna-li se o retezec a zaroven o format "*a"-nacita znaky po konec souboru-navrat.typ string
		else if(strcmp(muj_odkaz->data.dataRet,"*a")==0){
			while((znak=getchar())!=EOF){		
				if(citac+1 > delka_promenne){
					delka_promenne *=2;
					Ret_realokuj(&ukazatel->data.dataRet,delka_promenne);			
				}
				ukazatel->data.dataRet[citac] = znak;
				citac++;
			}
			if(citac+1 > delka_promenne){
                delka_promenne *=2;
                Ret_realokuj(&ukazatel->data.dataRet,delka_promenne);	
			}
			ukazatel->data.dataRet[citac] = '\0';
            
		}
		else{	
			//chyba
			printf("chyba1\n");
		}
	}
	else{
		//chyba
			printf("chyba2\n");
	}
    
    return;
}

