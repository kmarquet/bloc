#include <stdint.h>
#include "tools.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

static u8 z[62] =
{1,0,1,0,1,1,1,1,0,1,1,1,0,0,0,0,0,0,1,1,0,1,0,0,1,0,0,1,1,0,0,0,1,0,1,0,0,0,0,1,0,0,0,1,1,1,1,1,1,0,0,1,0,1,1,0,1,1,0,0,1,1};

void KeyExpansion ( u32 k[] )
{
    u8 i;
    u32 tmp;
    for ( i=3 ; i<42 ; i++ )
    {
        tmp = ROTATE_RIGHT_32(k[i-1],3);
        tmp = tmp ^ ROTATE_RIGHT_32(tmp,1);
        k[i] = ~k[i-3] ^ tmp ^ z[i-3] ^ 3;
    }
}

void Encrypt ( u32 text[], u32 crypt[], u32 key[] )
{
    u8 i;
    u32 tmp;
    crypt[0] = text[0];
    crypt[1] = text[1];

    for ( i=0 ; i<42 ; i++ )
    {
        tmp = crypt[0];
        crypt[0] = crypt[1] ^ ((ROTATE_LEFT_32(crypt[0],1)) & (ROTATE_LEFT_BYTE_32(crypt[0]))) ^ (ROTATE_LEFT_32(crypt[0],2)) ^ key[i];
        crypt[1] = tmp;
    }
}

void Decrypt ( u32 text[], u32 crypt[], u32 key[] )
{
    u8 i;
    u32 tmp;
    crypt[0] = text[0];
    crypt[1] = text[1];

    for ( i=0 ; i<42 ; i++ )
    {
        tmp = crypt[1];
        crypt[1] = crypt[0] ^ ((ROTATE_LEFT_32(crypt[1],1)) & (ROTATE_LEFT_BYTE_32(crypt[1]))) ^ (ROTATE_LEFT_32(crypt[1],2)) ^ key[41-i];
        crypt[0] = tmp;
    }
}

int main ()
{

    u32 text[2];
    text[0] = 0x6f722067;
    text[1] = 0x6e696c63;
    u32 crypt[2] = {0};
    u32 k[42];
    k[2] = 0x13121110;
    k[1] = 0x0b0a0908;
    k[0] = 0x03020100;
	
	
	START_ENCRYPT();
    KeyExpansion ( k );
    Encrypt ( text, crypt, k );
    //printf("%x %x\n%x %x\n\n\n", text[0], text[1], crypt[0], crypt[1]);
	START_DECRYPT();
    KeyExpansion ( k );
    Decrypt ( crypt, text, k );
    //printf("%x %x\n%x %x\n\n\n", text[0], text[1], crypt[0], crypt[1]);
	END_EXPE();

    return 0;
}
