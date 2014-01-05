/* NoekeonDirectRef.c
 *
 *   The text vectors should be as follows:
 * 
 *                      k = 00000000 00000000 00000000 00000000
 *                      a = 00000000 00000000 00000000 00000000
 * after NESSIEencrypt, b = b1656851 699e29fa 24b70148 503d2dfc
 * after NESSIEdecrypt, a?= 00000000 00000000 00000000 00000000
 * 
 *                      k = ffffffff ffffffff ffffffff ffffffff
 *                      a = ffffffff ffffffff ffffffff ffffffff
 * after NESSIEencrypt, b = 2a78421b 87c7d092 4f26113f 1d1349b2
 * after NESSIEdecrypt, a?= ffffffff ffffffff ffffffff ffffffff
 * 
 *                      k = b1656851 699e29fa 24b70148 503d2dfc
 *                      a = 2a78421b 87c7d092 4f26113f 1d1349b2
 * after NESSIEencrypt, b = e2f687e0 7b75660f fc372233 bc47532c
 * after NESSIEdecrypt, a?= 2a78421b 87c7d092 4f26113f 1d1349b2
 */
/************************************************************************************/
//#define PRINT

#include "tools.h"

#include "Nessie.h"
/*==================================================================================*/
#define   NROUND		16	
/*----------------------------------------------------------------------------------*/
/* Round Constants are : 80,1B,36,6C,D8,AB,4D,9A,2F,5E,BC,63,C6,97,35,6A,D4 (encrypt)*/
/*----------------------------------------------------------------------------------*/
#define RC1ENCRYPTSTART  T8 (0x80)
#define RC2DECRYPTSTART  T8 (0xD4) 

static const u32 NullVector[4] = {0,0,0,0};


 void Theta (u32 const * const k,u32 * const a)
{
  u32 tmp;

  tmp  = a[0]^a[2]; 
  tmp ^= ROTL32(tmp,8)^ROTL32(tmp,24); 
  a[1]^= tmp; 
  a[3]^= tmp; 

  a[0] ^= k[0]; a[1] ^= k[1]; a[2] ^= k[2]; a[3] ^= k[3]; 

  tmp  = a[1]^a[3]; 
  tmp ^= ROTL32(tmp,8)^ROTL32(tmp,24); 
  a[0]^= tmp; 
  a[2]^= tmp; 

} /* Theta */


 void Pi1(u32 * const a)
{ a[1] = ROTL32 (a[1], 1); 
  a[2] = ROTL32 (a[2], 5); 
  a[3] = ROTL32 (a[3], 2); 
}  /* Pi1 */


 void Pi2(u32 * const a)
{ a[1] = ROTL32 (a[1], 31);
  a[2] = ROTL32 (a[2], 27); 
  a[3] = ROTL32 (a[3], 30); 
}  /* Pi2 */



 void Gamma(u32 * const a)
{ u32 tmp;

  /* first non-linear step in gamma */
  a[1] ^= ~a[3] & ~a[2];
  a[0] ^=   a[2] & a[1];

  /* linear step in gamma */
  tmp   = a[3];
  a[3]  = a[0];
  a[0]  = tmp;
  a[2] ^= a[0]^a[1]^a[3];

  /* last non-linear step in gamma */
  a[1] ^= ~a[3] & ~a[2];
  a[0] ^=   a[2] & a[1];
} /* Gamma */


 void Round (u32 const * const k,u32 * const a,u8 const RC1,u8 const RC2)
{ 
  a[0] ^= RC1;
  Theta(k,a); 
  a[0] ^= RC2;
  Pi1(a); 
  Gamma(a); 
  Pi2(a); 
}  /* Round */


 void RCShiftRegFwd (u8 * const RC)
{ 

  if ((*RC)&0x80) (*RC)=((*RC)<<1) ^ 0x1B; else (*RC)<<=1;
  

  
} /* RCShiftRegFwd */


 void RCShiftRegBwd (u8 * const RC)
{ 

  if ((*RC)&0x01) (*RC)=((*RC)>>1) ^ 0x8D; else (*RC)>>=1;
  
} /* RCShiftRegBwd */


 void CommonLoop (u32 const * const k,u32 * const a, u8 RC1, u8 RC2)
{ 
  unsigned i;

  for(i=0 ; i<NROUND ; i++) {
    Round(k,a,RC1,RC2); 
    RCShiftRegFwd(&RC1);
    RCShiftRegBwd(&RC2);
  }
  a[0]^=RC1;
  Theta(k,a); 
  a[0]^=RC2;

} /* CommonLoop */

int main()
{
#ifdef PRINT
  uart1_init();
#endif

  u32 k[4]={0,0,0,0};
  u32 state[4]={0,0,0,0};
  
  START_ENCRYPT();

  CommonLoop (k,state,RC1ENCRYPTSTART,0);
  
#ifdef PRINT
  printf("b1656851 699e29fa 24b70148 503d2dfc\n");
  printf("%lx ", state[0]);
  printf("%lx ", state[1]);
  printf("%lx ", state[2]);
  printf("%lx ", state[3]);
  printf("\n");	
#endif

  START_DECRYPT();
  
  Theta(NullVector,k);

  CommonLoop (k,state,0,RC2DECRYPTSTART);


#ifdef PRINT
  printf("%lx ", state[0]);
  printf("%lx ", state[1]);
  printf("%lx ", state[2]);
  printf("%lx ", state[3]);
  printf("\n");
#endif
  END_EXPE();

  return 0;
}




