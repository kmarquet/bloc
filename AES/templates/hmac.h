/* hmac.h
	H-MAC general template for the MSP430 crypto library
	Written by Paul Ferrand <paul.ferrand@insa-lyon.fr>
 */
 
#ifndef HMAC_HASH_BLOCK_SIZE
#error "HMAC template called but HMAC_HASH_BLOCK_SIZE not defined."
#endif
 
#ifndef HMAC_HASH_DIGEST_SIZE
#error "HMAC template called but HMAC_HASH_DIGEST_SIZE not defined."
#endif
 
#ifndef HMAC_HASH_CTX
#error "HMAC template called but HMAC_HASH_CTX not defined."
#endif

#ifndef HMAC_HASH_SHORTNAME
#error "HMAC template called but HMAC_HASH_SHORTNAME not defined."
#endif
 
#ifndef HMAC_HASH_INIT_FUNCTION
#error "HMAC template called but HMAC_HASH_INIT_FUNCTION not defined."
#endif

#ifndef HMAC_HASH_UPDATE_FUNCTION
#error "HMAC template called but HMAC_HASH_UPDATE_FUNCTION not defined."
#endif 

#ifndef HMAC_HASH_FINAL_FUNCTION
#error "HMAC template called but HMAC_HASH_FINAL_FUNCTION not defined."
#endif 
 
#ifndef HMAC_HASH_DIGEST
#error "HMAC template called but HMAC_HASH_DIGEST not defined."
#endif 

/* We need an additional indirection to be sure to expand macroed names
	in SMASH. This is actually true for all macros. */
#define QSMASH(x, y, z) x##y##z
#define SMASH(x, y, z) QSMASH(x,y,z)

#define HMAC_initfunc SMASH(HMAC_,HMAC_HASH_SHORTNAME,_init)
#define HMAC_cleanupfunc SMASH(HMAC_,HMAC_HASH_SHORTNAME,_cleanup)
#define HMAC_updatefunc SMASH(HMAC_,HMAC_HASH_SHORTNAME,_update)
#define HMAC_finalfunc SMASH(HMAC_,HMAC_HASH_SHORTNAME,_final)

/* #define HMAC_CTX(name) HMAC_ ## name ## _CTX */
#define HMAC_ctx SMASH(HMAC_,HMAC_HASH_SHORTNAME,_CTX)
 
/* Portable macro and types definitions for the crypto library */
#include "../common/portable.h"
	
/** 
 * HMAC Context.
 */
typedef struct {
	u8 kipad[HMAC_HASH_BLOCK_SIZE];
	HMAC_HASH_CTX iCtx;
	u8 kopad[HMAC_HASH_BLOCK_SIZE];
	HMAC_HASH_CTX oCtx;
	u8 mac[HMAC_HASH_DIGEST_SIZE];
} HMAC_ctx;

void HMAC_initfunc( HMAC_ctx *ctx, u8 *key, u32 len);
void HMAC_cleanupfunc( HMAC_ctx *ctx);
void HMAC_updatefunc( HMAC_ctx *ctx, u8 *data, u32 len);
void HMAC_finalfunc( HMAC_ctx *ctx);
