#include "rc4.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define DEFAULT_KEY "MySecretKey2026"
#define INPUT_FILE "file.txt"
#define BUFFER_SIZE 8192

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

int main(int argc, char *argv[]) {
    const char *key_str = (argc > 1) ? argv[1] : DEFAULT_KEY;
    const char *input_file = INPUT_FILE;
    FILE *fp;
    uint8_t *buffer = NULL;
    size_t file_size = 0;
    size_t bytes_read = 0;
    long pos;

    printf("==================================================\n");
    printf("RC4 File Encryptor (C Implementation)\n");
    printf("==================================================\n");
    printf("Input file:  %s\n", input_file);
    printf("Key:         %s\n", key_str);
    printf("--------------------------------------------------\n");

    /* Open file in binary mode for reading */
    fp = fopen(input_file, "rb");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open '%s' for reading.\n", input_file);
        fprintf(stderr, "Please create file.txt with some content to encrypt.\n");
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

    /* Read file contents */
    bytes_read = fread(buffer, 1, file_size, fp);
    fclose(fp);

    if (bytes_read != file_size) {
        fprintf(stderr, "Error: Failed to read complete file (read %zu of %zu bytes).\n",
                bytes_read, file_size);
        free(buffer);
        return 1;
    }

    printf("Original file size: %zu bytes\n", file_size);
    print_hex("Original content preview: ", buffer, file_size, 100);

    /* Encrypt the data */
    rc4((const uint8_t *)key_str, strlen(key_str), buffer, file_size);

    /* Write encrypted data back to file */
    fp = fopen(input_file, "wb");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open '%s' for writing.\n", input_file);
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
    printf("Encryption completed successfully!\n");
    printf("File '%s' now contains encrypted data.\n", input_file);

    /* Show encrypted preview */
    fp = fopen(input_file, "rb");
    if (fp) {
        buffer = (uint8_t *)malloc(file_size);
        if (buffer) {
            fread(buffer, 1, file_size, fp);
            fclose(fp);
            print_hex("Encrypted content preview: ", buffer, file_size, 100);
            free(buffer);
        }
    }

    return 0;
}