#include <stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include "tools.h"

typedef uint16_t u16;
typedef uint8_t u8;

//#define PRINT

#define NBROUND 32

#define KSIZE 80

static const u8 S0[16] = {14, 9, 15, 0, 13, 4, 10, 11, 1, 2, 8, 3, 7, 6, 12, 5};
static const u8 S1[16] = {4, 11, 14, 9, 15, 13, 0, 10, 7, 12, 5, 6, 2, 8, 1, 3};
static const u8 S2[16] = {1, 14, 7, 12, 15, 13, 0, 6, 11, 5, 9, 3, 2, 4, 8, 10};
static const u8 S3[16] = {7, 6, 8, 11, 0, 15, 3, 14, 9, 10, 12, 13, 5, 2, 4, 1};
static const u8 S4[16] = {14, 5, 15, 0, 7, 2, 12, 13, 1, 8, 4, 9, 11, 10, 6, 3};
static const u8 S5[16] = {2, 13, 11, 12, 15, 14, 0, 9, 7, 10, 6, 3, 1, 8, 4, 5};
static const u8 S6[16] = {11, 9, 4, 14, 0, 15, 10, 13, 6, 12, 5, 7, 3, 8, 1, 2};
static const u8 S7[16] = {13, 10, 15, 0, 14, 4, 9, 11, 2, 1, 8, 3, 7, 5, 12, 6};
static const u8 S8[16] = {8, 7, 14, 5, 15, 13, 0, 6, 11, 12, 9, 10, 2, 4, 1, 3};
static const u8 S9[16] = {11, 5, 15, 0, 7, 2, 9, 13, 4, 8, 1, 12, 14, 10, 3, 6};


 void EncryptKeySchedule(u8 key[10], u8 output[NBROUND][4])
{
     u8 i, KeyR[4];
     
     output[0][3] = key[9];
     output[0][2] = key[8];
     output[0][1] = key[7];
     output[0][0] = key[6];
     
     for(i=1;i<32;i++)
     {
     // K <<< 29                 
     KeyR[3]=key[9];
     KeyR[2]=key[8];
     KeyR[1]=key[7];     
     KeyR[0]=key[6];     
     
     key[9]=(((key[6] & 0x07)<<5)&0xE0) ^ (((key[5]& 0xF8)>>3) & 0x1F);
     key[8]=(((key[5] & 0x07)<<5)&0xE0) ^ (((key[4]& 0xF8)>>3) & 0x1F);
     key[7]=(((key[4] & 0x07)<<5)&0xE0) ^ (((key[3]& 0xF8)>>3) & 0x1F);
     key[6]=(((key[3] & 0x07)<<5)&0xE0) ^ (((key[2]& 0xF8)>>3) & 0x1F);
     key[5]=(((key[2] & 0x07)<<5)&0xE0) ^ (((key[1]& 0xF8)>>3) & 0x1F);
     key[4]=(((key[1] & 0x07)<<5)&0xE0) ^ (((key[0]& 0xF8)>>3) & 0x1F);
     key[3]=(((key[0] & 0x07)<<5)&0xE0) ^ (((KeyR[3]& 0xF8)>>3) & 0x1F);
     key[2]=(((KeyR[3] & 0x07)<<5)&0xE0) ^ (((KeyR[2]& 0xF8)>>3) & 0x1F);
     key[1]=(((KeyR[2] & 0x07)<<5)&0xE0) ^ (((KeyR[1]& 0xF8)>>3) & 0x1F);
     key[0]=(((KeyR[1] & 0x07)<<5)&0xE0) ^ (((KeyR[0]& 0xF8)>>3) & 0x1F);         
                    
     // reste du keyschedule                 
     key[9]=(S9[((key[9]>>4) & 0x0F)]<<4) ^ S8[(key[9]& 0x0F)];
     
     key[6]=key[6] ^ ((i>>2) & 0x07);
     key[5]=key[5] ^ ((i & 0x03)<<6);
        
     output[i][3] = key[9];
     output[i][2] = key[8];
     output[i][1] = key[7];
     output[i][0] = key[6];                      
     }                          
}


