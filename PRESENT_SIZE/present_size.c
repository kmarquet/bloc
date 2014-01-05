/******************************************************************************
Written and Copyright (C) by Dirk Klose
and the EmSec Embedded Security group of Ruhr-Universitaet Bochum. 
All rights reserved.

Contact lightweight@crypto.rub.de for comments & questions.
This program is free software; You may use it or parts of it or
modifiy it under the following terms:

(1) Usage and/or redistribution and/or modification of the software 
or parts of the software is permitted for non-commercial use only.

(2a) If this software or parts are used as part of a new software, you
must license the entire work, as a whole, under this License to anyone
who comes into possession of a copy. This License will therefore
apply, to the whole of the work, and all its parts, regardless of how
they are packaged.

(2b) You may expand this license by your own license. In this case this
license still applies to the software as mentioned in (2a) and must
not be changed. The expansion must be clearly recognizable as such. In
any case of collision between the license and the expansion the
license is superior to the expansion.

(3) If this software or parts are used as part of a new software, you
must provide equivalent access to the source code of the entire work,
as a whole, to anyone who comes into possession of a copy, in the same
way through the same place at no further charge, as for the binary
version.

(4) This program is distributed in the hope that it will be useful,
but   WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
(5) These notices must be retained in any copies of any part of this
documentation and/or software.

(6) If this software is used credit must be given to the
"Embedded Security Group of Ruhr-Universitaet Bochum, Germany" as
the authors of the parts of the software used. This can be in the form
of a textual message at program startup or  at *beginning* of the
documentation (online or textual) provided with the package.

If you are interested in a commercial use 
please contact '''lightweigth@crypto.rub.de'''
******************************************************************************/
//#define PRINT

#include <stdint.h>

