#include <stdint.h>
#include "tools.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint64_t u64;

static u8 z[62] =
{1,0,1,0,1,1,1,1,0,1,1,1,0,0,0,0,0,0,1,1,0,1,0,0,1,0,0,1,1,0,0,0,1,0,1,0,0,0,0,1,0,0,0,1,1,1,1,1,1,0,0,1,0,1,1,0,1,1,0,0,1,1};

#define ROTATE_LEFT_48(x,bits) ( (x << bits) | ((x & 0xffffffffffff) >> (48-bits)) )
#define ROTATE_RIGHT_48(x,bits) ( ((x & 0xffffffffffff) >> bits) | (x << (48-bits)) )

void KeyExpansion ( u64 k[] )
{
    u8 i;
    u64 tmp = 0;
    for ( i=2 ; i<52 ; i++ )
    {
        tmp = ROTATE_RIGHT_48(k[i-1],3);
        tmp = tmp ^ ROTATE_RIGHT_48(tmp,1);
        k[i] = (~k[i-2] ^ tmp ^ z[(i-2)] ^ 3);
    }
}

void Encrypt ( u64 text[], u64 crypt[], u64 key[] )
{
    u8 i;
    u64 tmp;
    crypt[0] = text[0];
    crypt[1] = text[1];

    for ( i=0 ; i<52 ; i++ )
    {
        tmp = crypt[0];
        crypt[0] = crypt[1] ^ ((ROTATE_LEFT_48(crypt[0],1)) & (ROTATE_LEFT_48(crypt[0],8))) ^ (ROTATE_LEFT_48(crypt[0],2)) ^ key[i];
        crypt[1] = tmp;
    }
}

void Decrypt ( u64 text[], u64 crypt[], u64 key[] )
{
    u8 i;
    u64 tmp;
    crypt[0] = text[0];
    crypt[1] = text[1];

    for ( i=0 ; i<52 ; i++ )
    {
        tmp = crypt[1];
        crypt[1] = crypt[0] ^ ((ROTATE_LEFT_48(crypt[1],1)) & (ROTATE_LEFT_48(crypt[1],8))) ^ (ROTATE_LEFT_48(crypt[1],2)) ^ key[51-i];
        crypt[0] = tmp;
    }
}

int main ()
{

    u64 text[2];
    text[0] = 0x2072616c6c69;
    text[1] = 0x702065687420;
    u64 crypt[2] = {0};
    u64 k[52];
    k[1] = 0x0d0c0b0a0908;
    k[0] = 0x050403020100;
	
	START_ENCRYPT();
	KeyExpansion ( k );
    Encrypt ( text, crypt, k );
    //printf("%llx %llx\n%llx %llx\n\n\n", text[0], text[1], crypt[0]&0xffffffffffff, crypt[1]&0xffffffffffff);
    
	START_DECRYPT();
	KeyExpansion ( k );
	Decrypt ( crypt, text, k );
    //printf("%llx %llx\n%llx %llx\n\n\n", text[0], text[1], crypt[0]&0xffffffffffff, crypt[1]&0xffffffffffff);
	END_EXPE();
    return 0;
}
