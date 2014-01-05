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



u64 mux4_1 (u64 x[4], u64 s[2]) {

  return (x[3] & s[1] & s[0]) | (x[2] & s[1] & ~s[0]) | (x[1] & ~s[1] & s[0]) | (x[0] & ~s[1] & ~s[0]);
}


u64 mux16_1(u64 x[16], u64 s[4]) {

  u64 ya, yb, yc, yd, xa[4], xb[4], xc[4], xd[4], xe[4], sa[2], se[2];
  
  xa[0] = x[12];
  xa[1] = x[13];
  xa[2] = x[14];
  xa[3] = x[15];

  xb[0] = x[8];
  xb[1] = x[9];
  xb[2] = x[10];
  xb[3] = x[11];

  xc[0] = x[4];
  xc[1] = x[5];
  xc[2] = x[6];
  xc[3] = x[7];

  xd[0] = x[0];
  xd[1] = x[1];
  xd[2] = x[2];
  xd[3] = x[3];

  sa[0] = s[0];
  sa[1] = s[1];

  se[0] = s[2];
  se[1] = s[3];

  ya = mux4_1(xa, sa);
  yb = mux4_1(xb, sa);
  yc = mux4_1(xc, sa);
  yd = mux4_1(xd, sa);

  xe[0] = yd;
  xe[1] = yc;
  xe[2] = yb;
  xe[3] = ya;

  return mux4_1(xe, se);
}


void ktantan32_encrypt( const u64 plain[32], u64 cipher[32], const u64 key[80], int rounds ) {

  u64 L1[13], L2[19], fa, fb;
  u64 x16a[16], x16b[16], x16c[16], x16d[16], x16e[16], s16a[4], x4f[4], x4g[4], s4f[2], s4g[2], a0, a4, ka[rounds], kb[rounds], T[8], tmp;
  int i,j;

  for(i=0;i<19;++i) 
    L2[i] = plain[i];
  for(i=0;i<13;++i) 
    L1[i] = plain[i+19];

  for(i=0;i<8;++i)
    T[i] = ONES;

  for(i=0;i<rounds;++i) {

    tmp = T[7] ^ T[6] ^ T[4] ^ T[2]; 
    for(j=7;j>0;--j)
    T[j] = T[j-1];
    T[0] = tmp;

    for(j=0;j<16;++j)
      x16a[j] = key[j];
    for(j=0;j<16;++j)
      x16b[j] = key[j+16];
    for(j=0;j<16;++j)
      x16c[j] = key[j+32];
    for(j=0;j<16;++j)
      x16d[j] = key[j+48];
    for(j=0;j<16;++j)
      x16e[j] = key[j+64];
    
    s16a[0] = T[4];
    s16a[1] = T[5];
    s16a[2] = T[6];
    s16a[3] = T[7];

    x4f[3] = mux16_1(x16e, s16a);
    x4f[2] = mux16_1(x16d, s16a);
    x4f[1] = mux16_1(x16c, s16a);
    x4f[0] = mux16_1(x16b, s16a);
 
    x4g[3] = mux16_1(x16d, s16a);
    x4g[2] = mux16_1(x16c, s16a);
    x4g[1] = mux16_1(x16b, s16a);
    x4g[0] = mux16_1(x16a, s16a);
   
    s4f[0] = T[0];
    s4f[1] = T[1];
    s4g[0] = ~T[0];
    s4g[1] = ~T[1];
		     
    a0 = mux16_1(x16a, s16a);
    a4 = mux16_1(x16e, s16a);

    ka[i] = (~T[3] & ~T[2] & a0) ^ ((T[3] |  T[2]) & mux4_1(x4f, s4f));
    kb[i] = (~T[3] &  T[2] & a4) ^ ((T[3] | ~T[2]) & mux4_1(x4g, s4g));

    fa = L1[X1_32] ^ L1[X2_32] ^ (L1[X3_32] & L1[X4_32]) ^ (L1[X5_32] & IR[i])     ^ ka[i];
    fb = L2[Y1_32] ^ L2[Y2_32] ^ (L2[Y3_32] & L2[Y4_32]) ^ (L2[Y5_32] & L2[Y6_32]) ^ kb[i];

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


void ktantan32_decrypt( const u64 cipher[32], u64 plain[32], const u64 key[80], int rounds ) {

  u64 L1[13], L2[19], fa, fb;
  u64 x16a[16], x16b[16], x16c[16], x16d[16], x16e[16], s16a[4], x4f[4], x4g[4], s4f[2], s4g[2], a0, a4, ka[rounds], kb[rounds], T[8], tmp;
  int i,j;
  
  for(i=0;i<19;++i) 
    L2[i] = cipher[i];
  for(i=0;i<13;++i) 
    L1[i] = cipher[i+19];

  for(i=0;i<8;++i)
    T[i] = ONES;

  for(i=0;i<rounds;++i) {

    tmp = T[7] ^ T[6] ^ T[4] ^ T[2]; 
    for(j=7;j>0;--j)
    T[j] = T[j-1];
    T[0] = tmp;

    for(j=0;j<16;++j)
      x16a[j] = key[j];
    for(j=0;j<16;++j)
      x16b[j] = key[j+16];
    for(j=0;j<16;++j)
      x16c[j] = key[j+32];
    for(j=0;j<16;++j)
      x16d[j] = key[j+48];
    for(j=0;j<16;++j)
      x16e[j] = key[j+64];
    
    s16a[0] = T[4];
    s16a[1] = T[5];
    s16a[2] = T[6];
    s16a[3] = T[7];

    x4f[3] = mux16_1(x16e, s16a);
    x4f[2] = mux16_1(x16d, s16a);
    x4f[1] = mux16_1(x16c, s16a);
    x4f[0] = mux16_1(x16b, s16a);
 
    x4g[3] = mux16_1(x16d, s16a);
    x4g[2] = mux16_1(x16c, s16a);
    x4g[1] = mux16_1(x16b, s16a);
    x4g[0] = mux16_1(x16a, s16a);
   
    s4f[0] = T[0];
    s4f[1] = T[1];
    s4g[0] = ~T[0];
    s4g[1] = ~T[1];
		     
    a0 = mux16_1(x16a, s16a);
    a4 = mux16_1(x16e, s16a);

    ka[i] = (~T[3] & ~T[2] & a0) ^ ((T[3] |  T[2]) & mux4_1(x4f, s4f));
    kb[i] = (~T[3] &  T[2] & a4) ^ ((T[3] | ~T[2]) & mux4_1(x4g, s4g));
  }

  for(i=rounds-1;i>=0;--i) {

    fb = L1[0];    
    fa = L2[0];
    for(j=0;j<12;++j)
      L1[j] = L1[j+1];
    for(j=0;j<18;++j)
      L2[j] = L2[j+1];
    
    L1[X1_32] = fa ^ L1[X2_32] ^ (L1[X3_32] & L1[X4_32]) ^ (L1[X5_32] & IR[i])     ^ ka[i];
    L2[Y1_32] = fb ^ L2[Y2_32] ^ (L2[Y3_32] & L2[Y4_32]) ^ (L2[Y5_32] & L2[Y6_32]) ^ kb[i];
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
  ktantan32_encrypt( plain, cipher, key, 254 );
  //printf("\nktantan32_encrypt(key=11..11, plain=00.00) = ");
  //  for(i=0;i<32;++i)   printf("%llu",cipher[i]&1);
  //for(i=31;i>=0;i--)   printf("%llu",cipher[i]&1);
  
  START_DECRYPT();
  ktantan32_decrypt( cipher, plain, key, 254 );
  
  END_EXPE();
  return 0;
}
