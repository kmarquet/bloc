
static inline uint32_t lcs1_32(uint32_t x)
{
  uint32_t result = x;

  /* Shift a zero into the least significant bit, then correct by
   * adding in the carry */

  __asm__(
    "rla %A0" "\n\t"
    "rlc %B0" "\n\t"
    "adc %A0" "\n\t"
    : "+r" (result)
  );

  return result;
}


static inline uint64_t lcs1_64(uint64_t x)
{
  uint64_t result = x;

  /* Shift a zero into the least significant bit, then correct by
   * adding in the carry */

  __asm__(
    "rla %A0" "\n\t"
    "rlc %B0" "\n\t"
    "rlc %C0" "\n\t"
    "rlc %D0" "\n\t"
    "adc %A0" "\n\t"
    : "+r" (result)
  );

  return result;
}

/* Rotate right by 8 bits */
static inline uint64_t rcs8_64(uint64_t x)
{
  uint64_t result = x;
  uint16_t tmp;

  /*
   * Start with the following bytes:
   *
   * x_7 x_6 x_5 x_4 x_3 x_2 x_1 x_0
   *
   * Save the least significant byte in a temp register
   *
   * Swap the bytes in the two least significant words, giving
   *
   * x_7 x_6 x_5 x_4 x_2 x_3 x_0 x_1
   *
   * Get the bytes in the right word with a pair of xors:
   *
   * xor.b gives:
   *
   * x_7 x_6 x_5 x_4 x_2 x_3 0 x_1^x_3
   *
   * xor gives:
   * x_7 x_6 x_5 x_4 x_2 x_3 x_2 x_1
   *
   * Continue with the 2nd and third words:
   *
   * swpb (third word)
   * xor.b
   * xor
   *
   */

  __asm__(
    "mov.b %A0, %1"  "\n\t"
    "swpb  %A0"      "\n\t"
    "swpb  %B0"      "\n\t"
    "xor.b %B0, %A0" "\n\t"
    "xor   %B0, %A0" "\n\t"
    "swpb  %C0"      "\n\t"
    "xor.b %C0, %B0" "\n\t"
    "xor   %C0, %B0" "\n\t"
    "swpb  %D0"      "\n\t"
    "xor.b %D0, %C0" "\n\t"
    "xor   %D0, %C0" "\n\t"
    "swpb  %1"       "\n\t"
    "xor.b %1,  %D0" "\n\t"
    "xor   %1,  %D0" "\n\t"
    : "+r" (result), "=r" (tmp)
  );

  return result;
}


static inline uint32_t rcs8_32(uint32_t x)
{
  uint32_t result = x;
  uint16_t tmp;

  /*
   * Start with the following bytes:
   *
   * x_3 x_2 x_1 x_0
   *
   * Save the least significant byte in a temp register
   *
   * Swap the bytes in the two least significant words, giving
   *
   * x_2 x_3 x_0 x_1
   *
   * Get the bytes in the right word with a pair of xors:
   *
   * xor.b gives:
   *
   * x_2 x_3 0 x_1^x_3
   *
   * xor gives:
   * x_2 x_3 x_2 x_1
   *
   * Swap the temp variable and perform the pair of xors again to
   * get the following:
   *
   * 0   x_3 ...
   * x_0 x_3
   *
   */

  __asm__(
    "mov.b %A0, %1"  "\n\t"
    "xor.b %B0, %1"  "\n\t"
    "swpb  %A0"      "\n\t"
    "swpb  %B0"      "\n\t"
    "swpb  %1"       "\n\t"
    "xor   %1,  %A0" "\n\t"
    "xor   %1,  %B0" "\n\t"
    : "+r" (result), "=r" (tmp)
  );

  return result;
}

/* Rotate left by 8 bits */
static inline uint64_t lcs8_64(uint64_t x)
{
  uint64_t result = x;
  uint16_t tmp;

  /*
   * Start with the following bytes:
   *
   * x_7 x_6 x_5 x_4 x_3 x_2 x_1 x_0
   *
   * Save the most significant byte in a temp register
   *
   * Use xor to get the right bytes in the right words, and then
   * swap into the right places:
   *
   * xor.b : 0   x_6^x_4 x_5 x_4 x_3 x_2 x_1 x_0
   * xor   : x_5 x_6     x_5 x_4 x_3 x_2 x_1 x_0
   * swpb  : x_6 x_5     x_5 x_4 x_3 x_2 x_1 x_0
   */

  __asm__(
    "mov   %D0, %1"  "\n\t"

    "xor.b %C0, %D0" "\n\t"
    "xor   %C0, %D0" "\n\t"
    "swpb  %D0"      "\n\t"

    "xor.b %B0, %C0" "\n\t"
    "xor   %B0, %C0" "\n\t"
    "swpb  %C0"      "\n\t"

    "xor.b %A0, %B0" "\n\t"
    "xor   %A0, %B0" "\n\t"
    "swpb  %B0"      "\n\t"

    "xor.b %1, %A0"  "\n\t"
    "xor   %1, %A0"  "\n\t"
    "swpb  %A0"      "\n\t"

    : "+r" (result), "=r" (tmp)
  );

  return result;
}

static inline uint32_t lcs8_32(uint32_t x)
{
  uint32_t result = x;
  uint16_t tmp;

  /*
   * Start with the following bytes:
   *
   * x_3 x_2 x_1 x_0
   *
   * Swap the bytes and the use an xor swap to exchange x_3 and x_1
   */

  __asm__(
    "swpb  %B0"      "\n\t"
    "swpb  %A0"      "\n\t"

    "mov.b %B0, %1"  "\n\t"
    "xor.b %A0, %1"  "\n\t"

    "xor   %1, %B0" "\n\t"
    "xor   %1, %A0" "\n\t"

    : "+r" (result), "=r" (tmp)
  );

  return result;
}
