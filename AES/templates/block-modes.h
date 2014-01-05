/* block-modes.h
	Block cipher modes general template for the MSP430 crypto library
	Written by Paul Ferrand <paul.ferrand@insa-lyon.fr>
 */
 
#ifndef BM_CIPHER_BLOCK_SIZE
#error "Block modes template called but BM_CIPHER_BLOCK_SIZE not defined."
#endif
 
#ifndef BM_CIPHER_CTX
#error "Block modes template called but BM_CIPHER_CTX not defined."
#endif

#ifndef BM_CIPHER_SHORTNAME
#error "Block modes template called but BM_CIPHER_SHORTNAME not defined."
#endif
 
#ifndef BM_CIPHER_INIT_FUNCTION
#error "Block modes template called but BM_CIPHER_INIT_FUNCTION not defined."
#endif

#ifndef BM_CIPHER_ENCRYPT_FUNCTION
#error "Block modes template called but BM_CIPHER_ENCRYPT_FUNCTION not defined."
#endif 

#ifndef BM_CIPHER_DECRYPT_FUNCTION
#error "Block modes template called but BM_CIPHER_DECRYPT_FUNCTION not defined."
#endif 

/* We need an additional indirection to be sure to expand macroed names
	in SMASH. This is actually true for all macros. */
#define QSMASH(x, y, z) x##y##z
#define SMASH(x, y, z) QSMASH(x,y,z)

#define BM_CBC_init SMASH(,BM_CIPHER_SHORTNAME,_CBC_init)
#define BM_CBC_keysetup SMASH(,BM_CIPHER_SHORTNAME,_CBC_keysetup)
#define BM_CBC_ivsetup SMASH(,BM_CIPHER_SHORTNAME,_CBC_ivsetup)
#define BM_CBC_encrypt SMASH(,BM_CIPHER_SHORTNAME,_CBC_encrypt)
#define BM_CBC_encrypt_final SMASH(,BM_CIPHER_SHORTNAME,_CBC_encrypt_final)
#define BM_CBC_decrypt SMASH(,BM_CIPHER_SHORTNAME,_CBC_decrypt)
#define BM_ECB_init SMASH(,BM_CIPHER_SHORTNAME,_ECB_init)
#define BM_ECB_encrypt SMASH(,BM_CIPHER_SHORTNAME,_ECB_encrypt)
#define BM_ECB_encrypt_final SMASH(,BM_CIPHER_SHORTNAME,_ECB_encrypt_final)
#define BM_ECB_decrypt SMASH(,BM_CIPHER_SHORTNAME,_ECB_decrypt)
#define BM_CTR_init SMASH(,BM_CIPHER_SHORTNAME,_CTR_init)
#define BM_CTR_keysetup SMASH(,BM_CIPHER_SHORTNAME,_CTR_keysetup)
#define BM_CTR_noncesetup SMASH(,BM_CIPHER_SHORTNAME,_CTR_noncesetup)
#define BM_CTR_process SMASH(,BM_CIPHER_SHORTNAME,_CTR_process)

/* Contexts */
#define BM_CBC_ctx SMASH(,BM_CIPHER_SHORTNAME,_CBC_CTX)
#define BM_ECB_ctx SMASH(,BM_CIPHER_SHORTNAME,_ECB_CTX)
#define BM_CTR_ctx SMASH(,BM_CIPHER_SHORTNAME,_CTR_CTX)
 
/* Portable macro and types definitions for the crypto library */
#include "../common/portable.h"
	

/* 
 * CBC
 */
typedef struct {
	BM_CIPHER_CTX cipher_ctx;
	int inputBlockIndex;
	u8 chainBlock[BM_CIPHER_BLOCK_SIZE];
	u8 inputBlock[BM_CIPHER_BLOCK_SIZE];
} BM_CBC_ctx;

void BM_CBC_init( BM_CBC_ctx * ctx );
void BM_CBC_keysetup( BM_CBC_ctx * ctx, u8 *key );
void BM_CBC_ivsetup( BM_CBC_ctx * ctx, u8 *iv );
void BM_CBC_encrypt( 	BM_CBC_ctx * ctx, u8 *inBlock, u8 *outBlock, 
						u32 *outIndex, u32 len);
void BM_CBC_encrypt_final( BM_CBC_ctx * ctx, u8 *outBlock, u32 *outIndex );
void BM_CBC_decrypt( 	BM_CBC_ctx * ctx, u8 *inBlock, u8 *outBlock, 
						u32 *outIndex, u32 len);


/* 
 * ECB
 */
typedef struct {
	BM_CIPHER_CTX cipher_ctx;
	int inputBlockIndex;
	u8 inputBlock[BM_CIPHER_BLOCK_SIZE];
} BM_ECB_ctx;

void BM_ECB_init( BM_ECB_ctx *ctx, u8 *key  );
void BM_ECB_keysetup( BM_CBC_ctx * ctx, u8 *key );
void BM_ECB_encrypt( 	BM_ECB_ctx * ctx, u8 *inBlock, u8 *outBlock, 
						u32 *outIndex, u32 len );
void BM_ECB_encrypt_final( BM_ECB_ctx * ctx, u8 *outBlock, u32 *outIndex );
void BM_ECB_decrypt( 	BM_ECB_ctx * ctx, u8 *inBlock, u8 *outBlock, 
						u32 *outIndex, u32 len );

					
/* 
 * CTR
 */	
typedef struct {
	BM_CIPHER_CTX cipher_ctx;
	int cipherNonceIndex;
	u8 nonce[BM_CIPHER_BLOCK_SIZE];
	u8 cipherNonce[BM_CIPHER_BLOCK_SIZE];
} BM_CTR_ctx;

void BM_CTR_init( BM_CTR_ctx * ctx );
void BM_CTR_keysetup( BM_CTR_ctx * ctx, u8 *key );
void BM_CTR_noncesetup( BM_CTR_ctx * ctx, u8 *nonce );
void BM_CTR_process( 	BM_CTR_ctx * ctx, u8 *inBlock, u8 *outBlock, 
						u32 *outIndex, u32 len );
	
