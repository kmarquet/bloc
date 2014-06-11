#ifndef TOOLS_H
#define TOOLS_H

#define ASM_ROTATE 1

void delay(unsigned int d);
int uart1_init();
int uart1_putchar(int c);

int putchar(int c);

void end_expe();

#define END_EXPE() end_expe()
#define START_ENCRYPT() start_encrypt()
#define START_DECRYPT() start_decrypt()

#define ROTATE_LEFT_8(x,n) ( ((x) << (n)) | ((x) >> (8-(n))) )
#define ROTATE_LEFT_16(x,n) ( ((x) << (n)) | ((x) >> (16-(n))) )

#define ROTATE_RIGHT_8(x,n) ( ((x) >> (n)) | ((x) << (8-(n))) )
#define ROTATE_RIGHT_16(x,n) ( ((x) >> (n)) | ((x) << (16-(n))) )

#if ASM_ROTATE

#define ROTATE_LEFT_32(x,n) ( ((x) << (n)) | ((x) >> (32-(n))) )
#define ROTATE_LEFT_64(x,n) ( ((x) << (n)) | ((x) >> (64-(n))) )

#define ROTATE_RIGHT_32(x,n) ( ((x) >> (n)) | ((x) << (32-(n))) )
#define ROTATE_RIGHT_64(x,n) ( ((x) >> (n)) | ((x) << (64-(n))) )

#define ROTATE_RIGHT_BYTE_32(x) ROTATE_RIGHT_32(x,8)
#define ROTATE_RIGHT_BYTE_64(x) ROTATE_RIGHT_64(x,8)

#define ROTATE_LEFT_BYTE_32(x) ROTATE_LEFT_32(x,8)
#define ROTATE_LEFT_BYTE_64(x) ROTATE_LEFT_64(x,8)

#else

#include "msp430_rotate.h"

#define ROTATE_LEFT_32(x,n) {int i; for (i=0 ; i < n ; i++) x = lcs1_32(x)}
#define ROTATE_LEFT_64(x,n) {int i; for (i=0 ; i < n ; i++) x = lcs1_64(x)}

#define ROTATE_RIGHT_32(x,n)  {int i; for (i=0 ; i < n ; i++) x = rcs1_32(x)}
#define ROTATE_RIGHT_64(x,n)  {int i; for (i=0 ; i < n ; i++) x = rcs1_64(x)}

#define ROTATE_RIGHT_BYTE_32(x) rcs8_32(x)
#define ROTATE_RIGHT_8_64(x) rcs8_64(x)

#define ROTATE_LEFT_BYTE_32(x) lcs8_32(x)
#define ROTATE_LEFT_BYTE_64(x) lcs8_64(x)

#endif

#endif

