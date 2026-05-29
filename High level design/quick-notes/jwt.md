# JWT (JSON Web Token)

Think of JWT as the “identity card” a client carries after login.

## Big Picture

A user first proves identity using:

- email/password
- Google login
- OTP
- etc.

This step is called **Authentication**

“Who are you?”

After successful authentication, the server gives the client a JWT token.

Then for every future request:

- Client → sends JWT → Server verifies JWT → gives response

So JWT is mainly used for:

- Stateless Authentication

## Stateless Means

- Server does NOT remember previous requests from the client.
- Every request should contain all required information.
- Server does NOT store login session in memory/database for every user.
- Instead:
  - the token itself contains user identity information
  - server only verifies the token signature

## Real World Analogy

Imagine airport security.

### Step 1 — Verify Identity

- You show passport.
- Airport checks you.

### Step 2 — Get Boarding Pass

- Now airport gives boarding pass.

### Step 3 — Move Around

- Every checkpoint only checks boarding pass.
- They don’t repeatedly ask for passport.

**JWT = boarding pass.**

## Complete JWT Authentication Flow

### Step 1 — User Logs In

Client sends credentials:

```http
POST /login
{
  "email": "john@gmail.com",
  "password": "123456"
}
```

### Step 2 — Server Verifies Credentials

Server checks:

- email exists?
- password correct?

If valid:

- Server creates JWT.

### Step 3 — Server Generates JWT

JWT usually contains:

```json
{
  "userId": 101,
  "role": "admin",
  "exp": 1712345678
}
```

Then server signs it using a secret key.

Example:

```text
HMACSHA256(
  header.payload,
  secret_key
)
```

Now token becomes:

```text
xxxxx.yyyyy.zzzzz
```

### Step 4 — Server Sends JWT to Client

```json
{
  "token": "eyJhbGciOiJIUzI1NiIs..."
}
```

Client stores it in:

- localStorage
- sessionStorage
- HTTP-only cookie (best)

### Step 5 — Client Sends JWT in Every Request

```http
GET /profile
Authorization: Bearer eyJhbGc...
```

This is the MOST IMPORTANT PART.

JWT is used AFTER authentication for authorization requests.

### Step 6 — Server Verifies JWT

Server checks:

- token signature valid?
- token expired?
- token tampered?

If valid:

- Request allowed

Else:

- `401 Unauthorized`

## Internal Structure of JWT

JWT has 3 parts:

- `header`
- `payload`
- `signature`

Example:

```text
aaaa.bbbb.cccc
```

### A) Header

Contains algorithm info.

```json
{
  "alg": "HS256",
  "typ": "JWT"
}
```

### B) Payload

Contains claims/data.

```json
{
  "userId": 101,
  "role": "admin",
  "exp": 1712345678
}
```

This is NOT encrypted.

It is only Base64 encoded.

So never store inside JWT:

- passwords
- secrets
- sensitive info

### C) Signature

Generated using:

- `header + payload + secret`

Purpose:

- Prevent tampering

If attacker changes payload, for example:

```json
"role": "admin"
```

then the signature becomes invalid and the server rejects the token.

## Why JWT Became Popular

Traditional session auth works like this:

- Session ID → server DB lookup every request

JWT avoids DB lookup.

### Benefits

#### A) Stateless

- Server need not store sessions.
- Great for:
  - microservices
  - distributed systems
  - scalable architectures

#### B) Horizontal Scaling Easier

- If 10 servers exist, any server can verify JWT.
- No need for shared session storage.

#### C) Faster

- No session DB/cache lookup.

#### D) Works Well with APIs

Especially:

- mobile apps
- React frontend
- Angular frontend
- SPAs
- microservices

“JWT enables stateless authentication.”

### Session vs JWT Flow

- Traditional Session Flow:
  - Client → Session ID
  - Server → checks Redis/session DB
- JWT Flow:
  - Client → JWT
  - Server → verifies signature locally

No centralized session store needed.

Huge scalability advantage.

## Access Token vs Refresh Token

### Problem
```
- If JWT expires quickly:
  - User logs out frequently.
- If JWT expires very late:
  - Security risk increases.
```
### Solution

