/*  aes-naive.h
 * msp430 crypto librarie
 * Written by Paul Ferrand <paul.ferrand@insa-lyon.fr>
 */

#ifndef AES_H
#define AES_H

#include "tools.h"
#include "common/portable.h"

#define AES_BLOCK_SIZE 	16 /* in bytes */
#define AES_MAX_NR 		14 
#define STATIC_LOGTABLES
#define STATIC_SBOXES
typedef struct {
	u8 enc_ks[AES_BLOCK_SIZE * (AES_MAX_NR + 1)]; // encoding key schedule
	u8 dec_ks[AES_BLOCK_SIZE * (AES_MAX_NR + 1)]; // decoding key schedule
	u8 rounds;
} AES_CTX;

void AES_init_variable_keylength( AES_CTX *ctx, u8 *key, int klen);
void AES_init( AES_CTX *ctx, u8 *key);
void AES_encrypt( AES_CTX *ctx, u8 *in, u8 *out );
void AES_decrypt( AES_CTX *ctx, u8 *in, u8 *out);

/* Block cipher modes */
	
#define BM_CIPHER_BLOCK_SIZE AES_BLOCK_SIZE
#define BM_CIPHER_CTX AES_CTX
#define BM_CIPHER_SHORTNAME AES
#define BM_CIPHER_INIT_FUNCTION AES_init
#define BM_CIPHER_ENCRYPT_FUNCTION AES_encrypt
#define BM_CIPHER_DECRYPT_FUNCTION AES_decrypt

#include "templates/block-modes.h"

#endif /* AES_H */
