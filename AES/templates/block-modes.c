/* block-modes.c
	Block cipher modes general template for the MSP430 crypto library
	Written by Paul Ferrand <paul.ferrand@insa-lyon.fr>
 */

/* For memcpy */
#include "tools.h"


/**
  * Initialize the context. Copy the IV into the first chaining block
  * and setup the block cipher.
  */
void BM_CBC_init( BM_CBC_ctx * ctx )
{
	ctx->inputBlockIndex = 0;
}

void BM_CBC_keysetup( BM_CBC_ctx * ctx, u8 *key )
{
	BM_CIPHER_INIT_FUNCTION(&ctx->cipher_ctx, key);
	ctx->inputBlockIndex = 0;
}

void BM_CBC_ivsetup( BM_CBC_ctx * ctx, u8 *iv )
{
	memcpy(ctx->chainBlock, iv, BM_CIPHER_BLOCK_SIZE);
	ctx->inputBlockIndex = 0;
}

/**
 * Encrypt the flow of data. The method fills up the input block, and if it 
 * reaches
 */
void BM_CBC_encrypt( 	BM_CBC_ctx * ctx, u8 *inBlock, u8 *outBlock, 
						u32 *outIndex, u32 len)
{
	u32 plainIndex, cipherIndex, xorIndex;
	plainIndex = 0;
	cipherIndex = *outIndex;
	while( plainIndex < len )
	{
		if( ctx->inputBlockIndex < BM_CIPHER_BLOCK_SIZE)
		{
			ctx->inputBlock[ctx->inputBlockIndex++] = inBlock[plainIndex++];
		}
		if( ctx->inputBlockIndex == BM_CIPHER_BLOCK_SIZE)
		{
			ctx->inputBlockIndex = 0;
			for( xorIndex = 0; xorIndex < BM_CIPHER_BLOCK_SIZE; ++xorIndex )
			{
				ctx->inputBlock[xorIndex] ^= ctx->chainBlock[xorIndex];
			}
			
			BM_CIPHER_ENCRYPT_FUNCTION( &ctx->cipher_ctx, ctx->inputBlock, ctx->chainBlock );
			memcpy( outBlock + cipherIndex, ctx->chainBlock, BM_CIPHER_BLOCK_SIZE );
			cipherIndex += BM_CIPHER_BLOCK_SIZE;
		}
	}
	*outIndex = cipherIndex;
}

void BM_CBC_encrypt_final( BM_CBC_ctx * ctx, u8 *outBlock, u32 *outIndex )
{
	int padLength;
	u32 i, xorIndex, cipherIndex;
	cipherIndex = *outIndex;
	if 	(ctx->inputBlockIndex == 0)
		return;
	padLength = BM_CIPHER_BLOCK_SIZE - ctx->inputBlockIndex;
	for( i = 0; i < padLength; ++i)
	{
		ctx->inputBlock[ctx->inputBlockIndex++] = (u8)padLength;
	}
	for( xorIndex = 0; xorIndex < BM_CIPHER_BLOCK_SIZE; ++xorIndex )
	{
		ctx->inputBlock[xorIndex] ^= ctx->chainBlock[xorIndex];
	}
	BM_CIPHER_ENCRYPT_FUNCTION( &ctx->cipher_ctx, ctx->inputBlock, outBlock + cipherIndex );
	cipherIndex += BM_CIPHER_BLOCK_SIZE;
	*outIndex = cipherIndex;	
}

/**
 * Decrypt the flow of data. 
 */
void BM_CBC_decrypt( 	BM_CBC_ctx * ctx, u8 *inBlock, u8 *outBlock, 
						u32 *outIndex, u32 len)
{
	u32 plainIndex, cipherIndex, xorIndex;
	cipherIndex = 0;
	plainIndex = *outIndex;
	while( cipherIndex < len )
	{
		if( ctx->inputBlockIndex < BM_CIPHER_BLOCK_SIZE)
		{
			ctx->inputBlock[ctx->inputBlockIndex++] = inBlock[cipherIndex++];
		}
		if( ctx->inputBlockIndex == BM_CIPHER_BLOCK_SIZE)
		{
			ctx->inputBlockIndex = 0;
			BM_CIPHER_DECRYPT_FUNCTION( &ctx->cipher_ctx, ctx->inputBlock, outBlock + plainIndex );				
			for( xorIndex = 0; xorIndex < BM_CIPHER_BLOCK_SIZE; ++xorIndex )
			{
				outBlock[plainIndex + xorIndex] ^= ctx->chainBlock[xorIndex];
			}
			memcpy( ctx->chainBlock, ctx->inputBlock, BM_CIPHER_BLOCK_SIZE );			
			plainIndex += BM_CIPHER_BLOCK_SIZE;
		}
	}
	*outIndex = plainIndex;
}

/**
  * Initialize the context. Copy the IV into the first chaining block
  * and setup the block cipher.
  */
