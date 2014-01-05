//#define PRINT

#include <stdint.h>

typedef uint32_t u32;
typedef int32_t s32;
typedef uint16_t u16;
typedef uint8_t u8;

#define NBROUND 8

#include "tools.h"


 u16 mul(u16 a,u16 b)
{
	u32 r;
	u32 A=a,B=b;

	if(A==0 && B==0) return 0x0001;
	if(A == 0)
	{
		A=0x10000;
	}
	else if(B == 0)
	{
		B=0x10000;
	}
	r = A*B;
	r%=0x10001;
	if(r == 0x10000)r=0;
	return (u16)(r&0xFFFF);
}

 u16 invMod(u16 b)
{
    s32 x=0,lastx= 1,y=1,lasty=0,quotient,temp;
    u32 A=0x10001, B=b;
    while(B != 0)
    {
        quotient = A/B;
        temp=B;
        B=A%B;
        A=temp;

        temp=x;
        x=lastx-quotient*x;
        lastx=temp;

        temp=y;
        y=lasty-quotient*y;
        lasty=temp;
	}
	if(lasty<0)
	{
		lasty+=0x10001U;
	}
    return (u16)lasty;

}
/**********************************************************************/
 void KeyScheduleEncrypt(const u16 *key,u16 *subkey)
{
	u8 i;
	u16 k[8],temp0,temp1;
	for(i=0;i<8;i++)k[i]=key[i];


	for(i=0;i<6;i++)
	{
		subkey[(i<<3)]=k[0];
		subkey[(i<<3) + 1]=k[1];
		subkey[(i<<3) + 2]=k[2];
		subkey[(i<<3) + 3]=k[3];
		subkey[(i<<3) + 4]=k[4];
		subkey[(i<<3) + 5]=k[5];
		subkey[(i<<3) + 6]=k[6];
		subkey[(i<<3) + 7]=k[7];

		temp0=k[0];
		temp1=k[1];

		k[0]=((k[1]<<9)&0xFFFF) ^ (k[2]>>7);
		k[1]=((k[2]<<9)&0xFFFF) ^ (k[3]>>7);
		k[2]=((k[3]<<9)&0xFFFF) ^ (k[4]>>7);
		k[3]=((k[4]<<9)&0xFFFF) ^ (k[5]>>7);
		k[4]=((k[5]<<9)&0xFFFF) ^ (k[6]>>7);
		k[5]=((k[6]<<9)&0xFFFF) ^ (k[7]>>7);
		k[6]=((k[7]<<9)&0xFFFF) ^ (temp0>>7);
		k[7]=((temp0<<9)&0xFFFF) ^ (temp1>>7);
	}

	subkey[48]=k[0];
	subkey[49]=k[1];
	subkey[50]=k[2];
	subkey[51]=k[3];
	return;
}

/**********************************************************************/
 void KeyScheduleDecrypt(const u16 *key,u16 *subkey)
{
	u16 i, tempkey[52];
	KeyScheduleEncrypt(key,tempkey);

	subkey[0]=invMod(tempkey[48]);
	subkey[1]=(~tempkey[49])+1;
	subkey[2]=(~tempkey[50])+1;
	subkey[3]=invMod(tempkey[51]);
	subkey[4]=tempkey[46];
	subkey[5]=tempkey[47];

	for(i=1;i<NBROUND;i++)
	{
		subkey[(6*i)+0]=invMod(tempkey[(8-i)*6+0]);
		subkey[(6*i)+1]=(~tempkey[(8-i)*6+2])+1;
		subkey[(6*i)+2]=(~tempkey[(8-i)*6+1])+1;
		subkey[(6*i)+3]=invMod(tempkey[(8-i)*6+3]);
		subkey[(6*i)+4]=tempkey[(7-i)*6+4];
		subkey[(6*i)+5]=tempkey[(7-i)*6+5];
	}

	subkey[48]=invMod(tempkey[0]);
	subkey[49]=(~tempkey[1])+1;
	subkey[50]=(~tempkey[2])+1;
	subkey[51]=invMod(tempkey[3]);

	return;
}
/**********************************************************************/


 void Encrypt(u16 *state,const u16 *subkey)
{
	u16 i,t0,t1,t2;
	for(i=0;i<NBROUND;i++)
	{
		state[0]=mul(state[0],subkey[(6*i) + 0]);
		state[1]=(state[1]+subkey[(6*i) + 1])&0xFFFF;
		state[2]=(state[2]+subkey[(6*i) + 2])&0xFFFF;
		state[3]=mul(state[3],subkey[(6*i) + 3]);


		t0=mul(subkey[(6*i) + 4],state[0] ^ state[2]);
		t1=mul(subkey[(6*i) + 5],( t0 + (state[1] ^ state[3]))&0xFFFF);
		t2=(t0+t1)&0xFFFF;

		state[0]=state[0] ^ t1 ;
		state[3]=state[3] ^ t2 ;
		t0 = t2 ^ state[1];
		state[1]=state[2]^t1;
		state[2]=t0;
	}


	t1=state[1];
	state[0]=mul(state[0],subkey[48]);
	state[1]=(state[2]+subkey[49])&0xFFFF;
	state[2]=(t1+subkey[50])&0xFFFF;
	state[3]=mul(state[3],subkey[51]);

	return;
}

/**********************************************************************/
int main()
{
#ifdef PRINT
uart1_init();
u16 i,j;
#endif
	u16 state[4]={0,1,2,3};
	u16 key[8]={1,2,3,4,5,6,7,8};
	//u16 state[4]={0xFFFF,0xFFFF,0xFFFF,0xFFFF};
	//u16 key[8]={0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF};
	//u16 state[4]={0,0,0,0};
	//u16 key[8]={0,0,0,0,0,0,0,0};
	u16 subkey[52];

	START_ENCRYPT();

	KeyScheduleEncrypt(key,subkey);
#ifdef PRINT
	printf("\n\n----------Encryption Keys----------\n\n");
	for(j=0;j<8;j++){for(i=0;i<6;i++)printf("%4X ",subkey[6*j + i]);printf("\n");}
	for(i=0;i<4;i++)printf("%4X ",subkey[48 + i]);printf("\n\n");
	printf("\n\n----------Fin Encryption Keys----------\n\n");
#endif
	Encrypt(state,subkey);
#ifdef PRINT
	printf("\n\n----------Cipher text----------\n\n");
	for(i=0;i<4;i++)printf("%4X ",state[i]);printf("\n\n");
	printf("\n\n----------Fin Cipher text----------\n\n");
#endif

	START_DECRYPT();

	KeyScheduleDecrypt(key,subkey);
#ifdef PRINT
	printf("\n\n----------Decryption Keys----------\n\n");
	for(j=0;j<8;j++){for(i=0;i<6;i++)printf("%4X ",subkey[6*j + i]);printf("\n");}
	for(i=0;i<4;i++)printf("%4X ",subkey[48 + i]);printf("\n\n");
	printf("\n\n----------Fin Decryption Keys----------\n\n");
#endif
	Encrypt(state,subkey);
#ifdef PRINT
printf("\n\n----------Plain text----------\n\n");
	for(i=0;i<4;i++)printf("%4X ",state[i]);printf("\n\n");
printf("\n\n----------Fin Plain text----------\n\n");
#endif
	END_EXPE();
	return 0;
}

