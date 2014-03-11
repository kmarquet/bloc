#include <stdint.h>
#include "tools.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint64_t u64;

#define ROTATE_LEFT_48(x,bits) ( ((x) << bits) | ((x  >> (48-bits)) & 0xffffffffffff0000))
#define ROTATE_RIGHT_48(x,bits) ( ((x >> bits) & 0xffffffffffff0000) | ((x << (48-bits) )) )

void KeyExpansion ( u64 l[], u64 k[] )
{
    u8 i;
    for ( i=0 ; i<27 ; i++ )
    {
        l[i+1] = ( k[i] + ROTATE_RIGHT_48(l[i], 8) ) ^ (i<<16);
        k[i+1] = ROTATE_LEFT_48(k[i], 3) ^ l[i+1];
    }
}

void Encrypt ( u64 text[], u64 crypt[], u64 key[] )
{
    u8 i;
    crypt[0] = text[0];
    crypt[1] = text[1];

    for ( i=0 ; i<28 ; i++ )
    {
        crypt[0] = ( ROTATE_RIGHT_48(crypt[0], 8) + crypt[1] ) ^ key[i];
        crypt[1] = ROTATE_LEFT_48(crypt[1], 3) ^ crypt[0];
    }
}

void Decrypt ( u16 text[], u16 crypt[], u16 key[] )
{
    u8 i;
    crypt[0] = text[0];
    crypt[1] = text[1];

    for ( i=0 ; i<28 ; i++ )
    {
        crypt[1] = ROTATE_RIGHT_48( (crypt[0] ^ crypt[1]), 3);
        crypt[0] = ROTATE_LEFT_48( ((crypt[0] ^ key[27-i]) - crypt[1]) , 8 );
    }
}

int main ()
{
    u64 text[2];
    text[0] = 0x65776f68202c<<16;
    text[1] = 0x656761737520<<16;
    u64 crypt[2] = {0};
    u64 l[28] = {0};
    u64 k[28] = {0};
    l[0] = 0x0d0c0b0a0908<<16;
    k[0] = 0x050403020100<<16;
    
	START_ENCRYPT();
	KeyExpansion ( l, k );
    Encrypt ( text, crypt, k );
    //printf("%llx %llx\n%llx %llx\n\n\n", text[0]>>16, text[1]>>16, crypt[0]>>16, crypt[1]>>16);
	START_DECRYPT();
	KeyExpansion ( l, k );
    Decrypt ( crypt, text, k );
    //printf("%llx %llx\n%llx %llx\n\n\n", text[0], text[1], crypt[0], crypt[1]);
	END_EXPE();

    return 0;
}

