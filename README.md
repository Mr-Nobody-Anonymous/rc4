# RC4 Stream Cipher — File Encryption Assignment

Implementation of the **RC4 stream cipher** in both **C** and **Python**, with command-line tools to encrypt and decrypt files.

---

## Table of Contents
- [How RC4 Works](#how-rc4-works)
- [Project Structure](#project-structure)
- [Requirements](#requirements)
- [Building the C Implementation](#building-the-c-implementation)
- [Running the Python Implementation](#running-the-python-implementation)
- [Usage / Demo Walkthrough](#usage--demo-walkthrough)
- [Verification Workflow](#verification-workflow)
- [Library API](#library-api)
- [Security Notes](#-security-notes)

---

## How RC4 Works

RC4 is a symmetric stream cipher designed by Ron Rivest in 1987. It generates a
pseudo-random keystream that is XORed with the plaintext. Because XOR is its own
inverse, **the same function performs both encryption and decryption**.

### 1. Key Scheduling Algorithm (KSA)

Initializes a 256-byte permutation array `S` from the key:

```
for i from 0 to 255:
    S[i] = i
j = 0
for i from 0 to 255:
    j = (j + S[i] + key[i mod keylen]) mod 256
    swap(S[i], S[j])
```

### 2. Pseudo-Random Generation Algorithm (PRGA)

Produces one keystream byte per iteration and XORs it into the data:

```
i = 0, j = 0
for each byte of data:
    i = (i + 1) mod 256
    j = (j + S[i]) mod 256
    swap(S[i], S[j])
    K = S[(S[i] + S[j]) mod 256]
    output_byte = input_byte XOR K
```

The cipher state (`S`, `i`, `j`) persists across calls, enabling streaming use.

---

## Project Structure

```
rc4/
├── rc4.c              # Core RC4 algorithm (KSA + PRGA) — C implementation
├── rc4.h              # Public API header for the C library
├── rc4_encryptor.c    # CLI tool: encrypts file.txt in place (C)
├── rc4_decryptor.c    # CLI tool: decrypts to file_decrypted.txt (C)
├── rc4.py             # Core RC4 algorithm + RC4Cipher streaming class (Python)
├── rc4_encryptor.py   # CLI tool: encrypts file.txt in place (Python)
├── rc4_decryptor.py   # CLI tool: decrypts to file_decrypted.txt (Python)
├── Makefile           # Build automation for the C implementation
├── file.txt           # Sample input file (becomes encrypted after running)

---

## Requirements

| Component | Version |
|-----------|---------|
| GCC       | Any C99-compatible compiler |
| Python    | 3.6+ (no third-party packages needed) |
| Make      | Optional — for the Makefile |

Tested with GCC 15.2.0 (MSYS2) and Python 3.10.11 on Windows.

---

## Building the C Implementation

Using Make (recommended):

```bash
make            # builds rc4_encryptor.exe and rc4_decryptor.exe
```

Or manually with GCC:

```bash
gcc -O2 -Wall -std=c99 -o rc4_encryptor.exe rc4.c rc4_encryptor.c
gcc -O2 -Wall -std=c99 -o rc4_decryptor.exe rc4.c rc4_decryptor.c
```

Other useful targets:

```bash
make clean      # remove built executables
make test       # full round-trip demo (encrypt -> decrypt -> verify)
make help       # list all targets
```

## Running the Python Implementation

No compilation needed — scripts import `rc4` directly from this folder:

```bash
python rc4.py               # runs the built-in self-test
python rc4_encryptor.py     # encrypts file.txt in place
python rc4_decryptor.py     # decrypts to file_decrypted.txt
```

---

## Usage / Demo Walkthrough

Both toolchains work the same way. The default key is `MySecretKey2026`
(configured via the `DEFAULT_KEY` macro / `KEY` constant); you can override it
by passing the key as the first command-line argument.

**Important:** encryption modifies `file.txt` **in place**. Create a backup
first if you want to verify the round trip afterwards.

```bash
# 1. Back up the original file (used later for verification)
cp file.txt file.txt.backup

# --- C version ---
./rc4_encryptor.exe                # encrypts file.txt in place
./rc4_decryptor.exe                # writes file_decrypted.txt + verifies

# --- Python version ---
python rc4_encryptor.py
python rc4_decryptor.py

# Using a custom key (must match on both sides!)
./rc4_encryptor.exe "MyKey123"
./rc4_decryptor.exe "MyKey123"
```

### Example Output (C encryptor)

```
==================================================
RC4 File Encryptor (C Implementation)
==================================================
Input file:  file.txt
Key:         MySecretKey2026
--------------------------------------------------
Original file size: 392 bytes
Original content preview: ...
--------------------------------------------------
Encryption completed successfully!
File 'file.txt' now contains encrypted data.
Encrypted content preview: ...
```

### Example Output (C decryptor)

```
==================================================
RC4 File Decryptor (C Implementation)
==================================================
Encrypted file:  file.txt
Decrypted file:  file_decrypted.txt
Key:             MySecretKey2026
--------------------------------------------------
Encrypted file size: 392 bytes
...
--------------------------------------------------
VERIFICATION PASSED: Decrypted file matches original exactly!
```

---

## Verification Workflow

Because RC4 is symmetric, decrypting with the same key restores the original
bytes exactly. Both decryptors check this automatically when a backup exists:

```bash
cp file.txt file.txt.backup        # before encrypting
./rc4_encryptor.exe                # or: python rc4_encryptor.py
./rc4_decryptor.exe                # or: python rc4_decryptor.py
```

Expected result: `VERIFICATION PASSED: Decrypted file matches original exactly!`

If no backup exists, the decryptor still produces `file_decrypted.txt` but skips
the comparison step.

To restore the original file after an in-place encryption without decrypting:

```bash
cp file.txt.backup file.txt
```

You can also test the Python core library standalone (includes assertions):

```bash
python rc4.py          # prints "Function test passed!" and "Class test passed!"
```

---

## Library API

### C (`rc4.h`)

```c
#include "rc4.h"

/* One-shot convenience call */
uint8_t msg[] = "Hello";
rc4((uint8_t *)"key", 3, msg, 5);

/* Streaming style */
RC4_CTX ctx;
rc4_init(&ctx, key, key_len);
rc4_crypt(&ctx, chunk1, len1);   /* state carries over between calls */
rc4_crypt(&ctx, chunk2, len2);
```

### Python (`rc4.py`)

```python
from rc4 import rc4, RC4Cipher, encrypt, decrypt

# One-shot
cipher_text = rc4("MySecretKey2026", b"Hello")   # str keys are auto-encoded
plain_text  = rc4("MySecretKey2026", cipher_text)

# Streaming class
cipher = RC4Cipher(b"MySecretKey2026")
part1 = cipher.encrypt(b"First part ")
part2 = cipher.encrypt(b"second part")
cipher.reset()
cipher.decrypt(part1 + part2)                    # -> b"First part second part"
```

---

## 🔒 Security Notes

This assignment demonstrates *how* RC4 works, not how it should be used today.
Do **not** use RC4 to protect real data:

- **Biases in the keystream:** statistical flaws in RC4's output leak information about the plaintext.
- **Fluhrer–Mantin–Shamir (FMS) attack:** weak key-scheduling enables key recovery; variants like Klein's attack improve on it.
- **Deprecated everywhere:** prohibited in TLS (RFC 7465) and dropped from WEP/WPA successors after WEP was broken using these attacks.
- **No authentication:** like any raw stream cipher, ciphertexts can be bit-flipped undetectably (no MAC/AEAD).

For real applications use authenticated encryption such as **AES-GCM** or **ChaCha20-Poly1305**.

---

└── README.md          # This file
```

Generated at runtime (not committed):
`file.txt.backup`, `file_decrypted.txt`
