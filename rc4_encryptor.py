#!/usr/bin/env python3
"""
RC4 File Encryptor
Encrypts the contents of file.txt using RC4 stream cipher.
"""
import sys
import os
from typing import Union
from rc4 import rc4


def encrypt_file(input_file: str, output_file: str, key: Union[bytes, str]) -> bool:
    """
    Encrypt a file using RC4.
    
    Args:
        input_file: Path to the input file
        output_file: Path to the output file (can be same as input for in-place)
        key: Encryption key (bytes or string)
    
    Returns:
        True if successful, False otherwise
    """
    try:
        # Read the input file in binary mode
        with open(input_file, 'rb') as f:
            data = f.read()
        
        print(f"Read {len(data)} bytes from {input_file}")
        
        # Convert key to bytes if string
        if isinstance(key, str):
            key = key.encode()
        
        # Encrypt the data
        encrypted_data = rc4(key, data)
        
        # Write encrypted data to output file
        with open(output_file, 'wb') as f:
            f.write(encrypted_data)
        
        print(f"Written {len(encrypted_data)} encrypted bytes to {output_file}")
        return True
    
    except FileNotFoundError:
        print(f"Error: File '{input_file}' not found.")
        return False
    except PermissionError:
        print(f"Error: Permission denied accessing '{input_file}' or '{output_file}'.")
        return False
    except Exception as e:
        print(f"Error during encryption: {e}")
        return False


def main():
    # Configuration
    INPUT_FILE = "file.txt"
    OUTPUT_FILE = "file.txt"  # In-place encryption
    KEY = "MySecretKey2026"  # Change this key as needed
    
    print("=" * 50)
    print("RC4 File Encryptor")
    print("=" * 50)
    print(f"Input file:  {INPUT_FILE}")
    print(f"Output file: {OUTPUT_FILE}")
    print(f"Key:         {KEY}")
    print("-" * 50)
    
    # Check if input file exists
    if not os.path.exists(INPUT_FILE):
        print(f"Error: {INPUT_FILE} does not exist.")
        print("Please create file.txt with some content to encrypt.")
        sys.exit(1)
    
    # Show original content preview (first 100 bytes)
    with open(INPUT_FILE, 'rb') as f:
        original = f.read()
    
    print(f"Original file size: {len(original)} bytes")
    if len(original) > 0:
        preview = original[:100]
        print(f"Original content preview: {preview!r}")
        if len(original) > 100:
            print("  ... (truncated)")
    
    # Encrypt the file
    success = encrypt_file(INPUT_FILE, OUTPUT_FILE, KEY)
    
    if success:
        print("-" * 50)
        print("Encryption completed successfully!")
        print(f"File '{INPUT_FILE}' now contains encrypted data.")
        
        # Show encrypted content preview
        with open(OUTPUT_FILE, 'rb') as f:
            encrypted = f.read()
        preview = encrypted[:100]
        print(f"Encrypted content preview: {preview!r}")
        if len(encrypted) > 100:
            print("  ... (truncated)")
    else:
        print("-" * 50)
        print("Encryption failed!")
        sys.exit(1)


if __name__ == "__main__":
    # Allow key to be passed as command line argument
    import sys
    if len(sys.argv) > 1:
        # If argument provided, use it as key
        KEY = sys.argv[1]
        print(f"Using key from command line: {KEY}")
    
    main()