void BM_ECB_init( BM_ECB_ctx * ctx, u8 * key )
{
	BM_CIPHER_INIT_FUNCTION(&ctx->cipher_ctx, key);
	ctx->inputBlockIndex = 0;
}

/**
 * Encrypt the flow of data. 
 */
void BM_ECB_encrypt( 	BM_ECB_ctx * ctx, u8 *inBlock, u8 *outBlock, 
						u32 *outIndex, u32 len )
{
	u32 plainIndex, cipherIndex;
	plainIndex = 0;
	cipherIndex = *outIndex;
	while( plainIndex < len )
	{
		if( ctx->inputBlockIndex < BM_CIPHER_BLOCK_SIZE)
		{
			ctx->inputBlock[ctx->inputBlockIndex++] = inBlock[plainIndex++];
		}
		if( ctx->inputBlockIndex == BM_CIPHER_BLOCK_SIZE)
		{
			ctx->inputBlockIndex = 0;			
			BM_CIPHER_ENCRYPT_FUNCTION( &ctx->cipher_ctx, ctx->inputBlock, outBlock + cipherIndex );
			cipherIndex += BM_CIPHER_BLOCK_SIZE;
		}
	}
	*outIndex = cipherIndex;
}

void BM_ECB_encrypt_final( BM_ECB_ctx * ctx, u8 *outBlock, u32 *outIndex )
{
	int padLength;
	u32 i, cipherIndex;
	cipherIndex = *outIndex;
	if 	(ctx->inputBlockIndex == 0)
		return;
	padLength = BM_CIPHER_BLOCK_SIZE - ctx->inputBlockIndex;
	for( i = 0; i < padLength; ++i)
	{
		ctx->inputBlock[ctx->inputBlockIndex++] = (u8)padLength;
	}
	BM_CIPHER_ENCRYPT_FUNCTION( &ctx->cipher_ctx, ctx->inputBlock, outBlock + cipherIndex );
	cipherIndex += BM_CIPHER_BLOCK_SIZE;
	*outIndex = cipherIndex;	
}

/**
 * Decrypt the flow of data. 
 */
void BM_ECB_decrypt( 	BM_ECB_ctx * ctx, u8 *inBlock, u8 *outBlock, 
						u32 *outIndex, u32 len )
{
	u32 plainIndex, cipherIndex;
	cipherIndex = 0;
	plainIndex = *outIndex;
	while( cipherIndex < len )
	{
		if( ctx->inputBlockIndex < BM_CIPHER_BLOCK_SIZE)
		{
			ctx->inputBlock[ctx->inputBlockIndex++] = inBlock[cipherIndex++];
		}
		if( ctx->inputBlockIndex == BM_CIPHER_BLOCK_SIZE)
		{
			ctx->inputBlockIndex = 0;
			BM_CIPHER_DECRYPT_FUNCTION( &ctx->cipher_ctx, ctx->inputBlock, outBlock + plainIndex );				
			plainIndex += BM_CIPHER_BLOCK_SIZE;
		}
	}
	*outIndex = plainIndex;
}

/**
  * Initialize the context. Copy the IV into the first chaining block
  * and setup the block cipher.
  */
void BM_CTR_init( BM_CTR_ctx * ctx )
{
	ctx->cipherNonceIndex = 0;
}

void BM_CTR_keysetup( BM_CTR_ctx * ctx, u8 *key )
{
	BM_CIPHER_INIT_FUNCTION(&ctx->cipher_ctx, key);
	ctx->cipherNonceIndex = 0;
}

void BM_CTR_noncesetup( BM_CTR_ctx * ctx, u8 *nonce )
{
	memcpy(ctx->nonce, nonce, BM_CIPHER_BLOCK_SIZE);
	ctx->cipherNonceIndex = 0;
}

/**
 * Encrypt the flow of data. 
 */
void BM_CTR_process( 	BM_CTR_ctx * ctx, u8 *inBlock, u8 *outBlock, 
						u32 *outIndex, u32 len )
{
	u32 iIndex, oIndex, nonceIndex;
	u8 carry;
	oIndex = *outIndex;
	for( iIndex = 0; iIndex < len; ++iIndex )
	{
		if(iIndex % BM_CIPHER_BLOCK_SIZE == 0)
		{
			BM_CIPHER_ENCRYPT_FUNCTION( &ctx->cipher_ctx, ctx->nonce, ctx->cipherNonce );
			carry = 1;
			nonceIndex = BM_CIPHER_BLOCK_SIZE - 1;
			while( carry == 1 && nonceIndex >= 0 )
			{
				if( ++(ctx->nonce[nonceIndex--]) == 0 )
					carry = 1;
				else 
					carry = 0;
			}
			ctx->cipherNonceIndex = 0;
		}
		outBlock[oIndex + iIndex] = inBlock[iIndex] ^ ctx->cipherNonce[ctx->cipherNonceIndex++];
	}
	*outIndex = oIndex;
}
