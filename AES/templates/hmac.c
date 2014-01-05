/* hmac.c
	H-MAC general template for the MSP430 crypto library
	Written by Paul Ferrand <paul.ferrand@insa-lyon.fr>
 */

#include "tools.h"
 
void HMAC_initfunc( HMAC_ctx *ctx, u8 *key, u32 len)
{
	u32 i;
	u32 l;
	u8 *k;
	HMAC_HASH_CTX context;
	
	if ( len <= HMAC_HASH_BLOCK_SIZE )
	{
		k = key;
		l = len;
	} else {
		HMAC_HASH_INIT_FUNCTION(&context);
		HMAC_HASH_UPDATE_FUNCTION(&context, key, len);
		HMAC_HASH_FINAL_FUNCTION(&context);
		k = context.HMAC_HASH_DIGEST;
		l = HMAC_HASH_DIGEST_SIZE;
	}
	for( i = 0; i < l; ++i)
	{
		ctx->kopad[i] = k[i] ^ 0x5C;
		ctx->kipad[i] = k[i] ^ 0x36;
	}
	
	for( i = l; i < HMAC_HASH_BLOCK_SIZE; ++i)
	{
		ctx->kopad[i] = 0x5C;
		ctx->kipad[i] = 0x36;
	}
	HMAC_HASH_INIT_FUNCTION(&ctx->iCtx);
	HMAC_HASH_UPDATE_FUNCTION(&ctx->iCtx, ctx->kipad, HMAC_HASH_BLOCK_SIZE);
	HMAC_HASH_INIT_FUNCTION(&ctx->oCtx);
}

void HMAC_cleanupfunc( HMAC_ctx *ctx)
{
	memset(ctx->kipad, 0, HMAC_HASH_BLOCK_SIZE);
	memset(ctx->kopad, 0, HMAC_HASH_BLOCK_SIZE);
}

void HMAC_updatefunc( HMAC_ctx *ctx, u8 *data, u32 len)
{
	HMAC_HASH_UPDATE_FUNCTION(&ctx->iCtx, data, len);
}

void HMAC_finalfunc( HMAC_ctx *ctx)
{
	HMAC_HASH_FINAL_FUNCTION( &ctx->iCtx );
	HMAC_HASH_UPDATE_FUNCTION( &ctx->oCtx, ctx->kopad, HMAC_HASH_BLOCK_SIZE );
	HMAC_HASH_UPDATE_FUNCTION( &ctx->oCtx, ctx->iCtx.HMAC_HASH_DIGEST, HMAC_HASH_DIGEST_SIZE );
	HMAC_HASH_FINAL_FUNCTION( &ctx->oCtx );
	bloc_memcpy( ctx->mac, ctx->oCtx.HMAC_HASH_DIGEST, HMAC_HASH_DIGEST_SIZE);
}
