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


void katan32_encrypt( const u64 plain[32], u64 cipher[32], const u64 key[80], int rounds ) {

  u64 L1[13], L2[19], k[2*rounds], fa, fb;
  int i,j;

  for(i=0;i<19;++i) 
    L2[i] = plain[i];
  for(i=0;i<13;++i) 
    L1[i] = plain[i+19];

  for(i=0;i<80;++i)
    k[i]=key[i];
  for(i=80;i<2*rounds;++i)
    k[i]=k[i-80] ^ k[i-61] ^ k[i-50] ^ k[i-13] ;

  for(i=0;i<rounds;++i) {
    
    fa = L1[X1_32] ^ L1[X2_32] ^ (L1[X3_32] & L1[X4_32]) ^ (L1[X5_32] & IR[i])     ^ k[2*i];
    fb = L2[Y1_32] ^ L2[Y2_32] ^ (L2[Y3_32] & L2[Y4_32]) ^ (L2[Y5_32] & L2[Y6_32]) ^ k[2*i+1];

    for(j=12;j>0;--j)
      L1[j] = L1[j-1];
    for(j=18;j>0;--j)
      L2[j] = L2[j-1];
    L1[0] = fb;
    L2[0] = fa;
  }

  for(i=0;i<19;++i) 
    cipher[i] = L2[i];
  for(i=0;i<13;++i) 
    cipher[i+19] = L1[i];

}


void katan32_decrypt( const u64 cipher[32], u64 plain[32], const u64 key[80], int rounds ) {

  u64 L1[13], L2[19], k[2*rounds], fa, fb;
  int i,j;
  
  for(i=0;i<19;++i) 
    L2[i] = cipher[i];
  for(i=0;i<13;++i) 
    L1[i] = cipher[i+19];

  for(i=0;i<80;++i)
    k[i]=key[i];
  for(i=80;i<2*rounds;++i)
    k[i]=k[i-80] ^ k[i-61] ^ k[i-50] ^ k[i-13] ;

  for(i=rounds-1;i>=0;--i) {

    fb = L1[0];    
    fa = L2[0];
    for(j=0;j<12;++j)
      L1[j] = L1[j+1];
    for(j=0;j<18;++j)
      L2[j] = L2[j+1];
    
    L1[X1_32] = fa ^ L1[X2_32] ^ (L1[X3_32] & L1[X4_32]) ^ (L1[X5_32] & IR[i])     ^ k[2*i];
    L2[Y1_32] = fb ^ L2[Y2_32] ^ (L2[Y3_32] & L2[Y4_32]) ^ (L2[Y5_32] & L2[Y6_32]) ^ k[2*i+1];
  }
  
  for(i=0;i<19;++i) 
    plain[i] = L2[i];
  for(i=0;i<13;++i) 
    plain[i+19] = L1[i];
  
}

int main()
{
  u64 key[80];
  u64 plain[64], cipher[64];
  int i;

  for(i=0;i<80;++i)   key[i]=ONES;
  for(i=0;i<32;++i)   plain[i]=0;
  
  START_ENCRYPT();
  katan32_encrypt( plain, cipher, key, 254 );
  
  //printf("\nkatan32_encrypt(key=11..11, plain=00.00) = ");
  //  for(i=0;i<32;++i)   printf("%llu",cipher[i]&1);
  //for(i=31;i>=0;i--)   printf("%llu",cipher[i]&1);
  
  START_DECRYPT();
  katan32_decrypt( cipher, plain, key, 254 );
  
  END_EXPE();
  
  return 0;
}