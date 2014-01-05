//#define PRINT
#include <stdint.h>

typedef uint8_t  u8;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint16_t u16;

#define LED 64
#define RN 32
#define WORDFILTER 0xF

#include "tools.h"

static const u8 invSbox[16] = {5, 14, 15, 8, 12, 1, 2, 13,11,4,6,3,0,7,9,10};


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


static const u16 Table[4][1<<4]={
{0x5ADB,0x7E1A,0xB5FC,0xA795,0x24C1,0x0,0xEF27,0x1269,0xCBE6,0xD98F,0x913D,0x6C73,0x36A8,0xFD4E,0x48B2,0x8354},
{0xCE4B,0x5D3A,0x672C,0xBF85,0x9371,0x0,0xA967,0xD8A9,0x3A16,0xE2BF,0xF45D,0x8593,0x4BD8,0x71CE,0x16E2,0x2CF4},
{0xB918,0xA246,0xCD94,0x5123,0x1B5E,0x0,0x748C,0x9CB7,0x6FD2,0xF365,0xD6CA,0x3EF1,0x87E9,0xE83B,0x25AF,0x4A7D},
{0xBE6D,0xADB1,0xC73F,0x5FC9,0x13DC,0x0,0x7952,0x98F6,0x6A8E,0xF278,0xD4E3,0x3547,0x8B2A,0xE1A4,0x269B,0x4C15}
};

static const u16 invTable[4][1<<4]={
{0x0,0xC37D,0xB6E9,0x7594,0x5CF1,0x9F8C,0xEA18,0x2965,0xABD2,0x68AF,0x1D3B,0xDE46,0xF723,0x345E,0x41CA,0x82B7},
{0x0,0xC869,0xB3C1,0x7BA8,0x56B2,0x9EDB,0xE573,0x2D1A,0xAC54,0x643D,0x1F95,0xD7FC,0xFAE6,0x328F,0x4927,0x814E},
{0x0,0xD429,0x9841,0x4C68,0x1382,0xC7AB,0x8BC3,0x5FEA,0x2634,0xF21D,0xBE75,0x6A5C,0x35B6,0xE19F,0xADF7,0x79DE},
{0x0,0x45ED,0x8AF9,0xCF14,0x37D1,0x723C,0xBD28,0xF8C5,0x6E92,0x2B7F,0xE46B,0xA186,0x5943,0x1CAE,0xD3BA,0x9657}
};

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
	for(i=0; i < 32; i++) keys[i] = i;
	

	LEDRound(state, keys);
#ifdef PRINT
	for(i = 0; i < 4; i++){
	  for(j = 0; j < 4; j++)
	    printf("%X ",state[i][j]);
	  printf("\n");}
	
	printf("\n");
#endif
	
	START_DECRYPT();
	
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
