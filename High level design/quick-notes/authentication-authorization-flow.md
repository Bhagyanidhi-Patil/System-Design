# Authentication vs Authorization

Authentication and authorization are two core security concepts in software systems.

- **Authentication** = “Who are you?”
- **Authorization** = “What are you allowed to do?”

---

## High-Level Flow

Imagine logging into a banking app.

### Step-by-step

1. You open the app
2. You enter username/password
3. System verifies your identity
4. System creates a session/token
5. You request some resource
6. System checks permissions
7. Access is allowed or denied

---

## Authentication (Identity Verification)

Authentication confirms the user is genuine.

### Common Authentication Methods

**A. Username + Password**

Most common method.

**Example:**

- Username: `john@example.com`
- Password: `MySecret123`

### Internal Process

#### Step 1 — User submits credentials

```http
POST /login
{
  "email": "john@example.com",
  "password": "MySecret123"
}
```

#### Step 2 — Server finds user

Backend searches database:

```sql
SELECT * FROM users WHERE email='john@example.com';
```

#### Step 3 — Password hashing check

Passwords are NEVER stored as plain text.

Instead:

```text
Stored password = hash(password)
```

**Example:**

```text
"MySecret123"
→ bcrypt hash
→ $2a$10$asdkjfh...
```

When user logs in:

```text
Entered password
→ hash again
→ compare hashes
```

- If hashes match: ✅ authenticated
- Otherwise: ❌ invalid login

### Why Hash Passwords?

If database leaks:

- **BAD:**

```text
password = mypassword
```

- **GOOD:**

```text
password = hashed_random_data
```

**Common hashing algorithms:**

- bcrypt
- Argon2
- scrypt

**Modern password hashing always uses a salt.**

Without salt, password hashing is insecure against several attacks.

### What is Salt?

A salt is a random string added to the password before hashing.

Instead of:

```text
hash(password)
```

we do:

```text
hash(password + salt)
```

**Example:**

- Password = `mypassword`
- Salt = `X7$k9P`
- Combined = `mypasswordX7$k9P`

Then hashed.

### Why Salt is Important

#### Problem Without Salt

Suppose 2 users have the same password:

- `password123`

Without salt:

```text
hash(password123)
→ same hash for both users
```

Attacker can immediately detect:

- users sharing passwords
- common passwords

#### Salt Prevents This

With salt:

- User 1:

```text
password123 + A1B2
→ hash1
```

- User 2:

```text
password123 + X9Y8
→ hash2
```

Now hashes are completely different.

Even same passwords produce different hashes.

---

## Session Creation

After successful authentication, server must remember user.

Two common methods:

- Session-based authentication
- Token-based authentication (JWT)

### Why Sessions or Tokens?

Both session-based and token-based authentication are mechanisms used **AFTER** successful authentication to maintain the user's logged-in state across requests.

#### Authentication Process

User enters:

- email + password

Backend verifies:

- user exists
- password hash matches

If valid: ✅ **User is authenticated**

At this point server says: **"Okay, this user is genuine."**

#### The Stateless HTTP Problem

But **HTTP is stateless**.

**Meaning:**

- Each request is independent
- Server has no memory of previous requests

**So server needs a way to remember the user.**

That's where:

- Sessions
- Tokens

come in.

---

### Session-Based Authentication

Traditional web apps use sessions.

#### Flow

##### Step 1 — Login success

Server creates session:

```text
Session ID = abc123xyz
```

Stored in server memory/database:

```text
abc123xyz → User ID 45
```

##### Step 2 — Session cookie sent

```http
Set-Cookie: sessionId=abc123xyz
```

Browser stores cookie.

##### Step 3 — Future requests

Browser automatically sends:

```http
Cookie: sessionId=abc123xyz
```

##### Step 4 — Server validates

Server checks:

- Does session `abc123xyz` exist?

If yes: User is authenticated.

#### Session Architecture

```text
Browser
   ↓
Cookie (sessionId)
   ↓
Server
   ↓
Session Store
   ↓
User Data
```

#### Advantages of Sessions

- ✅ Easy logout
- ✅ Server controls sessions
- ✅ Can revoke anytime

#### Disadvantages

- ❌ Server must store sessions
- ❌ Harder to scale in distributed systems

---

### Token-Based Authentication (JWT)

Modern APIs/mobile apps commonly use JWT.

