#include "tools.h"
#include "msp430fg4618.h"

/*tean values
These are made by starting with a vector of 6 zeroes,
data followed by key, and coding with one cycle then 
moving the six cyclically so that n becomes n-1 modulo 6. 

We repeat with 2-64 cycles printing at powers of 2 in 
hexadecimal.  The process is reversed decoding back 
to the original zeroes which are printed.

  1        0 9e3779b9        0        0         0        0
  2 ec01a1de aaa0256d        0        0         0        0
  4 bc3a7de2 4e238eb9        0        0  ec01a1de 114f6d74
  8 31c5fa6c 241756d6 bc3a7de2 845846cf  2794a127 6b8ea8b8
 16 1d8e6992 9a478905 6a1d78c8  8c86d67  2a65bfbe b4bd6e46
 32 d26428af  a202283 27f917b1 c1da8993  60e2acaa a6eb923d
 64 7a01cbc9 b03d6068 62ee209f  69b7afc  376a8936 cdc9e923
  1        0        0        0        0         0        0

tea does the same but the number of iterations
is constant at 32

  1 41ea3a0a 94baa940        0        0         0        0
  2 4e8e7829 7d8236d8        0        0         0 41ea3a0a
  4 b84e28af b6b62088        0 41ea3a0a  4e8e7829 c88ba95e
  8 5ddf75d9 7a4ce68f b84e28af a0a47295  ed650698 1024eea0
 16 4ec5d2e2 5ada1d89 a9c3801a 32a1e654   8b63bb9 21410574
 32 7d2c6c57 7a6adb4d 69c53e0f 60388ada  df70a1f5 d9cb4e09
 64 2bb0f1b3 c023ed11 5c60bff2 7072d01c  4513c5eb 8f3a38ab
  1        0        0        0        0         0        0


The program used follows.

................................................................
*/

/* make test vectors for tea, xtea */

void code(long *v, long *k);

void decode(long *v, long *k);

void
tean(long *v, long *k, long ncycles);

int
main(long argc, long *argv[])
{
  static long pz[1024], n;
  
  for (n = 1; n < 65; n++) {
    tean(pz + n, pz + n + 2, n);
  }

  for (n = 64; n > 0; n--) {
    pz[n] = pz[n + 6];
    tean(pz + n, pz + n + 2, -n);
  }

  START_ENCRYPT();

  for (n = 1; n < 65; n++) { 
    code(pz + n, pz + n + 2);
    pz[n + 6] = pz[n];
  }
  
  START_DECRYPT();

  for (n = 64; n > 0; n--) {
    pz[n] = pz[n + 6];
    decode(pz + n, pz + n + 2);
  }

  END_EXPE();
  return 0;
}


/* TEA code and decode given below */

void code(long *v, long *k)
{   unsigned long y = v[0],
                  z = v[1],
                  sum = 0,		/* set up */
                  delta = 0x9e3779b9,
                  n = 32,
                  a = k[0],
                  b = k[1],
                  c = k[2],
                  d = k[3];

    while (n-- > 0)			/* basic cycle start */
    {   sum += delta;
        y   += (z << 4) + a ^ z + sum ^ (z >> 5) + b;
        z   += (y << 4) + c ^ y + sum ^ (y >> 5) + d;		/* end cycle */
    }

    v[0] = y;
    v[1] = z;
}


void decode(long *v, long *k)
{   unsigned long n = 32,
                  sum,
                  y = v[0],
                  z = v[1],
                  delta = 0x9e3779b9,
                  a = k[0],
                  b = k[1],
                  c = k[2],
                  d = k[3];

    sum = delta << 5;
						/* start cycle */
    while (n-- > 0)
    {   z   -= (y << 4) + c ^ y + sum ^ (y >> 5) + d; 
        y   -= (z << 4) + a ^ z + sum ^ (z >> 5) + b;
        sum -= delta;
    }
						/* end cycle */
    v[0] = y;
    v[1] = z;
}



/* XTEA is a version of slightly improved tea.
   The plain or cypher text is in v[0], v[1].
   The key is in k[n], where n = 0 - 3,
   The number of coding cycles is given by N and
   the number of decoding cycles given by -N     */
void
tean(long *v, long *k, long ncycles)      /* replaces TEA's code and decode */
{ 
  unsigned long
    y = v[0],
    z = v[1],
    DELTA = 0x9e3779b9,
    limit,
    sum;

    if (ncycles > 0)                  /* coding */
    {   limit = DELTA * ncycles;
        sum = 0;

        while (sum != limit)
        {   y   += (z << 4 ^ z >> 5) + z ^ sum + k[sum & 3];
            sum += DELTA;
            z   += (y << 4 ^ y >> 5) + y ^ sum + k[sum >> 11 & 3];
        }
    }
    else                          /* decoding */
    {    sum = DELTA * (-ncycles);

         while (sum)
         {   z   -= (y << 4 ^ y >> 5) + y ^ sum + k[sum >> 11 & 3];
             sum -= DELTA;
             y   -= (z << 4 ^ z >> 5) + z ^ sum + k[sum & 3];
         }
    }

    v[0] = y;
    v[1] = z;

    return;
}
