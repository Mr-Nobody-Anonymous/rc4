# Makefile for RC4 File Encryption Assignment (C implementation)

CC      = gcc
CFLAGS  = -O2 -Wall -Wextra -std=c99

TARGETS = rc4_encryptor.exe rc4_decryptor.exe

.PHONY: all clean test help

all: $(TARGETS)

rc4_encryptor.exe: rc4.c rc4.h rc4_encryptor.c
	$(CC) $(CFLAGS) -o $@ rc4.c rc4_encryptor.c

rc4_decryptor.exe: rc4.c rc4.h rc4_decryptor.c
	$(CC) $(CFLAGS) -o $@ rc4.c rc4_decryptor.c

# Full round-trip demo: backup -> encrypt -> decrypt -> verify
test: all
	cp file.txt file.txt.backup
	./rc4_encryptor.exe
	./rc4_decryptor.exe

clean:
	rm -f $(TARGETS)

help:
	@echo "Targets:"
	@echo "  all    - build rc4_encryptor.exe and rc4_decryptor.exe (default)"
	@echo "  test   - backup, encrypt, decrypt and verify file.txt"
	@echo "  clean  - remove built executables"
	@echo "  help   - show this message"
