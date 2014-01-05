//#define PRINT

#include "tools.h"

#include <stdint.h>
typedef uint16_t u16;

#define SIZE 96//n : plaintext size, key size. k*6B
#define B 16 //processor (or word) size.
#define NB (SIZE/(2*B)) //nb = n/2b : number of words per Feistel branch.
#define NBROUND 51 // odd number

#define MASK 0xFFFF

/**********************************************************************/
 void XOR(u16 x[NB],u16 y[NB])
{
	u16 i;
	for(i=0;i<NB;i++)
	{
		x[i]^=y[i];
	}
	return;
}

 void Sub(u16 x[NB])
{
	u16 i;
	for(i=0;i<(SIZE/(6*B));i++)
	{
		x[3*i]   ^= x[3*i+1] & x[3*i+2];
		x[3*i+1] ^= x[3*i]   & x[3*i+2];
		x[3*i+2] ^= x[3*i+1] | x[3*i];
	}
	return;
}

 void WordRot(u16 x[NB])
{
	u16 i;
	u16 temp=x[NB-1];
	for(i=NB-1;i>0;i--)
	{
		x[i]=x[i-1];
	}
	x[0]=temp;
	return;
}

 void InvWordRot(u16 x[NB])
{
	u16 i;
	u16 temp=x[0];
	for(i=0;i<NB-1;i++)
	{
		x[i]=x[i+1];
	}
	x[NB-1]=temp;
	return;
}

 void BitRot(u16 x[NB])
{
	u16 i;
	for(i=0;i<NB/3;i++)
	{
		x[3*i]=(x[3*i]>>1)^(x[3*i]<<(B-1));
		x[3*i+2]=(x[3*i+2]<<1)^(x[3*i+2]>>(B-1));
	}
	return;
}

 void Add(u16 x[NB],u16 y[NB])
{
	u16 i;
	for(i=0;i<NB;i++)
	{
		x[i]=(x[i]+y[i])&MASK;
	}
	return;
}

/**********************************************************************/
 void fk(u16 kr[NB],u16 kl[NB],u16 krDest[NB],u16 klDest[NB],u16 c[NB])
{
	u16 i;
	for(i=0;i<NB;i++) krDest[i]=kr[i];
	for(i=0;i<NB;i++) klDest[i]=kr[i];

	Add(krDest,c);
	Sub(krDest);
	BitRot(krDest);
	WordRot(krDest);
	XOR(krDest,kl);
	return;
}

 void fe(u16 r[NB],u16 l[NB],u16 k[NB])
{
	u16 temp[NB],i;
	for(i=0;i<NB;i++) temp[i]=r[i];

	Add(r,k);
	Sub(r);
	BitRot(r);
	WordRot(l);
	XOR(r,l);

	for(i=0;i<NB;i++) l[i]=temp[i];

	return;
}

 void fd(u16 r[NB],u16 l[NB],u16 k[NB])
{
	u16 temp[NB],i;
	for(i=0;i<NB;i++) temp[i]=r[i];

	Add(r,k);
	Sub(r);
	BitRot(r);
	XOR(r,l);
	InvWordRot(r);
	for(i=0;i<NB;i++) l[i]=temp[i];

	return;
}

 void KeySchedul(u16 mkey[2*NB],u16 rkey[NBROUND][2*NB])
{
	u16 i,j,temp,c[NB];
	for(i=1;i<NB;i++) c[i]=0;
	for(i=0;i<2*NB;i++) rkey[0][i]=mkey[i];
	for(i=1;i<=(NBROUND>>2);i++)
	{
		c[0]=i;
		//[KLi , KRi ] = FK (KLi−1 , KRi−1 , C(i));
		fk(rkey[i-1],rkey[i-1]+3,rkey[i],rkey[i]+3,c);
	}
	
	for(j=0;j<NB;j++)
	{
		temp=rkey[NBROUND>>2][j];
		rkey[NBROUND>>2][j]=rkey[NBROUND>>2][j+3];
		rkey[NBROUND>>2][j+3]=temp;
	}
	
	for(;i<NBROUND;i++)
	{
		c[0]=NBROUND-i;
		//[KLi , KRi ] = FK (KLi−1 , KRi−1 , C(r − i));
		fk(rkey[i-1],rkey[i-1]+3,rkey[i],rkey[i]+3,c);
	}
	return;
}

 void Decrypt(u16 state[2*NB],u16 rkey[NBROUND][2*NB])
{
	u16 i,temp;
	
	for(i=NBROUND;i>((NBROUND+1)>>2);i--)
	{
		fd(state,state+3,rkey[i-1]+3);
	}
	for(;i>=1;i--)
	{
		fd(state,state+3,rkey[i-1]);
	}
	for(i=0;i<NB;i++)
	{
		temp=state[i];
		state[i]=state[i+3];
		state[i+3]=temp;
	}

	return;
}

 void Encrypt(u16 state[2*NB],u16 rkey[NBROUND][2*NB])
{
	u16 i,temp;
	for(i=1;i<=((NBROUND+1)>>2);i++)
	{
		fe(state,state+3,rkey[i-1]);	
	}
	for(;i<=NBROUND;i++)
	{
		fe(state,state+3,rkey[i-1]+3);		
	}
		
	for(i=0;i<NB;i++)
	{
		temp=state[i];
		state[i]=state[i+3];
		state[i+3]=temp;
	}	
	return;
}


int main()
{
#ifdef PRINT
	uart1_init();
	u16 j;
#endif
	u16 i;
	u16 state[2*NB];
	u16 mkey[2*NB];
	u16 rkey[NBROUND][2*NB];
	
	
	for(i=0;i<2*NB;i++) state[i]=i;
	for(i=0;i<2*NB;i++) mkey[i]=2*i;

#ifdef PRINT
	printf("----------Depart----------\n\n");
	for(j=1;j<=2*NB;j++) printf("%4X ",state[2*NB - j]);printf("\n\n");
	printf("----------Fin Depart----------\n\n");
	printf("----------Encryption----------\n\n");
#endif

	START_ENCRYPT();
	KeySchedul(mkey,rkey);
	Encrypt(state,rkey);
	
#ifdef PRINT
	for(j=1;j<=2*NB;j++) printf("%4X ",state[2*NB - j]);printf("\n\n");
	printf("----------Fin Encryption----------\n\n");
	printf("----------Decryption----------\n\n");
#endif
	START_DECRYPT();
	KeySchedul(mkey,rkey);
	Decrypt(state,rkey);
#ifdef PRINT
	for(j=1;j<=2*NB;j++) printf("%4X ",state[2*NB - j]);printf("\n\n");
	printf("----------Fin Decryption----------\n\n");
#endif
	END_EXPE();
	return 0;
}
