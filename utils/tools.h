#ifndef TOOLS_H
#define TOOLS_H

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
#define ROTATE_LEFT_32(x,n) ( ((x) << (n)) | ((x) >> (32-(n))) )
#define ROTATE_LEFT_64(x,n) ( ((x) << (n)) | ((x) >> (64-(n))) )

#define ROTATE_RIGHT_8(x,n) ( ((x) >> (n)) | ((x) << (8-(n))) )
#define ROTATE_RIGHT_16(x,n) ( ((x) >> (n)) | ((x) << (16-(n))) )
#define ROTATE_RIGHT_32(x,n) ( ((x) >> (n)) | ((x) << (32-(n))) )
#define ROTATE_RIGHT_64(x,n) ( ((x) >> (n)) | ((x) << (64-(n))) )


#endif