Use:
```
- Access Token
- Refresh Token
```
#### Access Token
```
- Short expiry.
- Example: 15 mins
- Used in API calls.
```
#### Refresh Token
```
- Long expiry.
- Example: 7 days
- Used ONLY to get new access tokens.
```
### Flow
```
1. Login
2. Get access + refresh token
3. Access token expires
4. Client sends refresh token
5. Server issues new access token
```

## Where to Store JWT
JWT can be stored in different places on the client side.

The two most common are:
```
localStorage
HTTP-only cookies
```
### Option 1 — localStorage

- Frontend stores token like:

- localStorage.setItem("token", jwt)

- Later sends it manually:

- Authorization: Bearer JWT_TOKEN

***“JS code” means JavaScript running on the client side***
```
Flow:

Login
  ↓
Server sends JWT
  ↓
Frontend stores in localStorage
  ↓
Frontend manually attaches token
```
#### Problem

JavaScript can access localStorage.

So if attacker injects malicious JS (XSS attack):

localStorage.getItem("token")

Token can be stolen.

---

### Option 2 — HTTP-only Cookies (Preferred)
Server sends:
```
Set-Cookie: token=JWT_TOKEN;
HttpOnly;
Secure;
SameSite=Strict
```
- Browser stores it automatically.
- JavaScript CANNOT access it.

#### What “HttpOnly” Means
```
JavaScript cannot read this cookie

So this WON’T work:
document.cookie
Token remains hidden from JS.
This protects against many XSS attacks.
```
#### Then How Is JWT Sent?

Browser automatically sends cookie:
```
Cookie: token=JWT_TOKEN
```
just like session cookies.

### Browser = Web Client

In this context.

So when I say:
```
Browser stores cookie automatically
```
it means:
```
The web client (Chrome/Firefox/etc.) handles cookie storage.
```
---

## JWT Security Best Practices

### A) Always Use HTTPS
```
Otherwise token can be intercepted.
```
### B) Keep Access Token Short-Lived

Example:
```
- 15 mins
```
Limits damage if stolen.

### C) Use Refresh Tokens
```
Industry standard.
```
### D) Never Store Sensitive Data in JWT
```
- JWT is readable.
```
## Biggest Drawback of JWT

JWT is stateless.

Which means:
```
- Revoking tokens becomes difficult.
```
Suppose token valid for 7 days.
```
User logs out.
```
Server still cannot invalidate token easily unless:
```
- blacklist maintained
- expiry short
- revocation system added
```
This is the biggest tradeoff.

## Architecture Diagram (Mental Model)

```text
        LOGIN
Client -----------> Auth Server
                     |
                     | verify password
                     |
                     V
                 Generate JWT
                     |
                     V
Client <----------- JWT

--------------------------------

Future Requests

Client ---- JWT ----> API Server
                     |
                     | verify signature
                     |
                     V
                  Response
```

## Session-Based Authentication = Stateful Authentication

Because:

- Server stores user session data somewhere after login.

## How Session Authentication Works

### Step 1 — User Logs In

```http
POST /login
```

User sends:

```json
{
  "email": "john@gmail.com",
  "password": "123456"
}
```

### Step 2 — Server Verifies User

If credentials correct:

- Server creates a session.

Example:

```text
Session ID = XYZ123
```

Server stores:

```text
XYZ123 → User John
```

inside:

- memory
- Redis
- database

### Step 3 — Server Sends Session ID

Usually through cookie:

```http
Set-Cookie: sessionId=XYZ123
```

Browser stores it.

### Step 4 — Future Requests

Browser/client automatically sends:

```http
Cookie: sessionId=XYZ123
```

Server checks storage:

```text
XYZ123 → John
```

Then allows request.

### Why It Is Called “Stateful”

Because server maintains user state.

It remembers:

- who logged in
- active sessions
- session mappings

Example:

```text
SessionID → User Data
```

That stored information is called:

- Server State

Hence:

- Stateful Authentication

## Browser vs Client Note

In most of the explanations, when I said “browser”, you can generally think of it as:

- Client

But with one important nuance:

When I specifically said “browser”, I was referring to:

- automatic cookie storage
- automatic cookie sending
- session handling behavior

Because these are browser features.

Example:

```http
Cookie: sessionId=ABC123
```

Browser automatically manages this.