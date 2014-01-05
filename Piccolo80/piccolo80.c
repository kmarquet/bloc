//#define PRINT
#include <stdint.h>

typedef uint16_t u16;
typedef int16_t s16;
typedef uint8_t u8;

#include "tools.h"

#ifdef PRINT
void ps(u16 *state)
{
		printf("%x ", state[3]);
		printf("%x ", state[2]);
		printf("%x ", state[1]);
		printf("%x\n\n", state[0]);
		return;
}
#endif

static const u16 sBox[] = {0xe,0x4,0xb,0x2,0x3,0x8,0x0,0x9,0x1,0xa,0x7,0xf,0x6,0xc,0x5,0xd};
static const u16 mul2[] = {0x0,0x2,0x4,0x6,0x8,0xa,0xc,0xe,0x3,0x1,0x7,0x5,0xb,0x9,0xf,0xd};
static const u16 mul3[] = {0x0,0x3,0x6,0x5,0xc,0xf,0xa,0x9,0xb,0x8,0xd,0xe,0x7,0x4,0x1,0x2};
#define NBROUND 25

void Gr(u16 *state, const u16 *wk, const u16 *rk);
void Gr_1(u16 *state, const u16 *wk, const u16 *rk);
void RoundPermutation(u16 *chaine);
void wKS_80(const u16 *k, u16 *wkDest);
void rKS_80(const u16 *k, u16 *rkDest);
u16 FonctionF(u16 subState);

int main(int argc, char* argv[])
{
	// ************ Déclarations ************
	u16 text[4]={0xcdef,0x89ab,0x4567,0x0123};
	u16 k80[5] = {0x0011, 0x2233, 0x4455, 0x6677, 0x8899};
	u16 wk[4];
	u16 rk[2*NBROUND];

	
	// ****************************************
	//			Test clé 80 bits
	// ****************************************
	START_ENCRYPT();
	wKS_80(k80,wk);
	rKS_80(k80,rk);

	// ************ Vérification chiffrement ************

	Gr(text, wk, rk);
#ifdef PRINT
	printf("Verification avec la cle de 80 bits \n\nLe chiffrement donne : \n");
	printf("8d2b ff99 35f8 4056\n");
	ps(text);
#endif
	// ************ Controle du déchiffrement ************
	START_DECRYPT();
	wKS_80(k80,wk);
	rKS_80(k80,rk);
	Gr_1(text, wk, rk);
#ifdef PRINT
	printf("Controle du dechiffrement : \n\n");
	ps(text);
#endif
	END_EXPE();
	return 0;
}

// Fonction de chiffrement
void Gr(u16 *state, const u16 *wk, const u16 *rk)
{
	u16 round;
	// Premier Wk 
	state[3] ^= wk[0];
	state[1] ^= wk[1];
	for(round = 0; round <= (NBROUND - 2); round++)
	{
		state[2] ^= (FonctionF(state[3]) ^ rk[2 * round]);
		state[0] ^= (FonctionF(state[1]) ^ rk[2 * round + 1]);

		RoundPermutation(state);
	}
	state[2] ^= (FonctionF(state[3]) ^ rk[2 * NBROUND - 2]);
	state[0] ^= (FonctionF(state[1]) ^ rk[2 * NBROUND - 1]);
	
	// Deuxième Wk
	state[3] ^= wk[2];
	state[1] ^= wk[3];
	
	return;
}

// Fonction de déchiffrement
void Gr_1(u16 *state, const u16 *wk, const u16 *rk)
{
  u16 wk_1[4];
  u16 rk_1[2*NBROUND];
  
  u16 i;
  
  wk_1[0] = wk[2];
  wk_1[1] = wk[3];
  wk_1[2] = wk[0];
  wk_1[3] = wk[1];

  for(i=0; i<NBROUND; i++)
  {
    rk_1[2*i] = rk[2*NBROUND - 2*i - 2 + (i%2)];
    rk_1[2*i+1] = rk[2*NBROUND - 2*i -1 - (i%2)];
  }

  Gr(state,wk_1,rk_1);

  return;
}

void RoundPermutation(u16 *chaine)
{
	u16 temp_0 = chaine[0];
	u16 temp_2 = chaine[2];
	
	chaine[0]= (chaine[3] & 0xFF00) | (chaine[1] & 0x00FF);
	chaine[2]= (chaine[1] & 0xFF00) | (chaine[3] & 0x00FF);
	
	chaine[1]= (temp_0 & 0xFF00) | (temp_2 & 0x00FF);
	chaine[3]= (temp_2 & 0xFF00) | (temp_0 & 0x00FF);

	return;
}

void wKS_80(const u16 *k, u16 *wkDest) 
{
  wkDest[0] = (k[1] & 0x00FF) | (k[0] & 0xFF00);
  wkDest[1] = (k[0] & 0x00FF) | (k[1] & 0xFF00);
  wkDest[2] = (k[3] & 0x00FF) | (k[4] & 0xFF00);
  wkDest[3] = (k[4] & 0x00FF) | (k[3] & 0xFF00);
  return;
}

void rKS_80(const u16 *k, u16 *rkDest)
{
  u16 i;
  u16 tmp;
  u16 con[2];
  const u8 rkEvenBox[5]={2,0,2,4,0};
  const u8 rkOddBox[5]={3,1,3,4,1};
  u8 mod;

  for (i=0; i<NBROUND; i++)
  {
    tmp = ((i + 1) << 10) | (i + 1);
    con[0]= tmp ^ 0x2D3C;
    con[1]= (tmp<<1) ^ 0x0F1E;
	
    rkDest[i<<1] = con[1];
    rkDest[(i<<1)+1] = con[0];

	mod=i%5;
    rkDest[2*i] ^= k[rkEvenBox[mod]];
    rkDest[2*i+1] ^= k[rkOddBox[mod]];
  }
  return;
}

u16 FonctionF(u16 subState)
{
	u16 subState0=sBox[subState&0xF];
	u16 subState1=sBox[(subState>>=4)&0xF];
	u16 subState2=sBox[(subState>>=4)&0xF];
	u16 subState3=sBox[(subState>>=4)&0xF];
	
	u16 temp_0=mul3[subState3]^subState2^subState1^mul2[subState0];
	u16 temp_1=subState3^subState2^mul2[subState1]^mul3[subState0];
	u16 temp_2=subState3^mul2[subState2]^mul3[subState1]^subState0;
	u16 temp_3=mul2[subState3]^mul3[subState2]^subState1^subState0;
	
	subState=sBox[temp_3];
	subState<<=4;
	subState^=sBox[temp_2];
	subState<<=4;
	subState^=sBox[temp_1];
	subState<<=4;
	subState^=sBox[temp_0];

	return subState;
}
