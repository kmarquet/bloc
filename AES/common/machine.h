#ifdef MSP430

#include <io.h>
#include <iomacros.h>
#include "uart1.h"
	
int putchar( int c )
{
	return uart1_putchar( c );
}

#else

#include <time.h>

#endif
