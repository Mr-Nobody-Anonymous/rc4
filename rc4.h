#ifndef RC4_H
#define RC4_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * RC4 context structure for streaming encryption/decryption
 */
typedef struct {
    uint8_t S[256];
    uint8_t i;
    uint8_t j;
} RC4_CTX;

/**
 * Initialize RC4 context with a key
 * @param ctx Pointer to RC4 context
 * @param key Pointer to key data
 * @param key_len Length of key in bytes
 */
void rc4_init(RC4_CTX *ctx, const uint8_t *key, size_t key_len);

/**
 * Encrypt/decrypt data using RC4 (symmetric operation)
 * @param ctx Pointer to initialized RC4 context
 * @param data Pointer to data buffer (modified in-place)
 * @param len Length of data in bytes
 */
void rc4_crypt(RC4_CTX *ctx, uint8_t *data, size_t len);

/**
 * Convenience function: encrypt/decrypt in one call
 * @param key Pointer to key data
 * @param key_len Length of key in bytes
 * @param data Pointer to data buffer (modified in-place)
 * @param len Length of data in bytes
 */
void rc4(const uint8_t *key, size_t key_len, uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* RC4_H */