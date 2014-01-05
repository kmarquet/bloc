/*

 Reference BITSLICED implementations of:
 KATAN32, KATAN48, KATAN64, KTANTAN32, KTANTAN48 and KTANTAN64.
 Each of the 64 slices corresponds to a distinct instance.
 
 To work with a single instance, use values in {0,1} 
 (ie, only consider the least significant slice).

 Authors: 
 Jean-Philippe Aumasson, FHNW, Windisch, Switzerland
 Miroslav Knezevic, Katholieke Universiteit Leuven, Belgium
 Orr Dunkelman, Weizmann Institute of Science, Israel

 Thanks goes to Bo Zhu for pointing out a bug in the KTANTAN part

 Thanks ges to Wei Lei for pointing out a bug in the KTANTAN part
*/


#include "tools.h"
#include <stdint.h>
typedef uint64_t u64;

#define ONES 0xFFFFFFFFFFFFFFFFULL

#define X1_32  12
#define X2_32  7
#define X3_32  8
#define X4_32  5
#define X5_32  3
#define Y1_32  18
#define Y2_32  7
#define Y3_32  12
#define Y4_32  10
#define Y5_32  8
#define Y6_32  3

#define X1_48  18
#define X2_48  12
#define X3_48  15
#define X4_48  7
#define X5_48  6
#define Y1_48  28
#define Y2_48  19
#define Y3_48  21
#define Y4_48  13
#define Y5_48  15
#define Y6_48  6

#define X1_64  24
#define X2_64  15
#define X3_64  20
#define X4_64  11
#define X5_64  9
#define Y1_64  38
#define Y2_64  25
#define Y3_64  33
#define Y4_64  21
#define Y5_64  14
#define Y6_64  9


// IR constants, either 1 for all slices, are 0 for all slices
const u64 IR[254] = {
  ONES,ONES,ONES,ONES,ONES,ONES,ONES,0,0,0, // 0-9 
  ONES,ONES,0,ONES,0,ONES,0,ONES,0,ONES,
  ONES,ONES,ONES,0,ONES,ONES,0,0,ONES,ONES,
  0,0,ONES,0,ONES,0,0,ONES,0,0,
  0,ONES,0,0,0,ONES,ONES,0,0,0,
  ONES,ONES,ONES,ONES,0,0,0,0,ONES,0,
  0,0,0,ONES,0,ONES,0,0,0,0, // 60-69
  0,ONES,ONES,ONES,ONES,ONES,0,0,ONES,ONES,
  ONES,ONES,ONES,ONES,0,ONES,0,ONES,0,0,
  0,ONES,0,ONES,0,ONES,0,0,ONES,ONES,
  0,0,0,0,ONES,ONES,0,0,ONES,ONES,
  ONES,0,ONES,ONES,ONES,ONES,ONES,0,ONES,ONES,
  ONES,0,ONES,0,0,ONES,0,ONES,0,ONES, // 120-129
  ONES,0,ONES,0,0,ONES,ONES,ONES,0,0,
  ONES,ONES,0,ONES,ONES,0,0,0,ONES,0,
  ONES,ONES,ONES,0,ONES,ONES,0,ONES,ONES,ONES,
  ONES,0,0,ONES,0,ONES,ONES,0,ONES,ONES,
  0,ONES,0,ONES,ONES,ONES,0,0,ONES,0,
  0,ONES,0,0,ONES,ONES,0,ONES,0,0, // 180-189
  0,ONES,ONES,ONES,0,0,0,ONES,0,0,
  ONES,ONES,ONES,ONES,0,ONES,0,0,0,0,
  ONES,ONES,ONES,0,ONES,0,ONES,ONES,0,0,
  0,0,0,ONES,0,ONES,ONES,0,0,ONES,
  0,0,0,0,0,0,ONES,ONES,0,ONES,
  ONES,ONES,0,0,0,0,0,0,0,ONES, // 240-249
  0,0,ONES,0,
};

