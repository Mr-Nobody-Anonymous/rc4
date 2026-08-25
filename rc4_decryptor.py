#!/usr/bin/env python3
"""
RC4 File Decryptor
Decrypts the contents of file.txt using RC4 stream cipher.
"""
import sys
import os
from typing import Union
from rc4 import rc4


def decrypt_file(input_file: str, output_file: str, key: Union[bytes, str]) -> bool:
    """
    Decrypt a file using RC4.
    
    Args:
        input_file: Path to the encrypted input file
        output_file: Path to the output file (can be same as input for in-place)
        key: Decryption key (bytes or string) - must match encryption key
    
    Returns:
        True if successful, False otherwise
    """
    try:
        # Read the encrypted file in binary mode
        with open(input_file, 'rb') as f:
            encrypted_data = f.read()
        
        print(f"Read {len(encrypted_data)} encrypted bytes from {input_file}")
        
        # Convert key to bytes if string
        if isinstance(key, str):
            key = key.encode()
        
        # Decrypt the data (RC4 is symmetric)
        decrypted_data = rc4(key, encrypted_data)
        
        # Write decrypted data to output file
        with open(output_file, 'wb') as f:
            f.write(decrypted_data)
        
        print(f"Written {len(decrypted_data)} decrypted bytes to {output_file}")
        return True
    
    except FileNotFoundError:
        print(f"Error: File '{input_file}' not found.")
        return False
    except PermissionError:
        print(f"Error: Permission denied accessing '{input_file}' or '{output_file}'.")
        return False
    except Exception as e:
        print(f"Error during decryption: {e}")
        return False


def verify_files(file1: str, file2: str) -> bool:
    """
    Verify two files have identical content.
    
    Args:
        file1: Path to first file
        file2: Path to second file
    
    Returns:
        True if files are identical, False otherwise
    """
    try:
        with open(file1, 'rb') as f1, open(file2, 'rb') as f2:
            data1 = f1.read()
            data2 = f2.read()
        
        return data1 == data2
    except Exception as e:
        print(f"Error during verification: {e}")
        return False


def main():
    # Configuration
    ENCRYPTED_FILE = "file.txt"      # The encrypted file
    DECRYPTED_FILE = "file_decrypted.txt"  # Output decrypted file
    KEY = "MySecretKey2026"          # Must match encryption key
    ORIGINAL_FILE = "file.txt.backup"  # Backup of original for verification (optional)
    
    print("=" * 50)
    print("RC4 File Decryptor")
    print("=" * 50)
    print(f"Encrypted file:  {ENCRYPTED_FILE}")
    print(f"Decrypted file:  {DECRYPTED_FILE}")
    print(f"Key:             {KEY}")
    print("-" * 50)
    
    # Check if encrypted file exists
    if not os.path.exists(ENCRYPTED_FILE):
        print(f"Error: {ENCRYPTED_FILE} does not exist.")
        print("Please run the encryptor first to create an encrypted file.")
        sys.exit(1)
    
    # Show encrypted content preview
    with open(ENCRYPTED_FILE, 'rb') as f:
        encrypted = f.read()
    
    print(f"Encrypted file size: {len(encrypted)} bytes")
    if len(encrypted) > 0:
        preview = encrypted[:100]
        print(f"Encrypted content preview: {preview!r}")
        if len(encrypted) > 100:
            print("  ... (truncated)")
    
    # Decrypt the file
    success = decrypt_file(ENCRYPTED_FILE, DECRYPTED_FILE, KEY)
    
    if success:
        print("-" * 50)
        print("Decryption completed successfully!")
        
        # Show decrypted content preview
        with open(DECRYPTED_FILE, 'rb') as f:
            decrypted = f.read()
        
        print(f"Decrypted file size: {len(decrypted)} bytes")
        if len(decrypted) > 0:
            preview = decrypted[:100]
            print(f"Decrypted content preview: {preview!r}")
            if len(decrypted) > 100:
                print("  ... (truncated)")
        
        # Verify against original if backup exists
        if os.path.exists(ORIGINAL_FILE):
            print("-" * 50)
            print("Verifying against original backup...")
            if verify_files(ORIGINAL_FILE, DECRYPTED_FILE):
                print("✓ VERIFICATION PASSED: Decrypted file matches original exactly!")
            else:
                print("✗ VERIFICATION FAILED: Decrypted file does NOT match original!")
                sys.exit(1)
        else:
            print("-" * 50)
            print("Note: No original backup found for verification.")
            print(f"Create a backup before encryption: cp file.txt {ORIGINAL_FILE}")
    
    else:
        print("-" * 50)
        print("Decryption failed!")
        sys.exit(1)


if __name__ == "__main__":
    # Allow key to be passed as command line argument
    if len(sys.argv) > 1:
        KEY = sys.argv[1]
        print(f"Using key from command line: {KEY}")
    
    main()