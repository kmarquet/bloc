//#define PRINT
#include <stdint.h>

typedef uint8_t  u8;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint16_t u16;
void BuildTableSCShRMCS();

#define LED 64
#define RN 32
#define WORDFILTER 0xF

#include "tools.h"

static const u8 MixColMatrix[4][4] = {
	{4,  1, 2, 2},
	{8,  6, 5, 6},
	{11,14,10, 9},
	{2,  2,15,11},
};
static const u8 sbox[16] = {12, 5, 6, 11, 9, 0, 10, 13, 3, 14, 15, 8, 4, 7, 1, 2};


static const u8 invMixColMatrix[4][4] = {
        {12,12,13,4},
        {3,8,4,5},
        {7,6,2,14},
        {13,9,9,13}
};
static const u8 invSbox[16] = {5, 14, 15, 8, 12, 1, 2, 13,11,4,6,3,0,7,9,10};


 u8 FieldMult(u8 a, u8 b)
{
	const u8 ReductionPoly = 0x3;
	u8 x = a, ret = 0;
	int i;
	for(i = 0; i < 4; i++) {
		if((b>>i)&1) ret ^= x;
		if(x&0x8) {
			x <<= 1;
			x ^= ReductionPoly;
		}
		else x <<= 1;
	}
	return ret&WORDFILTER;
}

/* half&1 == 0: use first half of the key
 * else use the second half of the key
 * 
 * The key bytes are added row wise, i.e., first row , then second row etc.
 * */
 void AddKey(u8 state[4][4], u8* keyBytes, int half)
{
	int i, j;
	if((half&1) == 0){
		for(i = 0; i < 4; i++)
			for(j = 0; j < 4; j++)
				state[i][j] ^= keyBytes[4*i+j];
	}
	else{
		for(i = 0; i < 4; i++)
			for(j = 0; j < 4; j++)
				state[i][j] ^= keyBytes[4*i+j+((LED-64)>>2)];
	}
}

 void AddConstants(u8 state[4][4], int r)
{
	const u8 RC[48] = {
		0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3E, 0x3D, 0x3B, 0x37, 0x2F,
		0x1E, 0x3C, 0x39, 0x33, 0x27, 0x0E, 0x1D, 0x3A, 0x35, 0x2B,
		0x16, 0x2C, 0x18, 0x30, 0x21, 0x02, 0x05, 0x0B, 0x17, 0x2E,
		0x1C, 0x38, 0x31, 0x23, 0x06, 0x0D, 0x1B, 0x36, 0x2D, 0x1A,
		0x34, 0x29, 0x12, 0x24, 0x08, 0x11, 0x22, 0x04
	};
	state[1][0] ^= 1;
	state[2][0] ^= 2;
	state[3][0] ^= 3;

	u8 tmp = (RC[r] >> 3) & 7;
	state[0][1] ^= tmp;
	state[2][1] ^= tmp;
	tmp =  RC[r] & 7;
	state[1][1] ^= tmp;
	state[3][1] ^= tmp;
}

u16 Table[4][1<<4];
 void BuildTableSCShRMCS()
{
	int c, v, r;
	u16 tv;
	for(v = 0; v < (1<<4); v++)
	{
		for(c = 0; c < 4; c++)
		{ // compute the entry Table[c][v]
			tv = 0;
			for(r = 0; r < 4; r++)
			{
				tv <<= 4;
				tv |= (u16) FieldMult(MixColMatrix[r][c], sbox[v]);
			}
			Table[c][v] = tv;
		}
	}
}

 void SCShRMCS(u8 state[4][4])
{
	int c,r;
	u16 v;
	u8 os[4][4];
	memcpy(os, state, 4*4);

	for(c = 0; c < 4; c++){
		v = 0;
		for(r = 0; r < 4; r++)
			v ^= Table[r][os[r][(r+c)%4]];

		for(r = 1; r <= 4; r++){
			state[4-r][c] = (u8)v & WORDFILTER;
			v >>= 4;
		}
	}
}

 void LEDRound(u8 state[4][4], u8* keyBytes)
{
	int i,j;
	AddKey(state, keyBytes, 0);
	for(i = 0; i < RN/4; i++){
		for(j = 0; j < 4; j++)
		{
			AddConstants(state, i*4+j);
			SCShRMCS(state);
		}
		AddKey(state, keyBytes, i+1);
	}
}

/************************************************************************************************/

u16 invTable[4][1<<4];
 void invBuildTableSCShRMCS()
{
	int c, v, r;
	u16 tv;
	for(v = 0; v < (1<<4); v++)
	{
		for(c = 0; c < 4; c++)
		{ // compute the entry Table[c][v]
			tv = 0;
			for(r = 0; r < 4; r++)
			{
				tv <<= 4;
				tv |= (u16) FieldMult(invMixColMatrix[r][c], v);
			}
			invTable[c][v] = tv;
		}
	}
}

 void invSCShRMCS(u8 state[4][4])
{
	int c,r;
	u16 v;
	u8 os[4][4];
	memcpy(os, state, 4*4);

	for(c = 0; c < 4; c++){
		v = 0;
		for(r = 0; r < 4; r++)
			v ^= invTable[r][os[r][c]];

		for(r = 1; r <= 4; r++){
			state[4-r][(c+(4-r))%4] = invSbox[(u8)v & WORDFILTER];
			v >>= 4;
		}
	}
}

 void invLEDRound(u8 state[4][4], u8* keyBytes)
{
	int i,j;
	for(i = (RN/4)-1; i >=0; i--)
	{
		AddKey(state, keyBytes, i+1);
		for(j = 3; j >= 0; j--)
		{
			invSCShRMCS(state);
			AddConstants(state, i*4+j);
		}
	}
	AddKey(state, keyBytes, 0);
}
	



/************************************************************************************************/
int main(int argc, char*argv[])
{
#ifdef PRINT
uart1_init();
#endif
	int i,j;

	u8 state[4][4];
	u8 keys[128/4];

	START_ENCRYPT();

	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			state[i][j] = 0;
	for(i=0; i < 32; i++) keys[i] = 0;
	
	
	BuildTableSCShRMCS();
	LEDRound(state, keys);
#ifdef PRINT
	for(i = 0; i < 4; i++){
	  for(j = 0; j < 4; j++)
	    printf("%X ",state[i][j]);
	  printf("\n");}
	
	printf("\n");
#endif
	
	START_DECRYPT();

	invBuildTableSCShRMCS();
	invLEDRound(state, keys);
#ifdef PRINT
	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++)
			printf("%X ",state[i][j]);
		printf("\n");}
		
		printf("\n");
#endif
		END_EXPE();
	return 0;
}