void katan48_encrypt( const u64 plain[48], u64 cipher[48], const u64 key[80], int rounds ) {

  u64 L1[19], L2[29], k[2*rounds], fa_1, fa_0, fb_1, fb_0;
  int i,j;

  for(i=0;i<29;++i) 
    L2[i] = plain[i];
  for(i=0;i<19;++i) 
    L1[i] = plain[i+29];

  for(i=0;i<80;++i)
    k[i]=key[i];
  for(i=80;i<2*rounds;++i)
    k[i]=k[i-80] ^ k[i-61] ^ k[i-50] ^ k[i-13];

  for(i=0;i<rounds;++i) {
    
    fa_1 = L1[X1_48]   ^ L1[X2_48]   ^ (L1[X3_48] & L1[X4_48])     ^ (L1[X5_48] & IR[i])         ^ k[2*i];
    fa_0 = L1[X1_48-1] ^ L1[X2_48-1] ^ (L1[X3_48-1] & L1[X4_48-1]) ^ (L1[X5_48-1] & IR[i])       ^ k[2*i];
    fb_1 = L2[Y1_48]   ^ L2[Y2_48]   ^ (L2[Y3_48] & L2[Y4_48])     ^ (L2[Y5_48] & L2[Y6_48])     ^ k[2*i+1];
    fb_0 = L2[Y1_48-1] ^ L2[Y2_48-1] ^ (L2[Y3_48-1] & L2[Y4_48-1]) ^ (L2[Y5_48-1] & L2[Y6_48-1]) ^ k[2*i+1];

    for(j=18;j>1;--j)
      L1[j] = L1[j-2];
    for(j=28;j>1;--j)
      L2[j] = L2[j-2];
    L1[1] = fb_1;
    L1[0] = fb_0;
    L2[1] = fa_1;
    L2[0] = fa_0;
  }

  for(i=0;i<29;++i) 
    cipher[i] = L2[i];
  for(i=0;i<19;++i) 
    cipher[i+29] = L1[i];

}


void katan48_decrypt( const u64 cipher[48], u64 plain[48], const u64 key[80], int rounds ) {

  u64 L1[19], L2[29], k[2*rounds], fa_1, fa_0, fb_1, fb_0;
  int i,j;

  
  for(i=0;i<29;++i) 
    L2[i] = cipher[i];
  for(i=0;i<19;++i) 
    L1[i] = cipher[i+29];

  for(i=0;i<80;++i)
    k[i]=key[i];
  for(i=80;i<2*rounds;++i)
    k[i]=k[i-80] ^ k[i-61] ^ k[i-50] ^ k[i-13] ;

  for(i=rounds-1;i>=0;--i) {

    fb_1 = L1[1];    
    fb_0 = L1[0];    
    fa_1 = L2[1];
    fa_0 = L2[0];
    for(j=0;j<17;++j)
      L1[j] = L1[j+2];
    for(j=0;j<27;++j)
      L2[j] = L2[j+2];

    L1[X1_48]   = fa_1 ^ L1[X2_48]   ^ (L1[X3_48] & L1[X4_48])     ^ (L1[X5_48] & IR[i])         ^ k[2*i];
    L1[X1_48-1] = fa_0 ^ L1[X2_48-1] ^ (L1[X3_48-1] & L1[X4_48-1]) ^ (L1[X5_48-1] & IR[i])       ^ k[2*i];
    L2[Y1_48]   = fb_1 ^ L2[Y2_48]   ^ (L2[Y3_48] & L2[Y4_48])     ^ (L2[Y5_48] & L2[Y6_48])     ^ k[2*i+1];
    L2[Y1_48-1] = fb_0 ^ L2[Y2_48-1] ^ (L2[Y3_48-1] & L2[Y4_48-1]) ^ (L2[Y5_48-1] & L2[Y6_48-1]) ^ k[2*i+1];
  }
  
  for(i=0;i<29;++i) 
    plain[i] = L2[i];
  for(i=0;i<19;++i) 
    plain[i+29] = L1[i];

}

int main()
{
  u64 key[80];
  u64 plain[64], cipher[64];
  int i;
  
  for(i=0;i<80;++i)   key[i]=ONES;
  for(i=0;i<48;++i)   plain[i]=0;
  
  START_ENCRYPT();
  katan48_encrypt( plain, cipher, key, 254 );
  
  //printf("\nkatan48_encrypt(key=11..11, plain=00.00) = ");
  //  for(i=0;i<48;++i)   printf("%llu",cipher[i]&1);
  //for(i=47;i>=0;i--)   printf("%llu",cipher[i]&1);
  
  START_DECRYPT();
  katan48_decrypt( cipher, plain, key, 254 );
  
  END_EXPE();
  return 0;  
}