**Passwords should be hashed, not encrypted.**

* **Hashing** → One-way transformation. You cannot normally reverse the hash to get the original password.
* **Encryption** → Reversible using a key, so it is generally **not appropriate for storing passwords**.

### Example

```text
User enters:       MyPassword123
                       ↓
                   Password Hash
                       ↓
Stored in DB:      $argon2id$...
```

### During Login

```text
Entered password
       ↓
Hash it using the same password-hashing algorithm
       ↓
Compare with stored hash
       ↓
Match → Login successful
```

### Important

For real systems, don't use plain SHA-256 for passwords. Use a **password-specific hashing algorithm** such as:

* **Argon2id** — preferred modern choice
* **bcrypt**
* **scrypt**

These algorithms are intentionally slow and support **salting**, which makes password-cracking attacks much harder.

The key idea is: **the server does not decrypt the stored password. It hashes what you entered and compares the result.**

### Example

Suppose during signup you create:

```text
Password: hello123
```

The server generates a random **salt** and hashes it:

```text
hello123 + salt
       ↓
    Argon2id
       ↓
Stored hash: $argon2id$...xyz
```

The database stores the **salt + hash** (typically encoded together).

### Later, during login

You enter:

```text
hello123
```

The server:

```text
Entered password
       ↓
Reads salt from stored hash
       ↓
Hash entered password with that salt
       ↓
Gets calculated hash
       ↓
Compare calculated hash with stored hash
```

If they match:

```text
Calculated hash == Stored hash
        ↓
      ✅ Login
```

If you enter:

```text
hello124
```

you get a different hash:

```text
Calculated hash != Stored hash
        ↓
      ❌ Login failed
```
## DB won't store passowrd diretly , it just stores hash value , hash algo and salt then when we enter pasword how it knows which salt to check ?

- The important point is that **the salt is stored along with the password hash**.
- But DB also contains user id, email id along with hash, salt detais. When user enters password, it checks user id, email id and invoke data stored for that particular user. 
For example, the DB might have something conceptually like:

```text
user_id | password_hash
--------|--------------------------------------------
101     | $argon2id$v=19$m=65536,t=3,p=4$ABC123$XYZ789
```

That single stored value contains things like:

```text
Algorithm + parameters + salt + hash
```

### When you log in

You enter:

```text
Password = hello123
```

The server already knows **which user you're trying to log in as**, for example:

```text
email = bhoomika@gmail.com
```

So it first looks up that user's record:

```text
email → database → stored password hash
```

Then it extracts the salt and hashing parameters from the stored value:

```text
Stored value
     ↓
algorithm = Argon2id
salt = ABC123
parameters = ...
hash = XYZ789
```

Then:

```text
Entered password + ABC123
          ↓
       Argon2id
          ↓
    calculated hash
          ↓
Compare with XYZ789
```

If they match → ✅ **Password is correct.**

### Important clarification

It is not necessarily stored as separate columns:

```text
DB:
password_hash | salt
```

Often, password-hashing libraries encode the **algorithm, parameters, salt, and hash together** in the stored password-hash string.

So conceptually:

```text
DB
│
├── username/email
└── password_hash
      │
      ├── algorithm
      ├── parameters
      ├── salt
      └── hash
```

That's how the server knows which salt to use: **it retrieves the stored password-hash record for that specific user.**
