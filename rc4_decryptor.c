#include "rc4.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define DEFAULT_KEY "MySecretKey2026"
#define ENCRYPTED_FILE "file.txt"
#define DECRYPTED_FILE "file_decrypted.txt"
#define BACKUP_FILE "file.txt.backup"

void print_hex(const char *label, const uint8_t *data, size_t len, size_t max_print) {
    printf("%s", label);
    size_t print_len = (len < max_print) ? len : max_print;
    for (size_t i = 0; i < print_len; i++) {
        printf("%02x", data[i]);
    }
    if (len > max_print) {
        printf("... (%zu more bytes)", len - max_print);
    }
    printf("\n");
}

int verify_files(const char *file1, const char *file2) {
    FILE *fp1 = fopen(file1, "rb");
    FILE *fp2 = fopen(file2, "rb");
    
    if (!fp1 || !fp2) {
        if (fp1) fclose(fp1);
        if (fp2) fclose(fp2);
        return 0;
    }

    int result = 1;
    uint8_t buf1[4096];
    uint8_t buf2[4096];
    size_t read1, read2;

    while (1) {
        read1 = fread(buf1, 1, sizeof(buf1), fp1);
        read2 = fread(buf2, 1, sizeof(buf2), fp2);
        
        if (read1 != read2) {
            result = 0;
            break;
        }
        if (read1 == 0) {
            break;  /* Both reached EOF */
        }
        if (memcmp(buf1, buf2, read1) != 0) {
            result = 0;
            break;
        }
    }

    fclose(fp1);
    fclose(fp2);
    return result;
}

int main(int argc, char *argv[]) {
    const char *key_str = (argc > 1) ? argv[1] : DEFAULT_KEY;
    const char *encrypted_file = ENCRYPTED_FILE;
    const char *decrypted_file = DECRYPTED_FILE;
    FILE *fp;
    uint8_t *buffer = NULL;
    size_t file_size = 0;
    size_t bytes_read = 0;
    long pos;

    printf("==================================================\n");
    printf("RC4 File Decryptor (C Implementation)\n");
    printf("==================================================\n");
    printf("Encrypted file:  %s\n", encrypted_file);
    printf("Decrypted file:  %s\n", decrypted_file);
    printf("Key:             %s\n", key_str);
    printf("--------------------------------------------------\n");

    /* Open encrypted file */
    fp = fopen(encrypted_file, "rb");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open '%s' for reading.\n", encrypted_file);
        fprintf(stderr, "Please run the encryptor first to create an encrypted file.\n");
        return 1;
    }

    /* Get file size */
    fseek(fp, 0, SEEK_END);
    pos = ftell(fp);
    if (pos < 0) {
        fprintf(stderr, "Error: Cannot determine file size.\n");
        fclose(fp);
        return 1;
    }
    file_size = (size_t)pos;
    fseek(fp, 0, SEEK_SET);

    if (file_size == 0) {
        fprintf(stderr, "Error: File is empty.\n");
        fclose(fp);
        return 1;
    }

    /* Allocate buffer */
    buffer = (uint8_t *)malloc(file_size);
    if (!buffer) {
        fprintf(stderr, "Error: Memory allocation failed for %zu bytes.\n", file_size);
        fclose(fp);
        return 1;
    }

    /* Read encrypted data */
    bytes_read = fread(buffer, 1, file_size, fp);
    fclose(fp);

    if (bytes_read != file_size) {
        fprintf(stderr, "Error: Failed to read complete file (read %zu of %zu bytes).\n",
                bytes_read, file_size);
        free(buffer);
        return 1;
    }

    printf("Encrypted file size: %zu bytes\n", file_size);
    print_hex("Encrypted content preview: ", buffer, file_size, 100);

    /* Decrypt the data (RC4 is symmetric) */
    rc4((const uint8_t *)key_str, strlen(key_str), buffer, file_size);

    /* Write decrypted data to output file */
    fp = fopen(decrypted_file, "wb");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open '%s' for writing.\n", decrypted_file);
        free(buffer);
        return 1;
    }

    size_t bytes_written = fwrite(buffer, 1, file_size, fp);
    fclose(fp);

    if (bytes_written != file_size) {
        fprintf(stderr, "Error: Failed to write complete file (wrote %zu of %zu bytes).\n",
                bytes_written, file_size);
        free(buffer);
        return 1;
    }

    free(buffer);

    printf("--------------------------------------------------\n");
    printf("Decryption completed successfully!\n");
    printf("Decrypted file size: %zu bytes\n", file_size);

    /* Show decrypted preview */
    fp = fopen(decrypted_file, "rb");
    if (fp) {
        buffer = (uint8_t *)malloc(file_size);
        if (buffer) {
            fread(buffer, 1, file_size, fp);
            fclose(fp);
            print_hex("Decrypted content preview: ", buffer, file_size, 100);
            free(buffer);
        }
    }

    /* Verify against original backup if it exists */
    printf("--------------------------------------------------\n");
    if (verify_files(BACKUP_FILE, decrypted_file)) {
        printf("VERIFICATION PASSED: Decrypted file matches original exactly!\n");
    } else {
        printf("VERIFICATION FAILED: Decrypted file does NOT match original!\n");
        printf("Note: Create a backup before encryption: cp file.txt %s\n", BACKUP_FILE);
        return 1;
    }

    return 0;
}