// Include-Dateien
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

 void Encrypt(u16 *state, u16 *aKey)
{	

// counter
	u16 round=1;
	u16 temp;
// Variables Key Scheduling
	
// Variables pLayer
	u16 temp_0;
	u16 temp_1;
	u16 temp_2;
	u16 temp_3;
	u16 j=1;
	s16 position;
	s16 reste;
	u16 key[5];

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
        temp=0;
        temp |= (u16)(sBox4[(state[0]&0xF)]);
		temp |= (u16)(sBox4[(state[0]>>=4)&0xF]<<4);
		temp |= (u16)(sBox4[(state[0]>>=4)&0xF]<<8);
		temp |= (u16)(sBox4[(state[0]>>=4)&0xF]<<12);
		state[0]=temp;
		
        temp=0;
        temp |= (u16)(sBox4[(state[1]&0xF)]);
		temp |= (u16)(sBox4[(state[1]>>=4)&0xF]<<4);
		temp |= (u16)(sBox4[(state[1]>>=4)&0xF]<<8);
		temp |= (u16)(sBox4[(state[1]>>=4)&0xF]<<12);
		state[1]=temp;
		
		temp=0;
        temp |= (u16)(sBox4[(state[2]&0xF)]);
		temp |= (u16)(sBox4[(state[2]>>=4)&0xF]<<4);
		temp |= (u16)(sBox4[(state[2]>>=4)&0xF]<<8);
		temp |= (u16)(sBox4[(state[2]>>=4)&0xF]<<12);
		state[2]=temp;
		
		temp=0;
        temp |= (u16)(sBox4[(state[3]&0xF)]);
		temp |= (u16)(sBox4[(state[3]>>=4)&0xF]<<4);
		temp |= (u16)(sBox4[(state[3]>>=4)&0xF]<<8);
		temp |= (u16)(sBox4[(state[3]>>=4)&0xF]<<12);
		state[3]=temp;
//	******************* sBox End ***************************
//	******************* pLayer *****************************
	
		position=-16;
		temp_0=0;temp_1=0;temp_2=0;temp_3=0;
		for(j=0;j<64;j++)
		{

			if(j==63)
			{
				position = 63;
			}
			else
			{		
				reste=63-position;
				if(reste>16)
				{
					position+=16;
				}
				else
				{
					position=16-reste;
				}
			}
					
			if(state[0] & 0x01)
			{
				if(position<=15)
				{temp_0|=(u16)0x01<<position;}
				else if(position<=31)
				{temp_1|=(u16)0x01<<(position&0xF);}
				else if(position<=47)
				{temp_2|=(u16)0x01<<(position&0xF);}
				else
				{temp_3|=(u16)0x01<<(position&0xF);}
			}

			state[0]>>=1;
			if(state[1]&0x0001) state[0]|=0x8000;
			
			state[1]>>=1;
			if(state[2]&0x0001) state[1]|=0x8000;
			
			state[2]>>=1;
			if(state[3]&0x0001) state[2]|=0x8000;
			
			state[3]>>=1;
		}

		state[3]=temp_3;
		state[2]=temp_2;
		state[1]=temp_1;
		state[0]=temp_0;
		
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
		temp = key[4]>>12;
		key[4] &= 0x0FFF;
		key[4] |= sBox4[temp]<<12;
		
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


 void Decrypt(u16 *state, u16 *aKey)
{	
// counter
	u16 round;
// Variables Key Scheduling
	u16 subkey[31][4];
	u16 key[5];
	u16 temp;
// Variables pLayer
	u16 j;
	u16 temp_0;
	u16 temp_1;
	u16 temp_2;
	u16 temp_3;
	s16 position;
	
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
			temp = key[4]>>12;
			key[4] &= 0x0FFF;
			key[4] |= sBox4[temp]<<12;
		
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
			
		position=-4;
		u16 reste=0;
		temp_0=0;temp_1=0;temp_2=0;temp_3=0;
		for(j=0;j<64;j++)
		{
			if(j==63)
			{
				position = 63;
			}
			else
			{		
				reste=63-position;
				if(reste>4)
				{
					position+=4;
				}
				else
				{
					position=4-reste;
				}
			}
			
			if(state[0] & 0x01)
			{
				if(position<=15)
				{temp_0|=(u16)0x01<<position;}
				else if(position<=31)
				{temp_1|=(u16)0x01<<(position&0xF);}
				else if(position<=47)
				{temp_2|=(u16)0x01<<(position&0xF);}
				else
				{temp_3|=(u16)0x01<<(position&0xF);}
			}

			state[0]>>=1;
			if(state[1]&0x0001) state[0]|=0x8000;
			
			state[1]>>=1;
			if(state[2]&0x0001) state[1]|=0x8000;
			
			state[2]>>=1;
			if(state[3]&0x0001) state[2]|=0x8000;
			
			state[3]>>=1;
		}
		state[3]=temp_3;
		state[2]=temp_2;
		state[1]=temp_1;
		state[0]=temp_0;
//	******************* pLayer End *************************
//	******************* sBox *******************************
			
		temp=0;
        temp |= (u16)(invsBox4[(state[0]&0xF)]);
		temp |= (u16)(invsBox4[(state[0]>>=4)&0xF]<<4);
		temp |= (u16)(invsBox4[(state[0]>>=4)&0xF]<<8);
		temp |= (u16)(invsBox4[(state[0]>>=4)&0xF]<<12);
		state[0]=temp;
		
        temp=0;
        temp |= (u16)(invsBox4[(state[1]&0xF)]);
		temp |= (u16)(invsBox4[(state[1]>>=4)&0xF]<<4);
		temp |= (u16)(invsBox4[(state[1]>>=4)&0xF]<<8);
		temp |= (u16)(invsBox4[(state[1]>>=4)&0xF]<<12);
		state[1]=temp;
		
		temp=0;
        temp |= (u16)(invsBox4[(state[2]&0xF)]);
		temp |= (u16)(invsBox4[(state[2]>>=4)&0xF]<<4);
		temp |= (u16)(invsBox4[(state[2]>>=4)&0xF]<<8);
		temp |= (u16)(invsBox4[(state[2]>>=4)&0xF]<<12);
		state[2]=temp;
		
		temp=0;
        temp |= (u16)(invsBox4[(state[3]&0xF)]);
		temp |= (u16)(invsBox4[(state[3]>>=4)&0xF]<<4);
		temp |= (u16)(invsBox4[(state[3]>>=4)&0xF]<<8);
		temp |= (u16)(invsBox4[(state[3]>>=4)&0xF]<<12);
		state[3]=temp;
		
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
	u16 key[5]={0x780,0x0890,0xa120,0x0140,0x0000};
	u16 state[4]={0x8700,0x0104,0x4567,0xffff};
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
