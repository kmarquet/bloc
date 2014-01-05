//#define PRINT
#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#include "tools.h"

static const unsigned short sBox4[] = {
  0x4, 0xF, 0x3, 0x8, 0xD, 0xA, 0xC, 0x0, 0xB, 0x5, 0x7, 0xE, 0x2, 0x6, 0x1, 0x9
};

static const unsigned short sBox4Inv[] = {
  0x7, 0xE, 0xC, 0x2, 0x0, 0x9, 0xD, 0xA, 0x3, 0xF, 0x5, 0x8, 0x6, 0x4, 0xB, 0x1
};


void pks(u8 *state)
{
#ifdef PRINT
		printf("%x ", state[0]);
		printf("%x ", state[1]);
		printf("%x ", state[2]);
		printf("%x ", state[3]);
		printf("%x ", state[4]);
		printf("%x ", state[5]);
		printf("%x ", state[6]);
  		printf("%x\n\n", state[7]);
		return;
#else
		;
#endif
}


void
AddRoundKey(u8* state, u8* key)
{
  //	****************** addRoundkey *************************
  state[3] ^= key[9];
  state[2] ^= key[8];
  state[1] ^= key[7];
  state[0] ^= key[6];
  //	****************** addRoundkey End *********************
}

void
Substitute(u8* state)
{
	u8 temp_sBox;

	//	******************* sBox *******************************
	temp_sBox = 0;
	temp_sBox |= sBox4[(state[0] & 0xF)];
	temp_sBox |= (sBox4[(state[0]>>4)]) << 4;
	state[0]=temp_sBox;

	temp_sBox = 0;
	temp_sBox |= sBox4[(state[1] & 0xF)];
	temp_sBox |= (sBox4[(state[1]>>4)]) << 4;
	state[1]=temp_sBox;

	temp_sBox = 0;
	temp_sBox |= sBox4[(state[2] & 0xF)];
	temp_sBox |= (sBox4[(state[2]>>4)]) << 4;
	state[2]=temp_sBox;

	temp_sBox = 0;
	temp_sBox |= sBox4[(state[3] & 0xF)];
	temp_sBox |= (sBox4[(state[3]>>4)]) << 4;
	state[3]=temp_sBox;
	//	******************* sBox End ***************************
}

void
Mix(u8* state)
{
  u8 n7 = state[0] & 0xF;
  u8 n6 = state[0] >> 4;
  u8 n5 = state[1] & 0xF;
  u8 n4 = state[1] >> 4;
  u8 n3 = state[2] & 0xF;
  u8 n2 = state[2] >> 4;
  u8 n1 = state[3] & 0xF;
  u8 n0 = state[3] >> 4;

  state[3] = ((n1 ^ n2 ^ n3 ^ n4 ^ n5 ^ n6) <<  4) | (n0 ^ n2 ^ n3 ^ n5 ^ n6 ^ n7);
  state[2] = ((n0 ^ n1 ^ n3 ^ n4 ^ n6 ^ n7) << 4) | (n0 ^ n1 ^ n2 ^ n4 ^ n5 ^ n7);
  state[1] = ((n0 ^ n1 ^ n3 ^ n4 ^ n5) << 4) | (n0 ^ n1 ^ n2 ^ n5 ^ n6);
  state[0] = ((n1 ^ n2 ^ n3 ^ n6 ^ n7) << 4) | (n0 ^ n2 ^ n3 ^ n4 ^ n7);
}

void
Permute(u8* state)
{
  u8 n7 = state[0] & 0xF;
  u8 n6 = state[0] >> 4;
  u8 n5 = state[1] & 0xF;
  u8 n4 = state[1] >> 4;
  u8 n3 = state[2] & 0xF;
  u8 n2 = state[2] >> 4;
  u8 n1 = state[3] & 0xF;
  u8 n0 = state[3] >> 4;

  state[3] = (n2 << 4) | n0;
  state[2] = (n3 << 4) | n6;
  state[1] = (n7 << 4) | n4;
  state[0] = (n5 << 4) | n1;
}

