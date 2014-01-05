//#define PRINT
#include <stdint.h>

#include"Tables_4bit.inc"

typedef uint16_t u16;
typedef int16_t s16;


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

 void Encrypt(u16 *state, const u16 *aKey)
{	
// counter
	u16 round=1;
// Variables sBox
	u16 sTemp;
// Variables Key Scheduling
	u16 key[5];
	u16 temp_0;
	u16 temp_1;
// Variables pLayer
	u16 pTemp[4];
	u16 j=1;

	key[4] = aKey[4];
	key[3] = aKey[3];
	key[2] = aKey[2];
	key[1] = aKey[1];
	key[0] = aKey[0];
	for(round=1;round<32;round++)
	{
//	****************** addRoundkey *************************
		state[3] ^= key[4];
		state[2] ^= key[3];
		state[1] ^= key[2];
		state[0] ^= key[1];
//	****************** addRoundkey End *********************
//	******************* sBox *******************************
        sTemp=0;
        sTemp |= (u16)(sBox4[(state[0]&0xF)]);
		sTemp |= (u16)(sBox4[(state[0]>>=4)&0xF]<<4);
		sTemp |= (u16)(sBox4[(state[0]>>=4)&0xF]<<8);
		sTemp |= (u16)(sBox4[(state[0]>>=4)&0xF]<<12);
		state[0]=sTemp;
		
        sTemp=0;
        sTemp |= (u16)(sBox4[(state[1]&0xF)]);
		sTemp |= (u16)(sBox4[(state[1]>>=4)&0xF]<<4);
		sTemp |= (u16)(sBox4[(state[1]>>=4)&0xF]<<8);
		sTemp |= (u16)(sBox4[(state[1]>>=4)&0xF]<<12);
		state[1]=sTemp;
		
		sTemp=0;
        sTemp |= (u16)(sBox4[(state[2]&0xF)]);
		sTemp |= (u16)(sBox4[(state[2]>>=4)&0xF]<<4);
		sTemp |= (u16)(sBox4[(state[2]>>=4)&0xF]<<8);
		sTemp |= (u16)(sBox4[(state[2]>>=4)&0xF]<<12);
		state[2]=sTemp;
		
		sTemp=0;
        sTemp |= (u16)(sBox4[(state[3]&0xF)]);
		sTemp |= (u16)(sBox4[(state[3]>>=4)&0xF]<<4);
		sTemp |= (u16)(sBox4[(state[3]>>=4)&0xF]<<8);
		sTemp |= (u16)(sBox4[(state[3]>>=4)&0xF]<<12);
		state[3]=sTemp;
//	******************* sBox End ***************************
//	******************* pLayer *****************************
	
		pTemp[0]=0;pTemp[1]=0;pTemp[2]=0;pTemp[3]=0;
		for(j=0;j<64;j++)
		{				
			if(state[j>>4] & 0x01)
			{
				pTemp[mul16mod63[j]>>4]|=(u16)0x01<<(mul16mod63[j]&0xF);
			}
			state[j>>4]>>=1;
		}

		
		state[3]=pTemp[3];
		state[2]=pTemp[2];
		state[1]=pTemp[1];
		state[0]=pTemp[0];
		
//	******************* pLayer End *************************
//	******************* Key Scheduling *********************
		// <<61 ==(rot)== >>19	
		temp_0 = key[0];
		temp_1 = key[1];
		
		key[0]=key[1]>>3;
		key[0]|=decBox3[key[2]&0x07];
		
		key[1]=key[2]>>3;
		key[1]|=decBox3[key[3]&0x07];
		
		key[2]=key[3]>>3;
		key[2]|=decBox3[key[4]&0x07];
		
		key[3]=key[4]>>3;
		key[3]|=decBox3[temp_0&0x07];
		
		key[4]=temp_0>>3;
		key[4]|=decBox3[temp_1&0x07];
		
		//sBox
		temp_0 = key[4]>>12;
		key[4] &= 0x0FFF;
		key[4] |= sBox4[temp_0]<<12;
		
		//Permutation
		if(round&0x01)key[0] ^= 0x8000;  
		key[1] ^= ( round >> 1 );
//	******************* Key Scheduling End*********************
	}
//	****************** addRoundkey *************************
 	state[3] ^= key[4];
	state[2] ^= key[3];
	state[1] ^= key[2];
	state[0] ^= key[1];

//	****************** addRoundkey End *********************
	return;
}


 void Decrypt(u16 *state, const u16 *aKey)
{	
// counter
	u16 round;
// Variables sBox
	u16 sTemp;
// Variables Key Scheduling
	u16 subkey[31][4];
	u16 key[5];
	u16 temp_0;
	u16 temp_1;
// Variables pLayer
	u16 j;
	u16 pTemp[4];
	
	key[4] = aKey[4];
	key[3] = aKey[3];
	key[2] = aKey[2];
	key[1] = aKey[1];
	key[0] = aKey[0];

//	****************** Key Scheduling **********************
		for(round=1;round<32;round++)
		{
			// <<61 ==(rot)== >>19	
			temp_0 = key[0];
			temp_1 = key[1];
		
			key[0]=key[1]>>3;
			key[0]|=decBox3[key[2]&0x07];
		
			key[1]=key[2]>>3;
			key[1]|=decBox3[key[3]&0x07];
		
			key[2]=key[3]>>3;
			key[2]|=decBox3[key[4]&0x07];
		
			key[3]=key[4]>>3;
			key[3]|=decBox3[temp_0&0x07];
		
			key[4]=temp_0>>3;
			key[4]|=decBox3[temp_1&0x07];
		
			//sBox
			temp_0 = key[4]>>12;
			key[4] &= 0x0FFF;
			key[4] |= sBox4[temp_0]<<12;
		
			//Permutation
			if(round&0x01)key[0] ^= 0x8000;  
			key[1] ^= ( round >> 1 );
			
			subkey[round-1][3] = key[4];
			subkey[round-1][2] = key[3];
			subkey[round-1][1] = key[2];
			subkey[round-1][0] = key[1];
		}
		
//	****************** Key Scheduling End ******************

		for(round=31;round>0;round--)
		{
//	****************** addRoundkey *************************
		state[3] ^= subkey[round-1][3];
		state[2] ^= subkey[round-1][2];
		state[1] ^= subkey[round-1][1];
		state[0] ^= subkey[round-1][0];
//	****************** addRoundkey End *********************
//	******************* pLayer *****************************
			
		pTemp[0]=0;pTemp[1]=0;pTemp[2]=0;pTemp[3]=0;
		for(j=0;j<64;j++)
		{
			if(state[j>>4] & 0x01)
			{
				pTemp[mul4mod63[j]>>4]|=(u16)0x01<<(mul4mod63[j]&0xF);
			}
			state[j>>4]>>=1;
		}
		state[3]=pTemp[3];
		state[2]=pTemp[2];
		state[1]=pTemp[1];
		state[0]=pTemp[0];
//	******************* pLayer End *************************
//	******************* sBox *******************************
			
		sTemp=0;
        sTemp |= (u16)(invsBox4[(state[0]&0xF)]);
		sTemp |= (u16)(invsBox4[(state[0]>>=4)&0xF]<<4);
		sTemp |= (u16)(invsBox4[(state[0]>>=4)&0xF]<<8);
		sTemp |= (u16)(invsBox4[(state[0]>>=4)&0xF]<<12);
		state[0]=sTemp;
		
        sTemp=0;
        sTemp |= (u16)(invsBox4[(state[1]&0xF)]);
		sTemp |= (u16)(invsBox4[(state[1]>>=4)&0xF]<<4);
		sTemp |= (u16)(invsBox4[(state[1]>>=4)&0xF]<<8);
		sTemp |= (u16)(invsBox4[(state[1]>>=4)&0xF]<<12);
		state[1]=sTemp;
		
		sTemp=0;
        sTemp |= (u16)(invsBox4[(state[2]&0xF)]);
		sTemp |= (u16)(invsBox4[(state[2]>>=4)&0xF]<<4);
		sTemp |= (u16)(invsBox4[(state[2]>>=4)&0xF]<<8);
		sTemp |= (u16)(invsBox4[(state[2]>>=4)&0xF]<<12);
		state[2]=sTemp;
		
		sTemp=0;
        sTemp |= (u16)(invsBox4[(state[3]&0xF)]);
		sTemp |= (u16)(invsBox4[(state[3]>>=4)&0xF]<<4);
		sTemp |= (u16)(invsBox4[(state[3]>>=4)&0xF]<<8);
		sTemp |= (u16)(invsBox4[(state[3]>>=4)&0xF]<<12);
		state[3]=sTemp;
		
//	******************* sBox End ***************************
//	****************** pLayer End **************************
		}
//	****************** addRoundkey *************************
 	state[3] ^= aKey[4];
	state[2] ^= aKey[3];
	state[1] ^= aKey[2];
	state[0] ^= aKey[1];

//	****************** addRoundkey End *********************
}



int main(void)
{
	#ifdef PRINT
	uart1_init();
#endif
// Input values
	u16 key[5]={0x0000,0x0000,0x0000,0x0000,0x0000};
	u16 state[4]={0x0000,0x0000,0x0000,0x0000};

	START_ENCRYPT();
  
	Encrypt(state,key);

	START_DECRYPT();

	Decrypt(state,key);
	
#ifdef PRINT
	ps(state);
	printf("\nFIN\n");
#endif
		
	END_EXPE();
	return 0;
}