void Swap(u8 block[8])
{
    u8 tmp[4];

    tmp[0] = block[0];
    tmp[1] = block[1];
    tmp[2] = block[2];
    tmp[3] = block[3];

    block[0] = block[4];
    block[1] = block[5];
    block[2] = block[6];
    block[3] = block[7];

    block[4] = tmp[0];
    block[5] = tmp[1];
    block[6] = tmp[2];
    block[7] = tmp[3];
}

 void OneRound(u8 x[8], u8 k[4])
{
	u8 t[4],tmp[4];

	// AJOUT CLE
    tmp[0]=x[4]^k[0];
    tmp[1]=x[5]^k[1];
    tmp[2]=x[6]^k[2];
    tmp[3]=x[7]^k[3];         

    // PASSAGE DANS LES BOITES S
    tmp[0] = ((S1[((tmp[0])>>4) & 0x0F])<<4)^S0[(tmp[0] & 0x0F)];
    tmp[1] = ((S3[((tmp[1])>>4) & 0x0F])<<4)^S2[(tmp[1] & 0x0F)];
    tmp[2] = ((S5[((tmp[2])>>4) & 0x0F])<<4)^S4[(tmp[2] & 0x0F)];
    tmp[3] = ((S7[((tmp[3])>>4) & 0x0F])<<4)^S6[(tmp[3] & 0x0F)];          
    
    // PASSAGE DE LA PERMUTATION P
	t[0] =((tmp[0]>>4) & 0x0F)^(tmp[1] & 0xF0);
	t[1] = (tmp[0] & 0x0F) ^ ((tmp[1]& 0x0F)<<4);
	t[2] = ((tmp[2]>>4) & 0x0F)^(tmp[3] & 0xF0);
	t[3] = (tmp[2] & 0x0F) ^ ((tmp[3]& 0x0F)<<4);
    // FIN DE LA FONCTION F

    // PARTIE GAUCHE AVEC DECALAGE DE 8 SUR LA GAUCHE  
    tmp[0]=x[3]^t[0]; 
    tmp[1]=x[0]^t[1]; 
    tmp[2]=x[1]^t[2]; 
    tmp[3]=x[2]^t[3]; 
    
	// PARTIE DROITE
    x[0]=tmp[0];
    x[1]=tmp[1];
    x[2]=tmp[2];
    x[3]=tmp[3];      
    
  
}

 void Encrypt(u8 x[8], u8 subkey[NBROUND][4])
{
     int i;
     
     for(i=0; i<31; i++)
     {
        OneRound(x, subkey[i]);        
        Swap(x);
     }
     OneRound(x, subkey[i]);
}

 void OneRound_Inv(u8 y[8], u8 k[4])
{
     u8 t[4],tmp[4];

	 // FAIRE PASSER Y_0, Y_1, Y_2, Y_3 dans F
	// AJOUT CLE
    tmp[0]=y[4]^k[0]; 
    tmp[1]=y[5]^k[1]; 
    tmp[2]=y[6]^k[2]; 
    tmp[3]=y[7]^k[3];  
     
 
     // PASSAGE DANS LES BOITES S
    tmp[0] = ((S1[((tmp[0])>>4) & 0x0F])<<4)^S0[(tmp[0] & 0x0F)];
    tmp[1] = ((S3[((tmp[1])>>4) & 0x0F])<<4)^S2[(tmp[1] & 0x0F)];
    tmp[2] = ((S5[((tmp[2])>>4) & 0x0F])<<4)^S4[(tmp[2] & 0x0F)];
    tmp[3] = ((S7[((tmp[3])>>4) & 0x0F])<<4)^S6[(tmp[3] & 0x0F)];    
 
   // PASSAGE DE LA PERMUTATION P
	t[0] =((tmp[0]>>4) & 0x0F)^(tmp[1] & 0xF0);
	t[1] = (tmp[0] & 0x0F) ^ ((tmp[1]& 0x0F)<<4);
	t[2] = ((tmp[2]>>4) & 0x0F)^(tmp[3] & 0xF0);
	t[3] = (tmp[2] & 0x0F) ^ ((tmp[3]& 0x0F)<<4);
    // FIN DE LA FONCTION F
    
        // PARTIE DROITE AVEC DECALAGE DE 8 SUR LA DROITE
	tmp[0]= y[0]^t[0]; 
    tmp[1]= y[1]^t[1]; 
    tmp[2]= y[2]^t[2]; 
    tmp[3]= y[3]^t[3]; 
 
 	// PARTIE GAUCHE
    y[0]=tmp[1];
    y[1]=tmp[2];
    y[2]=tmp[3];
    y[3]=tmp[0];
 
}

 void Decrypt(u8 x[8], u8 subkey[NBROUND][4])
{
     int i;

     OneRound_Inv(x, subkey[31]);
     for(i=30; i>=0; i--)
     {
        Swap(x);
        OneRound_Inv(x, subkey[i]);   
     }
}


int main()
{
#ifdef PRINT
uart1_init();
#endif
	u8 mkey[10];
	u8 rkey[NBROUND][4];

     mkey[0] = 0xdc;
    mkey[1] = 0xfe; 
    mkey[2] = 0xef;
    mkey[3] = 0xcd;
    mkey[4] = 0xab;
    mkey[5] = 0x89;
    mkey[6] = 0x67; 
    mkey[7] = 0x45;
    mkey[8] = 0x23; 
    mkey[9] = 0x01;      
    
   u8 state[8]={0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01}; 
/*
for(i=0;i<8;i++)
{
   state[i]=0;
   mkey[i]=0;
}
   mkey[8] = 0; 
   mkey[9] = 0;  
*/
#ifdef PRINT
printf("----------Clair----------\n\n");
printf("%02X %02X %02X %02X %02X %02X %02X %02X",state[7], state[6], state[5], state[4], state[3], state[2], state[1], state[0]);
printf("\n");
printf("----------Chiffr\'e----------\n\n");
#endif		
 
 START_ENCRYPT();
    
 EncryptKeySchedule(mkey,rkey);
 Encrypt(state,rkey);
 
 #ifdef PRINT
 printf("%02X %02X %02X %02X %02X %02X %02X %02X",state[7], state[6], state[5], state[4], state[3], state[2], state[1], state[0]);
 printf("\n");
 printf("----------Déchiffr\'e----------\n\n");
 #endif

 START_DECRYPT();
 Decrypt(state,rkey);

 #ifdef PRINT
 printf("%02X %02X %02X %02X %02X %02X %02X %02X",state[7], state[6], state[5], state[4], state[3], state[2], state[1], state[0]);
 printf("\n");
 printf("----------Déchiffr\'e----------\n\n");
 #endif

 END_EXPE();

return 0;
}