**JWT = JSON Web Token**

#### JWT Flow

##### Step 1 — Login

User sends credentials.

##### Step 2 — Server validates

If correct:

Server generates token:

```text
HEADER.PAYLOAD.SIGNATURE
```

**Example payload:**

```json
{
  "userId": 45,
  "role": "admin",
  "exp": 1717000000
}
```

##### Step 3 — Token sent to client

```json
{
  "token": "eyJhbGc..."
}
```

##### Step 4 — Client stores token

Usually:

- localStorage
- memory
- secure cookie

##### Step 5 — Client sends token

```http
Authorization: Bearer eyJhbGc...
```

##### Step 6 — Server validates token

Server checks:

- signature valid?
- expired?
- tampered?

If valid: Authenticated.

#### JWT Architecture

```text
Client
   ↓
JWT Token
   ↓
API Server
   ↓
Signature Verification
```

No session storage needed.

#### Advantages of JWT

- ✅ Stateless
- ✅ Scales well
- ✅ Good for microservices/mobile apps

#### Disadvantages

- ❌ Harder logout
- ❌ Token revocation difficult
- ❌ If stolen, attacker can use until expiry

---

## Logout Mechanisms

When a user logs out, the session or token becomes unusable.

### 1. Session-Based Authentication

Logout is very clean and immediate.

#### What happens?

Server deletes the session.

**Example:**

Before logout:

```text
sessionId abc123 → userId 45
```

After logout:

```text
sessionId abc123 → deleted
```

Browser may also delete the cookie.

#### Next Request

Browser sends:

```http
Cookie: sessionId=abc123
```

Server checks:

- session not found

**Result:**

- `401 Unauthorized`

Session becomes invalid immediately.

### 2. JWT Token-Based Authentication

JWT works differently.

Usually the server does **NOT** store JWTs.

So technically: **JWT itself does not automatically "die" on logout** unless special handling is implemented.

#### Common JWT Logout

Frontend deletes token.

**Example:**

```javascript
localStorage.removeItem("token")
```

or cookie deleted.

Now browser stops sending token.

**Effectively:** ✅ user logged out

#### Important Detail: Token Theft Risk

Suppose attacker copied the JWT before logout.

That token may still work until: **token expiry time reached**

because server may not know user logged out.

**Example Scenario:**

- JWT expires in: `1 hour`
- User logs out after: `10 minutes`
- Frontend deletes token
- But if attacker stole token: **token may still work for remaining 50 minutes** unless revocation mechanism exists

#### Why JWTs Use Expiry

JWT contains:

```json
{
  "exp": 1717000000
}
```

After expiry:

- Server rejects token

When the JWT (access token) expires:

- Server stops accepting it
- even if the user is still using the app
- So the system must issue a new token

---

## Authorization (Permission Checking)

After authentication comes authorization.

Now system checks:

- What can this user do?

### Example

Authenticated users:

| User | Role |
|------|------|
| John | Admin |
| Alice | Customer |

### Authorization Rules

#### Admin

- ✅ delete users
- ✅ manage system

#### Customer

- ✅ view profile
- ❌ delete users

### Authorization Flow

#### Step 1 — Request comes

```http
DELETE /users/45
```

#### Step 2 — Backend identifies user

From session/JWT:

- User = Alice
- Role = Customer

#### Step 3 — Permission check

Backend policy:

```python
if role != "admin":
    deny_access()
```

#### Step 4 — Response

- `403 Forbidden`

Authenticated but not allowed.

---

### Common Authorization Models

#### A. RBAC (Role-Based Access Control)

Most common.

Roles:

- admin
- editor
- customer

Permissions attached to roles.

**Example:**

- `admin` → create/delete users
- `editor` → edit articles
- `customer` → view products

#### B. ABAC (Attribute-Based Access Control)

Uses attributes:

- department
- location
- time
- ownership

**Example:**

Allow access only if:

```text
department == "finance" AND country == "India"
```

#### C. ACL (Access Control List)

Resource-specific permissions.

**Example:**

File A:

- John → read
- Alice → write

Used in file systems/cloud storage.

---

### Multi-Factor Authentication (MFA)

Extra security layer.

User needs:

- Password
- OTP/phone/app/biometric

**Example:**

Password + Google Authenticator code

Even if password leaks: attacker still blocked.