#include "../lib/bt-5.0.0/inc/btree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../lib/bt-5.0.0/inc/bc.h"
#include "../lib/bt-5.0.0/inc/bt.h"
#include "../lib/bt-5.0.0/inc/btree_int.h"
/* for ASCII only */
static char code[128] = { 0 };
void addcode(const char *s, int c)
{
	while (*s) 
    {
		code[(int)*s] = code[0x20 ^ (int)*s] = c;
		s++;
	}
}
 
void init()
{
	static const char *cls[] = { "AEIOU", "", "BFPV", "CGJKQSXZ", "DT", "L", "MN", "R", 0};
	
    int i;
	
    for (i = 0; cls[i]; i++)
		addcode(cls[i], i - 1);
}
 
/* returns a static buffer; user must copy if want to save
   result across calls */
const char* soundex(const char *s)
{
	static char out[5];
	int c, prev, i;
 
	out[0] = out[4] = 0;
	if (!s || !*s) return out;
 
	out[0] = *s++;
 
	/* first letter, though not coded, can still affect next letter: Pfister */
	prev = code[(int)out[0]];
	
    for (i = 1; *s && i < 4; s++) {
		if ((c = code[(int)*s]) == prev) continue;
 
		if (c == -1) prev = 0;	/* vowel as separator */
		else if (c > 0) 
        {
			out[i++] = c + '0';
			prev = c;
		}
	}
	while (i < 4) out[i++] = '0';
	
    return out;
}

int main(){
    FILE *f1 = fopen("readFile/anhviet.txt", "r");
    BTA *dt;
    
    dt = btcrt("words.dat", 0, FALSE);

    
    btdups(dt, FALSE);

    char tmp[500];
    char anh[500], viet[9500];
    char sd_tmp[10];
    int count;
    
    init();
    
    for(int i=0;i<3;i++)
        fgetc(f1);
    
    while(!feof(f1))
    {
        fgets(tmp, 3000, f1);
        
        if(tmp[0] == '@')
        {
            if(strlen(viet) && strlen(anh))
            {
                if(viet[strlen(viet)-1] == '\n')
                    viet[strlen(viet)-1] = '\0';
                
                
                btins(dt, anh, viet, strlen(viet)+1);
                
                anh[0] = '\0';
                viet[0] = '\0';
            }
            
            if(tmp[strlen(tmp)-1] == '\n')
                tmp[strlen(tmp)-1] = '\0';
            
            strcpy(anh, tmp + 1);
            
            for(int i=0; i < strlen(anh); i++)
                if(anh[i] == '/'){
                    strcat(viet, anh + i);
                    strcat(viet, "\n");
                    
                    anh[i-1] = '\0';
                }
        }
        else{
            if(tmp[0] != '\n')
            {
                strcat(viet, tmp);
            }
        }
    }
    
    viet[strlen(viet)-1] = '\0';
    
   
    btins(dt, anh, viet, strlen(viet)+1);
    
    anh[0] = '\0';
    viet[0] = '\0';
    
    printf("Created words.data\n");
   
    
    fclose(f1);

    btcls(dt);

    
    return 0;
}
