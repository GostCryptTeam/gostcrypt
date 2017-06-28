#undef DEBUG
#include "include.h"

#define POLY1 0xEF75L
#define POLY2 0x58C2FL
#define POLY3 0x1E7B4EED1DFL

#define MASK1 0x1FFFFL
#define MASK2 0x7FFFFL
#define MASK3 0x3FFFFFFFFFFL

mot64  motpar( mot64 w)
 {
  w ^= (w >> 32);
  w ^= (w >> 16);
  w ^= (w >>  8);
  w ^= (w >>  4);
  w ^= (w >>  2);
  w ^= (w >>  1);
  
  return(w & 01L);
 }

/*************************************************************/
/* Systeme de chiffrement par flot scex41. Ce systeme est un */
/* exemple pour etudier l'attaque de Siegenthaler.           */
/* La clef fait 77 bits (10 octets)                          */
/*                                                           */
/* Usage scex d|e infile outfile "clef"                      */
/*                                                           */
/* CBA FILIOL Eric                                           */
/*************************************************************/
int main(int argc, char * argv[])
 {
  register mot64 reg1, reg2, reg3, reb; 
  mot64 i, j;
  FILE * fin, * fout;
  mot08 outblock, lettre, f[8] = {0,0,0,1,0,1,1,1}, x;
  
  /***************************************/
  /*     Ouverture des fichiers          */
  /***************************************/
  fin = fopen(argv[2],"r");
  fout = fopen(argv[3],"w");

  /***************************************/
  /* Mise a la clef des registres        */
  /***************************************/
  reg1 = (mot64)(argv[4][0]) | ((mot64)(argv[4][1]) << 8) | ((mot64)(argv[4][2]) << 16);
  reg1 &= MASK1;

  reg2 = (mot64)(argv[4][2]) | ((mot64)(argv[4][3]) << 8) | ((mot64)(argv[4][4]) << 16);
  reg2 >>= 1;
  reg2 &= MASK2; 

  reg3 = (mot64)(argv[4][4]) | ((mot64)(argv[4][5]) << 8) | ((mot64)(argv[4][6]) << 16) | ((mot64)(argv[4][7]) << 24) | ((mot64)(argv[4][8]) << 32) | ((mot64)(argv[4][9]) << 40);
  reg3 >>= 4;
  reg3 &= MASK3;

  printf("Crypto %s - Etat initiaux des registres 1 et 2 : %LX %LX %LX\n", argv[3], reg1, reg2, reg3);

  /***************************************/
  /* Generation de la suite chiffante    */
  /***************************************/
  j = 0L;
  while(fscanf(fin,(argv[1][0] == 'e')?"%c":"%02X",&lettre), !feof(fin))
   {
    j++;
    outblock = 0;
    for(i = 0;i < 8;i++)
     {
      x = f[(reg1 & 1) | ((reg2 & 1) << 1) | ((reg3 & 1) << 2)];
      outblock |= (x << i);
      
      reb = motpar(reg1 & POLY1);
      reg1 >>=1;
      reg1 |= reb?0x10000L:0L;
      /* Autre solution reg1 |= (reb << 16); */
      
      reb = motpar(reg2 & POLY2);
      reg2 >>=1;
      reg2 |= reb?0x40000L:0L;
      /* Autre solution reg2 |= (reb << 18); */

      reb = motpar(reg3 & POLY3);
      reg3 >>=1;
      reg3 |= reb?0x20000000000L:0L;
      /* Autre solution reg3 |= (reb << 40); */
     }
    if(argv[1][0] == 'e') fprintf(fout,"%02X",lettre^outblock);
    else fprintf(fout,"%c",lettre^outblock);
   }
  printf("\n");

  fclose(fin);
  fclose(fout);
 }
