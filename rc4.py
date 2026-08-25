"""
RC4 Stream Cipher Implementation
"""
from typing import Union, ByteString


def rc4(key: Union[bytes, bytearray], data: Union[bytes, bytearray]) -> bytes:
    """
    RC4 stream cipher encryption/decryption.
    RC4 is symmetric - same function works for both encryption and decryption.
    
    Args:
        key: The encryption key (bytes)
        data: The data to encrypt/decrypt (bytes)
    
    Returns:
        Encrypted/decrypted data as bytes
    """
    if isinstance(key, str):
        key = key.encode()
    if isinstance(data, str):
        data = data.encode()
    
    # Key Scheduling Algorithm (KSA)
    S = list(range(256))
    j = 0
    key_len = len(key)
    
    for i in range(256):
        j = (j + S[i] + key[i % key_len]) % 256
        S[i], S[j] = S[j], S[i]
    
    # Pseudo-Random Generation Algorithm (PRGA)
    i = 0
    j = 0
    result = bytearray()
    
    for byte in data:
        i = (i + 1) % 256
        j = (j + S[i]) % 256
        S[i], S[j] = S[j], S[i]
        k = S[(S[i] + S[j]) % 256]
        result.append(byte ^ k)
    
    return bytes(result)


class RC4Cipher:
    """RC4 Cipher class for streaming encryption/decryption."""
    
    def __init__(self, key: Union[bytes, bytearray, str]):
        if isinstance(key, str):
            key = key.encode()
        self.key = bytes(key)
        self.reset()
    
    def reset(self):
        """Reset the cipher state for a new stream."""
        # Key Scheduling Algorithm (KSA)
        self.S = list(range(256))
        j = 0
        key_len = len(self.key)
        
        for i in range(256):
            j = (j + self.S[i] + self.key[i % key_len]) % 256
            self.S[i], self.S[j] = self.S[j], self.S[i]
        
        self.i = 0
        self.j = 0
    
    def crypt(self, data: Union[bytes, bytearray]) -> bytes:
        """Encrypt or decrypt data (RC4 is symmetric)."""
        if isinstance(data, str):
            data = data.encode()
        
        result = bytearray()
        for byte in data:
            self.i = (self.i + 1) % 256
            self.j = (self.j + self.S[self.i]) % 256
            self.S[self.i], self.S[self.j] = self.S[self.j], self.S[self.i]
            k = self.S[(self.S[self.i] + self.S[self.j]) % 256]
            result.append(byte ^ k)
        
        return bytes(result)
    
    def encrypt(self, data: Union[bytes, bytearray]) -> bytes:
        """Encrypt data."""
        return self.crypt(data)
    
    def decrypt(self, data: Union[bytes, bytearray]) -> bytes:
        """Decrypt data (same as encrypt for RC4)."""
        return self.crypt(data)


# Standalone functions for simple use
def encrypt(key: Union[bytes, bytearray, str], data: Union[bytes, bytearray]) -> bytes:
    """Encrypt data using RC4."""
    return rc4(key, data)


def decrypt(key: Union[bytes, bytearray, str], data: Union[bytes, bytearray]) -> bytes:
    """Decrypt data using RC4 (same as encrypt)."""
    return rc4(key, data)


if __name__ == "__main__":
    # Test the RC4 implementation
    key = b"SecretKey123"
    plaintext = b"Hello, World! This is a test message for RC4 encryption."
    
    print(f"Original: {plaintext}")
    
    # Test using function
    encrypted = encrypt(key, plaintext)
    print(f"Encrypted: {encrypted.hex()}")
    
    decrypted = decrypt(key, encrypted)
    print(f"Decrypted: {decrypted}")
    
    assert decrypted == plaintext, "Decryption failed!"
    print("Function test passed!")
    
    # Test using class
    cipher = RC4Cipher(key)
    encrypted2 = cipher.encrypt(plaintext)
    print(f"\nClass encrypted: {encrypted2.hex()}")
    
    cipher.reset()
    decrypted2 = cipher.decrypt(encrypted2)
    print(f"Class decrypted: {decrypted2}")
    
    assert decrypted2 == plaintext, "Class decryption failed!"
    print("Class test passed!")