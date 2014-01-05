//#define PRINT

#include <stdint.h>
#include "tools.h"

typedef uint16_t u16;
typedef uint8_t u8;

#define KSIZE 80
//#define KSIZE 128 


static const u8 S[16] = {0x0c, 0x00, 0x0f, 0x0a,
                    0x02, 0x0b, 0x09, 0x05,
                    0x08, 0x03, 0x0d, 0x07,
                    0x01, 0x0e, 0x06, 0x04};
                    
static const u8 Pi[16] = {0x05, 0x00, 0x01, 0x04,
                    0x07, 0x0c, 0x03, 0x08,
                    0x0d, 0x06, 0x09, 0x02,
                    0x0f, 0x0a, 0x0b, 0x0e};                    

static const u8 Pi_Inv[16] = {0x01, 0x02, 0x0b, 0x06,
                    0x03, 0x00, 0x09, 0x04,
                    0x07, 0x0a, 0x0d, 0x0e,
                    0x05, 0x08, 0x0f, 0x0c};

static const u8 CON[35] = {0x01, 0x02, 0x04, 0x08,
                    0x10, 0x20, 0x03, 0x06,
                    0x0c, 0x18, 0x30, 0x23,
                    0x05, 0x0a, 0x14, 0x28,
                    0x13, 0x26, 0x0f, 0x1e,
                    0x3c, 0x3b, 0x35, 0x29,
                    0x11, 0x22, 0x07, 0x0e,
                    0x1c, 0x38, 0x33, 0x25,
                    0x09, 0x12, 0x24                   
                    };

void OneRound(u8 x[16], u8 k[8]);
void OneRound_Inv(u8 x[16], u8 k[8]);

void KeySch(const u16 *key, u8 output[36][8])
{
     int i;
     u8 KeyR[KSIZE/4],temp, temp1, temp2, temp3;
          
      for(i=0;i<(KSIZE/4);i++)
     {
       KeyR[i] = (key[(i/4)]>>(4*(i&0x03))) & 0x0F;
     }

     for(i=0;i<35;i++)
      {
#if KSIZE == 80                      
      output[i][0] = KeyR[1];
      output[i][1] = KeyR[3];
      output[i][2] = KeyR[4];
      output[i][3] = KeyR[6];
      output[i][4] = KeyR[13];
      output[i][5] = KeyR[14];
      output[i][6] = KeyR[15];
      output[i][7] = KeyR[16];        
      
      KeyR[1]=KeyR[1] ^S[KeyR[0]];
      KeyR[4]=KeyR[4] ^S[KeyR[16]];
      KeyR[7]=KeyR[7] ^(CON[i]>>3);
      KeyR[19]=KeyR[19] ^(CON[i]&0x07);
      
      temp=KeyR[0];
      KeyR[0]=KeyR[1];
      KeyR[1]=KeyR[2];      
      KeyR[2]=KeyR[3];      
      KeyR[3]=temp;            
      
      temp=KeyR[0];
      temp1=KeyR[1];
      temp2=KeyR[2];
      temp3=KeyR[3];
      
      KeyR[0]=KeyR[4];
      KeyR[1]=KeyR[5];      
      KeyR[2]=KeyR[6];      
      KeyR[3]=KeyR[7];            
      
      KeyR[4]=KeyR[8];
      KeyR[5]=KeyR[9];      
      KeyR[6]=KeyR[10];      
      KeyR[7]=KeyR[11];           
      
      KeyR[8]=KeyR[12];
      KeyR[9]=KeyR[13];      
      KeyR[10]=KeyR[14];      
      KeyR[11]=KeyR[15];     

      KeyR[12]=KeyR[16];
      KeyR[13]=KeyR[17];      
      KeyR[14]=KeyR[18];      
      KeyR[15]=KeyR[19];
      
      KeyR[16]=temp;
      KeyR[17]=temp1;      
      KeyR[18]=temp2;      
      KeyR[19]=temp3;     
      
#elif KSIZE == 128
output[i][0] = KeyR[2];
      output[i][1] = KeyR[3];
      output[i][2] = KeyR[12];
      output[i][3] = KeyR[15];
      output[i][4] = KeyR[17];
      output[i][5] = KeyR[18];
      output[i][6] = KeyR[28];
      output[i][7] = KeyR[31];        
      
      KeyR[1]=KeyR[1] ^S[KeyR[0]];
      KeyR[4]=KeyR[4] ^S[KeyR[16]];
      KeyR[23]=KeyR[23] ^S[KeyR[30]];
      
      KeyR[7]=KeyR[7] ^(CON[i]>>3);
      KeyR[19]=KeyR[19] ^(CON[i]&0x07);
      
      temp=KeyR[0];
      KeyR[0]=KeyR[1];
      KeyR[1]=KeyR[2];      
      KeyR[2]=KeyR[3];      
      KeyR[3]=temp;  
                
      temp=KeyR[0];
      temp1=KeyR[1];
      temp2=KeyR[2];
      temp3=KeyR[3];
      
      KeyR[0]=KeyR[4];
      KeyR[1]=KeyR[5];      
      KeyR[2]=KeyR[6];      
      KeyR[3]=KeyR[7];            
      
      KeyR[4]=KeyR[8];
      KeyR[5]=KeyR[9];      
      KeyR[6]=KeyR[10];      
      KeyR[7]=KeyR[11];           
      
      KeyR[8]=KeyR[12];
      KeyR[9]=KeyR[13];      
      KeyR[10]=KeyR[14];      
      KeyR[11]=KeyR[15];     

      KeyR[12]=KeyR[16];
      KeyR[13]=KeyR[17];      
      KeyR[14]=KeyR[18];      
      KeyR[15]=KeyR[19];

      KeyR[16]=KeyR[20];
      KeyR[17]=KeyR[21];      
      KeyR[18]=KeyR[22];      
      KeyR[19]=KeyR[23];
      
      KeyR[20]=KeyR[24];
      KeyR[21]=KeyR[25];      
      KeyR[22]=KeyR[26];      
      KeyR[23]=KeyR[27];

      KeyR[24]=KeyR[28];
      KeyR[25]=KeyR[29];      
      KeyR[26]=KeyR[30];      
      KeyR[27]=KeyR[31];

      KeyR[28]=temp;
      KeyR[29]=temp1;      
      KeyR[30]=temp2;      
      KeyR[31]=temp3;          

#endif             
     }
     
#if KSIZE == 80      
      output[35][0] = KeyR[1];
      output[35][1] = KeyR[3];
      output[35][2] = KeyR[4];
      output[35][3] = KeyR[6];
      output[35][4] = KeyR[13];
      output[35][5] = KeyR[14];
      output[35][6] = KeyR[15];
      output[35][7] = KeyR[16];    
#elif KSIZE == 128
      output[35][0] = KeyR[2];
      output[35][1] = KeyR[3];
      output[35][2] = KeyR[12];
      output[35][3] = KeyR[15];
      output[35][4] = KeyR[17];
      output[35][5] = KeyR[18];
      output[35][6] = KeyR[28];
      output[35][7] = KeyR[31];  
#endif         

}     

 void OneRound(u8 x[16], u8 k[8])
{
  u8 t[16];
  u8 i;

    for(i=0;i<8;i++)
    {
    x[2*i+1]=S[x[2*i]^k[i]]^x[2*i+1] & 0x0F;
    }

    for(i=0;i<16;i++)
    {
      t[Pi[i]]=x[i];
    }
  
    for(i=0;i<16;i++)
    {
    x[i]=t[i];
    }
}

 void Encrypt(u8 x[16], u8 Subkey[36][8])
{
     u8 i;
     // 35 TOURS
     for(i=0;i<35;i++)
    {
      OneRound(x,Subkey[i]);
    }
    // DERNIER TOUR
    for(i=0;i<8;i++)
    {
      x[2*i+1]=S[x[2*i]^Subkey[35][i]]^x[2*i+1] & 0x0F;
    }
}

 void OneRound_Inv(u8 x[16], u8 k[8])
{
  u8 t[16];
  u8 i;

   for(i=0;i<8;i++)
    {
    x[2*i+1]=S[x[2*i]^k[i]]^x[2*i+1] & 0x0F;
    }

    for(i=0;i<16;i++)
    {
    t[Pi_Inv[i]]=x[i];
    }

    for(i=0;i<16;i++)
    {
    x[i]=t[i];
    }	
}


 void Decrypt(u8 x[16], u8 Subkey[36][8])
{
  u8 i;
  
  // 35 TOURS
  for(i=35;i>0;i--)
    {
    OneRound_Inv(x,Subkey[i]);
    }
        // DERNIER TOUR
    for(i=0;i<8;i++)
    {
    x[2*i+1]=S[x[2*i]^Subkey[0][i]]^x[2*i+1] & 0x0F;
    }
}


