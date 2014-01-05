#include<stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;

#include "tools.h"

/* expanded S-box S DESXL */
static const u8 SBE[64] =
{
14, 0, 5, 11, 5, 10, 0, 7, 7, 9, 8, 6, 2, 4, 15, 
9, 11, 6, 14,13, 8, 13, 3, 4, 1, 12, 2, 1, 15, 3, 
12, 10, 4, 10, 9, 7, 9, 12, 6, 1, 2, 15, 15, 12, 14, 
1, 5, 2, 8, 5, 3, 0, 7, 11, 8, 14, 13, 3, 4, 10, 0, 6, 11, 13
};

/* two tables required for the keyschedule */
static const u32 LHs[16] =
{
    0x00000000, 0x00000001, 0x00000100, 0x00000101,
    0x00010000, 0x00010001, 0x00010100, 0x00010101,
    0x01000000, 0x01000001, 0x01000100, 0x01000101,
    0x01010000, 0x01010001, 0x01010100, 0x01010101
};

static const u32 RHs[16] =
{
    0x00000000, 0x01000000, 0x00010000, 0x01010000,
    0x00000100, 0x01000100, 0x00010100, 0x01010100,
    0x00000001, 0x01000001, 0x00010001, 0x01010001,
    0x00000101, 0x01000101, 0x00010101, 0x01010101,
};


/* Key permute Map, which also removes the parity bits  PC1 
static const u8 KeyPermuteMap[] = 
{ 
56, 48, 40, 32, 24, 16,  8,  0,
57, 49, 41, 33, 25, 17,  9,  1,
58, 50, 42, 34, 26, 18, 10,  2,
59, 51, 43, 35, 62, 54, 46, 38,
30, 22, 14,  6, 61, 53, 45, 37,
29, 21, 13,  5, 60, 52, 44, 36,
28, 20, 12,  4, 27, 19, 11,  3
};*/

/* Key comression, which select 48 out of the 56 bis of the key PC2 
static const u8 KeyCompression[] =  
{
13, 16, 10, 23,  0,  4,  2, 27,
14,  5, 20,  9, 22, 18, 11,  3,
25,  7, 15,  6, 26, 19, 12,  1,
40, 51, 30, 36, 46, 54, 29, 39,
50, 44, 32, 47, 43, 48, 38, 55,
33, 52, 45, 41, 49, 35, 28, 31
};*/

#define GET_UINT32_BE(n,b,i)                            \
{                                                       \
    (n) = ( (u32) (b)[(i)    ] << 24 )             \
        | ( (u32) (b)[(i) + 1] << 16 )             \
        | ( (u32) (b)[(i) + 2] <<  8 )             \
        | ( (u32) (b)[(i) + 3]       );            \
}



#define PUT_UINT32_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}

#define SWAP(a,b) { u32 t = a; a = b; b = t; t = 0; }

