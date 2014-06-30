#if REALPLATFORM
#include "msp430fg4618.h"
#else
#include <msp430.h>
#endif


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

/*********** SPECIFIC PART *************/
#if REALPLATFORM

void
rtc_init()
{
  RTCCTL = RTCHOLD + RTCMODE_2 + RTCTEV_3 + RTCIE; // SMCLK clock ; 32-bit overflow
  RTCNT1 = 0x00; // reset counter
  RTCNT2 = 0x00;
  RTCNT3 = 0x00;
  RTCNT4 = 0x00;

  //  RTCCTL |= ; // enable interrupts
}

void
rtc_start()
{
  RTCCTL &= ~RTCHOLD; // Start RTC
}

void
rtc_stop()
{
  RTCCTL |= RTCHOLD; // Stop RTC
}

void
rtc_restart()
{
  RTCNT1 = 0x00; // reset counter
  RTCNT2 = 0x00;
  RTCNT3 = 0x00;
  RTCNT4 = 0x00;
  rtc_start();
}


unsigned long int
get_rtc_count()
{  
  unsigned long int rtcnt;
  unsigned long int cnt1, cnt2, cnt3, cnt4;

  cnt1 = RTCNT1;
  cnt2 = RTCNT2;
  cnt3 = RTCNT3;
  cnt4 = RTCNT4;

  rtcnt = (cnt4 << 24) + (cnt3 << 16) + (cnt2 << 8) + cnt1;
  
  return rtcnt;
}

#endif


/**************************************************/
/******************** END EXPE*********************/
/**************************************************/

void
start_encrypt()
{
#if REALPLATFORM
  rtc_init();
  rtc_start();
#endif
}

void
start_decrypt()
{
#if REALPLATFORM
  rtc_stop();
  unsigned long int cnt = get_rtc_count();
  rtc_restart();
#endif
}


void
end_expe()
{
#if REALPLATFORM
  rtc_stop();
  unsigned long int cnt = get_rtc_count();
  rtc_restart();
#endif
}
