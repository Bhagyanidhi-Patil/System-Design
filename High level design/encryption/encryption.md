# 🔐 Symmetric vs Asymmetric Encryption

---

## 🔐 Symmetric Encryption

Uses **one same key** for both encryption and decryption.  
Both sender and receiver share the same secret key.

### 📌 Example
You lock a box with a key 🔑 and send it to someone who uses the same key to open it.

### 🧱 Common Algorithms
- AES
- DES (older)
- ChaCha20

### ✅ Pros
- Fast
- Good for large data encryption

### ❌ Cons
- Key sharing is risky
- If key is leaked, security is broken

---

## 🔑 Asymmetric Encryption

Uses **two different keys**:
- Public Key (for encryption)
- Private Key (for decryption)

### 📌 Example
- Anyone can lock a box using your public key 🔓
- Only you can open it using your private key 🔐

### 🧱 Common Algorithms
- RSA
- ECC (Elliptic Curve Cryptography)
- ElGamal

### ✅ Pros
- More secure key exchange
- No need to share private key

### ❌ Cons
- Slower than symmetric encryption

---

## 🔄 Symmetric vs Asymmetric Encryption

| Feature | 🔐 Symmetric Encryption | 🔑 Asymmetric Encryption |
|--------|--------------------------|---------------------------|
| Keys Used | One shared secret key | Public + Private key |
| Encryption | Same key used | Public key used |
| Decryption | Same key used | Private key used |
| Key Sharing | Must be shared securely | Public key can be shared openly |
| Security Risk | If key leaks → insecure | Private key remains safe |
| Speed | Fast | Slower |
| Use Case | File encryption, disk encryption | HTTPS, email encryption, key exchange |

---

## 🔐 Symmetric Encryption Process

1. Key Generation: Alice and Bob share a secret key  
2. Encryption: Alice encrypts message using shared key  
3. Transmission: Encrypted message sent to Bob  
4. Decryption: Bob decrypts using same key  

🧱 Tools: AES, DES, ChaCha20  

---

## 🔑 Asymmetric Encryption Process

1. Key Generation: Bob creates public/private key pair  
2. Public Key Sharing: Bob shares public key with Alice  
3. Encryption: Alice encrypts message using public key  
4. Transmission: Message sent to Bob  
5. Decryption: Bob decrypts using private key  

🧱 Tools: RSA, ECC, ElGamal  

---

<p align="center">
  <img src="image.png" width="800"/>
</p>

<p align="center">
  <img src="image-1.png" width="800"/>
</p>

<p align="center">
  <img src="image-2.png" width="800"/>
</p>