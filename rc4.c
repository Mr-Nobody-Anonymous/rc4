#include "rc4.h"
#include <string.h>

/**
 * Key Scheduling Algorithm (KSA)
 * Initializes the permutation array S based on the key
 */
static void rc4_ksa(RC4_CTX *ctx, const uint8_t *key, size_t key_len) {
    uint8_t j = 0;
    uint8_t tmp;

    /* Initialize S with identity permutation */
    for (uint16_t i = 0; i < 256; i++) {
        ctx->S[i] = (uint8_t)i;
    }

    /* Key scheduling */
    for (uint16_t i = 0; i < 256; i++) {
        j = j + ctx->S[i] + key[i % key_len];
        tmp = ctx->S[i];
        ctx->S[i] = ctx->S[j];
        ctx->S[j] = tmp;
    }

    /* Initialize indices */
    ctx->i = 0;
    ctx->j = 0;
}

/**
 * Pseudo-Random Generation Algorithm (PRGA)
 * Generates keystream and XORs with data
 */
static void rc4_prga(RC4_CTX *ctx, uint8_t *data, size_t len) {
    uint8_t i = ctx->i;
    uint8_t j = ctx->j;
    uint8_t tmp;
    uint8_t k;

    for (size_t n = 0; n < len; n++) {
        i = i + 1;
        j = j + ctx->S[i];
        
        tmp = ctx->S[i];
        ctx->S[i] = ctx->S[j];
        ctx->S[j] = tmp;
        
        k = ctx->S[(ctx->S[i] + ctx->S[j]) & 0xFF];
        data[n] ^= k;
    }

    ctx->i = i;
    ctx->j = j;
}

void rc4_init(RC4_CTX *ctx, const uint8_t *key, size_t key_len) {
    if (!ctx || !key || key_len == 0) {
        return;
    }
    rc4_ksa(ctx, key, key_len);
}

void rc4_crypt(RC4_CTX *ctx, uint8_t *data, size_t len) {
    if (!ctx || !data || len == 0) {
        return;
    }
    rc4_prga(ctx, data, len);
}

void rc4(const uint8_t *key, size_t key_len, uint8_t *data, size_t len) {
    RC4_CTX ctx;
    rc4_init(&ctx, key, key_len);
    rc4_crypt(&ctx, data, len);
}