#define DES_ROUND(X,Y)                                                                  \
{                                                                                       \    
  T=SBE[(((X>>31)&0x01)^(X<<1)^*SK++)& 0x3F]^ \
      (SBE[((X>>3)^(*SK>>6))& 0x3F]<<4)^    \
      (SBE[((X>>7)^(*SK>>12))& 0x3F]<<8)^    \
      (SBE[((X>>11)^(*SK>>18))& 0x3F]<<12)^  \      
      (SBE[((X>>15)^(*SK>>24))& 0x3F]<<16)^  \ 
      (SBE[((X>>19)^(*SK>>30)^(*SK++<<2))& 0x3F]<<20)^  \
      (SBE[((X>>23)^(*SK>>4))& 0x3F]<<24)^              \    
      (SBE[((X>>27)^(*SK>>10))& 0x3F]<<28);         \
      \
    T=((T&0x01)<<8)^((T&0x02)<<15)^((T&0x04)<<20)^((T&0x08)<<27)                        \
     ^((T&0x10)<<8)^((T&0x20)<<22)^((T&0x40)>>5)^((T&0x80)<<10)                         \
     ^((T&0x100)<<15)^((T&0x200)<<6)^((T&0x400)<<19)^((T&0x800)>>6)                     \
     ^((T&0x1000)<<13)^((T&0x2000)<<6)^((T&0x4000)>>5)^((T&0x8000)>>15)                 \            
     ^((T&0x10000)>>9)^((T&0x20000)>>4)^((T&0x40000)<<6)^((T&0x80000)>>17)   \
     ^((T&0x100000)>>17)^((T&0x200000)<<7)^((T&0x400000)>>12)^((T&0x800000)>>5)\
     ^((T&0x1000000)<<7)^((T&0x2000000)>>14)^((T&0x4000000)>>5)^((T&0x8000000)>>21) \
     ^((T&0x10000000)>>24)^((T&0x20000000)>>3)^((T&0x40000000)>>16)^((T&0x80000000)>>11); \
     \
     Y^=T; \
}



 void SetKey(u32 SK[32], const unsigned char key[8] )
{
    int i;
    u32 X, Y, T;

    GET_UINT32_BE( X, key, 0 );
    GET_UINT32_BE( Y, key, 4 );

    /*
* Permuted Choice 1
*/
    T = ((Y >> 4) ^ X) & 0x0F0F0F0F; X ^= T; Y ^= (T << 4);
    T = ((Y ) ^ X) & 0x10101010; X ^= T; Y ^= (T );

    X = (LHs[ (X ) & 0xF] << 3) | (LHs[ (X >> 8) & 0xF ] << 2)
        | (LHs[ (X >> 16) & 0xF] << 1) | (LHs[ (X >> 24) & 0xF ] )
        | (LHs[ (X >> 5) & 0xF] << 7) | (LHs[ (X >> 13) & 0xF ] << 6)
        | (LHs[ (X >> 21) & 0xF] << 5) | (LHs[ (X >> 29) & 0xF ] << 4);

    Y = (RHs[ (Y >> 1) & 0xF] << 3) | (RHs[ (Y >> 9) & 0xF ] << 2)
        | (RHs[ (Y >> 17) & 0xF] << 1) | (RHs[ (Y >> 25) & 0xF ] )
        | (RHs[ (Y >> 4) & 0xF] << 7) | (RHs[ (Y >> 12) & 0xF ] << 6)
        | (RHs[ (Y >> 20) & 0xF] << 5) | (RHs[ (Y >> 28) & 0xF ] << 4);

    X &= 0x0FFFFFFF;
    Y &= 0x0FFFFFFF;

    /*
* calculate subkeys
*/
    for( i = 0; i < 16; i++ )
    {
        if( i < 2 || i == 8 || i == 15 )
        {
            X = ((X << 1) | (X >> 27)) & 0x0FFFFFFF;
            Y = ((Y << 1) | (Y >> 27)) & 0x0FFFFFFF;
        }
        else
        {
            X = ((X << 2) | (X >> 26)) & 0x0FFFFFFF;
            Y = ((Y << 2) | (Y >> 26)) & 0x0FFFFFFF;
        }

        *SK++ = ((X << 4) & 0x24000000) | ((X << 28) & 0x10000000)
                | ((X << 14) & 0x08000000) | ((X << 18) & 0x02080000)
                | ((X << 6) & 0x01000000) | ((X << 9) & 0x00200000)
                | ((X >> 1) & 0x00100000) | ((X << 10) & 0x00040000)
                | ((X << 2) & 0x00020000) | ((X >> 10) & 0x00010000)
                | ((Y >> 13) & 0x00002000) | ((Y >> 4) & 0x00001000)
                | ((Y << 6) & 0x00000800) | ((Y >> 1) & 0x00000400)
                | ((Y >> 14) & 0x00000200) | ((Y ) & 0x00000100)
                | ((Y >> 5) & 0x00000020) | ((Y >> 10) & 0x00000010)
                | ((Y >> 3) & 0x00000008) | ((Y >> 18) & 0x00000004)
                | ((Y >> 26) & 0x00000002) | ((Y >> 24) & 0x00000001);

        *SK++ = ((X << 15) & 0x20000000) | ((X << 17) & 0x10000000)
                | ((X << 10) & 0x08000000) | ((X << 22) & 0x04000000)
                | ((X >> 2) & 0x02000000) | ((X << 1) & 0x01000000)
                | ((X << 16) & 0x00200000) | ((X << 11) & 0x00100000)
                | ((X << 3) & 0x00080000) | ((X >> 6) & 0x00040000)
                | ((X << 15) & 0x00020000) | ((X >> 4) & 0x00010000)
                | ((Y >> 2) & 0x00002000) | ((Y << 8) & 0x00001000)
                | ((Y >> 14) & 0x00000808) | ((Y >> 9) & 0x00000400)
                | ((Y ) & 0x00000200) | ((Y << 7) & 0x00000100)
                | ((Y >> 7) & 0x00000020) | ((Y >> 3) & 0x00000011)
                | ((Y << 2) & 0x00000004) | ((Y >> 21) & 0x00000002);
    }
}




void Encrypt( u32 SK[32],
                    const unsigned char input[8],
                    unsigned char output[8] )
{
    int i;
    u32 X, Y, T;


    GET_UINT32_BE( X, input, 0 );
    GET_UINT32_BE( Y, input, 4 );

    for( i = 0; i < 8; i++ )
    {
        DES_ROUND( Y, X );
        DES_ROUND( X, Y );
    }

    PUT_UINT32_BE( Y, output, 0 );
    PUT_UINT32_BE( X, output, 4 );



}


void Decrypt( u32 SK[32],
                    const unsigned char input[8],
                    unsigned char output[8] )
{                    
     int i;
     u32 X, Y, T;                    

for( i = 0; i < 16; i += 2 )
    {
        SWAP( SK[i    ], SK[30 - i] );
        SWAP( SK[i + 1], SK[31 - i] );
    }

    GET_UINT32_BE( X, input, 0 );
    GET_UINT32_BE( Y, input, 4 );



    for( i = 0; i < 8; i++ )
    {
        DES_ROUND( Y, X );
        DES_ROUND( X, Y );
    }



    PUT_UINT32_BE( Y, output, 0 );
    PUT_UINT32_BE( X, output, 4 );

}
    
   
int main()
{
    u8 input[8], output[8], i, key[8];
    u32 K[2], SK[32];

 
 
	for(i=0;i<8;i++)
    {                
     input[i]=i;           
     key[i]=i;
    }

  START_ENCRYPT();
    
  SetKey(SK, key);

  START_DECRYPT();
	
  Encrypt(SK, input, output);  

  /*
	for(i=0;i<8;i++) 
	{ 
	printf("\n%x ",output[i]); 
	}
	*/
    
   Decrypt(SK, output, input);  

   /*
	for(i=0;i<8;i++) 
	{ 
	printf("\n%x ",input[i]); 
	}
	*/

//system("PAUSE");

   END_EXPE();
   return 0;
}