int main()
{
	#ifdef PRINT
	uart1_init();
	#endif

 u8 Plain[16], SubK[36][8];
 u16 KEY[(KSIZE/4)];
 u8 i;

#if KSIZE == 80
  KEY[0] = 0x1100;
  KEY[1] = 0x3322;
  KEY[2] = 0x5544;
  KEY[3] = 0x7766;
  KEY[4] = 0x9988;

#elif KSIZE == 128
  KEY[0] = 0x1100;
  KEY[1] = 0x3322;
  KEY[2] = 0x5544;
  KEY[3] = 0x7766;
  KEY[4] = 0x9988;
  KEY[5] = 0xBBAA;
  KEY[6] = 0xDDCC;
  KEY[7] = 0xFFEE;
#endif
  
  for(i=0;i<16;i++)
  {
  Plain[i] = i;
  }

	#ifdef PRINT
	printf("----------Clair----------\n\n");
	for(i=0;i<16;i++) 
	{ 

	printf("%X ",Plain[i]); 
	}
	printf("\n"); 
	#endif

	START_ENCRYPT();

    KeySch(KEY, SubK);
    Encrypt(Plain,SubK);
	
	#ifdef PRINT
	printf("----------Chiffr\'e----------\n\n"); 
	for(i=0;i<16;i++) 
	{ 
	printf("%X ",Plain[i]); 
	}
	printf("\n");
	#endif


    START_DECRYPT();
    Decrypt(Plain,SubK);
	
	#ifdef PRINT
	printf("----------Déchiffr\'e----------\n\n");
	for(i=0;i<16;i++) 
	{ 
	printf("%X ",Plain[i]); 
	}
	printf("\n"); 
	#endif

    END_EXPE();

 return 0;  
 
} 
