//#define PRINT
#define INLINE

#include <stdint.h>

typedef int8_t s8;
typedef uint8_t u8;
typedef uint16_t u16;

#include "tools.h"

static const u8 sBox[16] =
{0x7, 0x4, 0xa, 0x9, 0x1, 0xf, 0xb, 0x0,
0xc, 0x3, 0x2, 0x6, 0x8, 0xe, 0xd, 0x5};


#ifdef INLINE
#define L(x) ( ((x) << 1) ^ (0x1B & (u8)((s8)(x) >> 7)) )
#else
 u8 L (u8 x)
{
    u8 h = (u8)((s8)x >> 7);
    x = x << 1;
    x ^= 0x1B & h;
}
#endif

 void KeySetup ( u8 k[], u8 r )
{
    u8 i;

    u8 tmp[10] =
    {k[1],k[2],k[3],k[4],k[0],
    k[6],k[7],k[8],k[9],k[5]};

    for (i=0 ; i<5 ; i++)
    {
        k[i] = tmp[i+5];
        k[i+5] = tmp[i] ^ tmp[i+5];
    }

    k[2] = k[2] ^ r;
    k[6] = (sBox[k[6]&0x0f]) | (sBox[k[6]>>4] << 4);
    k[7] = (sBox[k[7]&0x0f]) | (sBox[k[7]>>4] << 4);
}

 void InvKeySetup ( u8 k[], u8 r )
{
    u8 i;
    u8 tmp[10];

    k[2] = k[2] ^ r;
    k[6] = (sBox[k[6]&0x0f]) | (sBox[k[6]>>4] << 4);
    k[7] = (sBox[k[7]&0x0f]) | (sBox[k[7]>>4] << 4);

    tmp[0] = k[4]; tmp[1] = k[0]; tmp[2] = k[1]; tmp[3] = k[2]; tmp[4] = k[3];
    tmp[5] = k[9]; tmp[6] = k[5]; tmp[7] = k[6]; tmp[8] = k[7]; tmp[9] = k[8];

    for (i=0 ; i<5 ; i++)
    {
        k[i] = tmp[i] ^ tmp[i+5];
        k[i+5] = tmp[i];
    }
}


 void Encrypt ( u8 text[], u8 crypt[], u8 key[] )
{
    u8 i;
    for (i=0 ; i<8 ; i++)
    {
        crypt[i] = text[i];
    }

    u8 t[8];
    u8 r;
    for (r=1 ; r<17 ; r++)
    {

        for (i=0 ; i<8 ; i++)
        {
            crypt[i] = crypt[i] ^ key[i];
        }

        for (i=0 ; i<8 ; i++)
        {
            crypt[i] = (sBox[crypt[i]&0xf]) | (sBox[crypt[i]>>4] << 4);
        }

        for (i=0 ; i<8 ; i++)
        {
            t[(i+6)&0x7] = crypt[i];

        }

        crypt[0] = L(t[0] ^ t[1]) ^ t[1] ^ t[2] ^ t[3];
        crypt[1] = L(t[1] ^ t[2]) ^ t[0] ^ t[2] ^ t[3];
        crypt[2] = L(t[2] ^ t[3]) ^ t[0] ^ t[1] ^ t[3];
        crypt[3] = L(t[3] ^ t[0]) ^ t[0] ^ t[1] ^ t[2];
        crypt[4] = L(t[4] ^ t[5]) ^ t[5] ^ t[6] ^ t[7];
        crypt[5] = L(t[5] ^ t[6]) ^ t[4] ^ t[6] ^ t[7];
        crypt[6] = L(t[6] ^ t[7]) ^ t[4] ^ t[5] ^ t[7];
        crypt[7] = L(t[7] ^ t[4]) ^ t[4] ^ t[5] ^ t[6];

        KeySetup(key,r);
    }

    for (i=0 ; i<8 ; i++)
    {
        crypt[i] = crypt[i] ^ key[i];
    }

}


 void Decrypt ( u8 text[], u8 crypt[], u8 key[] )
{
    u8 i;
    for (i=0 ; i<8 ; i++)
    {
        crypt[i] = text[i];
    }

    for (i=0 ; i<8 ; i++)
    {
        crypt[i] = crypt[i] ^ key[i];
    }

    u8 t[8];
    u8 r;
    for (r=16 ; r>0 ; r--)
    {
        u8 var1 = crypt[3]^crypt[1]^crypt[2]^crypt[0];
        u8 var2 = crypt[7]^crypt[6]^crypt[5]^crypt[4];

        t[0] = L(L(L(var1)^crypt[2]^crypt[0])^crypt[1]^crypt[0])^crypt[3]^crypt[1]^crypt[2];
        t[1] = L(L(L(var1)^crypt[3]^crypt[1])^crypt[2]^crypt[1])^crypt[0]^crypt[2]^crypt[3];
        t[2] = L(L(L(var1)^crypt[0]^crypt[2])^crypt[3]^crypt[2])^crypt[1]^crypt[3]^crypt[0];
        t[3] = L(L(L(var1)^crypt[1]^crypt[3])^crypt[0]^crypt[3])^crypt[2]^crypt[0]^crypt[1];

        t[4] = L(L(L(var2)^crypt[6]^crypt[4])^crypt[5]^crypt[4])^crypt[7]^crypt[5]^crypt[6];
        t[5] = L(L(L(var2)^crypt[7]^crypt[5])^crypt[6]^crypt[5])^crypt[4]^crypt[6]^crypt[7];
        t[6] = L(L(L(var2)^crypt[4]^crypt[6])^crypt[7]^crypt[6])^crypt[5]^crypt[7]^crypt[4];
        t[7] = L(L(L(var2)^crypt[5]^crypt[7])^crypt[4]^crypt[7])^crypt[6]^crypt[4]^crypt[5];

        for (i=0 ; i<8 ; i++)
        {
            crypt[i] = t[(i+6)&0x7];
        }

        for (i=0 ; i<8 ; i++)
        {
            crypt[i] = (sBox[crypt[i]&0xf]) | (sBox[crypt[i]>>4] << 4);
        }

        InvKeySetup(key,r);

        for (i=0 ; i<8 ; i++)
        {
            crypt[i] = crypt[i] ^ key[i];
        }

    }

}


int main ()
{
	#ifdef PRINT
	uart1_init();
	u8 i;
	#endif

    u8 text[8] = {0};
    u8 crypt[8] = {0};
    u8 k[10] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

	START_ENCRYPT();
    Encrypt ( text, crypt, k );
	START_DECRYPT();
    Decrypt (crypt, text, k );
	END_EXPE();

	#ifdef PRINT
	printf("Ciphered :\n");
    for (i=0 ; i<8 ; i++)
    {
        printf("%x ",crypt[i]);
    }
    printf("\n\nDeciphered :\n");
    for (i=0 ; i<8 ; i++)
    {
        printf("%x ",text[i]);
    }
	#endif

    return 0;
}