void
KeySchedule(u8* key, u8 round)
{
  //ROTATE RIGHT 16 bits
  u8 tmp = key[9];
  u8 tmp2 = key[8];

  key[8] = key[0];
  key[9] = key[1];
  key[0] = key[2];
  key[1] = key[3];
  key[2] = key[4];
  key[3] = key[5];
  key[4] = key[6];
  key[5] = key[7];
  key[6] = tmp2;
  key[7] = tmp;

  //ROTATE RIGHT 3 bits
  tmp = key[0] << 5;
  key[0] = (key[0] >> 3) | (key[1] << 5);
  key[1] = (key[1] >> 3) | (key[2] << 5);
  key[2] = (key[2] >> 3) | (key[3] << 5);
  key[3] = (key[3] >> 3) | (key[4] << 5);
  key[4] = (key[4] >> 3) | (key[5] << 5);
  key[5] = (key[5] >> 3) | (key[6] << 5);
  key[6] = (key[6] >> 3) | (key[7] << 5);
  key[7] = (key[7] >> 3) | (key[8] << 5);
  key[8] = (key[8] >> 3) | (key[9] << 5);
  key[9] = (key[9] >> 3) | tmp;



  key[9] = (sBox4[(key[9] >> 4)] << 4) | sBox4[(key[9] & 0xF)];
  key[2] = key[2] ^ ((round >> 2) & 0x7);
  key[1] = key[1] ^ ((round & 0x3) << 6);
}

void
Encrypt(u8* state, u8* key)
{
	u8 round;

	for(round=1; round <= 32; round++)
	  {
	    u32 state_L = *((u32*) (state + 4));
	    u32 state_R = *((u32*) state);

	    KeySchedule(key, round);

	    AddRoundKey(state + 4, key);
	    Substitute(state + 4);
	    Mix(state + 4);
	    Permute(state + 4);

	    *((u32*) (state + 4)) = (*((u32*) (state + 4))) ^ state_R;
	    *((u32*) state) = state_L;
	  }

	return;
}


void
InvKeySchedule(u8* key, u8 round)
{
  key[1] = key[1] ^ ((round & 0x3) << 6);
  key[2] = key[2] ^ ((round >> 2) & 0x7);
  key[9] = (sBox4Inv[(key[9] >> 4)] << 4) | sBox4Inv[(key[9] & 0xF)];

  //ROTATE LEFT 16 bits
  u8 tmp = key[9];
  u8 tmp2 = key[8];

  key[9] = key[7];
  key[8] = key[6];
  key[7] = key[5];
  key[6] = key[4];
  key[5] = key[3];
  key[4] = key[2];
  key[3] = key[1];
  key[2] = key[0];
  key[1] = tmp;
  key[0] = tmp2;

  //ROTATE LEFT 3 bits
  tmp = key[9] >> 5;
  key[9] = (key[9] << 3) | (key[8] >> 5);
  key[8] = (key[8] << 3) | (key[7] >> 5);
  key[7] = (key[7] << 3) | (key[6] >> 5);
  key[6] = (key[6] << 3) | (key[5] >> 5);
  key[5] = (key[5] << 3) | (key[4] >> 5);
  key[4] = (key[4] << 3) | (key[3] >> 5);
  key[3] = (key[3] << 3) | (key[2] >> 5);
  key[2] = (key[2] << 3) | (key[1] >> 5);
  key[1] = (key[1] << 3) | (key[0] >> 5);
  key[0] = (key[0] << 3) | (tmp);

}

void Decrypt(u8 *state, u8 *key)
{
	int round;

	for(round=32; round >= 1; round--)
	  {
	    u32 state_L = *((u32*) (state + 4));
	    u32 state_R = *((u32*) state);

	    AddRoundKey(state, key);
	    Substitute(state);
	    Mix(state);
	    Permute(state);

	    *((u32*) state) = (*((u32*) state)) ^ state_L;
	    *((u32*) (state + 4)) = state_R;

	    InvKeySchedule(key, round);
	  }

	return;
}



int main(void)
{
#ifdef PRINT
  uart1_init();
#endif
  // Input values
  u8 state[8]={0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  //u8 state[8]={0x00};

  u8 key[10]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
  //u8 key[10]={0};

  START_ENCRYPT();
  Encrypt(state, key);
#ifdef PRINT
  printf("\n\nCiphered text :\n");
  pks(state);
#endif
  START_DECRYPT();
  Decrypt(state, key);
#ifdef PRINT
  printf("Deciphered text :\n");
  pks(state);
#endif
  END_EXPE();

  return 0;
}
