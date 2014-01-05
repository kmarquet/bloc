#include <msp430.h>
#include <stdio.h>

/**************************************************/
/******************** MEMCPY **********************/
/**************************************************/

/* inline void* */
/* memcpy(void* dest, const void* src, size_t count) */
/* { */
/*   char* dst8 = (char*)dest; */
/*   char* src8 = (char*)src; */
  
/*   while (count--) { */
/*     *dst8++ = *src8++; */
/*   } */
/*   return dest; */
/* } */


/**************************************************/
/********************* DELAY **********************/
/**************************************************/

void delay(unsigned int d) 
{
  unsigned int i,j;
  for(j=0; j < 0xff; j++)
    {
      for (i = 0; i<d; i++) 
	{
	  nop();
	  nop();
	}
    }
}

/**************************************************/
/********************* CLOCK **********************/
/**************************************************/


/**************************************************/
/********************* UART 1 *********************/
/**************************************************/
#define UART1_PIN_RX 7
#define UART1_PIN_TX 6

#define UART1_BIT_RX (1 << UART1_PIN_RX)
#define UART1_BIT_TX (1 << UART1_PIN_TX)

int uart1_init()
{

  WDTCTL = WDTPW + WDTHOLD;

  P1IE   = 0x00;        // Interrupt enable
  P2IE   = 0x00;        // 0:disable 1:enable

  //Init of MSP430 Usart1 pins
  P3SEL |= (UART1_BIT_RX | UART1_BIT_TX);

  //Init of USART1 Module
  U1ME  |= UTXE1|URXE1;           //Enable USART1 transmiter and receiver (UART mode)
  
  U1CTL  = SWRST;                 //reset
  U1CTL  = CHAR;                  //init & release reset
  
  U1TCTL = SSEL1 | TXEPT;        //use SMCLK 
  U1RCTL = 0;

  // 38400 @ SMCLK 1MHz
#define U1BR1_INIT        0
#define U1BR0_INIT        0x1B
#define U1MCTL_INIT       0x03
  
  U1BR1  = U1BR1_INIT;
  U1BR0  = U1BR0_INIT;
  U1MCTL = U1MCTL_INIT;

  delay(0x800);
  return 0;
}

int uart1_putchar(int c)
{
  U1TXBUF = c;
  while ((U1TCTL & TXEPT) != TXEPT);
  return (unsigned char)c;
}


/**************************************************/
/******************** PUT CHAR *********************/
/**************************************************/

int putchar(int c)
{
  return uart1_putchar(c);
}

/**************************************************/
/******************** END EXPE*********************/
/**************************************************/

void
start_encrypt()
{
  ;
}
void
start_decrypt()
{
  ;
}


void
end_expe()
{
  ;
}

