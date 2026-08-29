# Access Token vs Refresh Token

Access tokens and refresh tokens are two different credentials used together in modern authentication (especially OAuth 2.0). They solve different problems.
## 🔐 First: What is a token?

When you log in to an application, you provide something like:
```
Username + Password
        ↓
      Login
        ↓
    Server verifies
        ↓
   Tokens are given
```
Instead of sending your password with every API request, the server gives you a token.

The token basically says:

`This user has already authenticated. You can trust this request.`

There are two important types:

- Access Token
- Refresh Token
---

## Access Token

An access token is a credential that a client sends with API requests to prove that the user is authenticated.

### Characteristics

- **Short-lived** (e.g., 5–60 minutes)
- Contains user identity and permissions
- **Where it goes:** Usually sent on every API request
- **Typical format:** Often a JWT (JSON Web Token), but not always

### Example

An access token is used to `access protected APIs/resources.`

**For example**, suppose you log into Instagram.

The application might make requests like:
```
GET /profile
GET /messages
POST /upload
GET /followers
```
These APIs need authentication.

So the client sends:

`Authorization: Bearer <access_token>`

The server checks the access token and says:
```
Token valid?
     ↓
   YES
     ↓
Allow request
```

### Why Short-Lived?

If stolen, the attacker has limited time to use it.

- **Risk window** = token lifetime
- A 15-minute stolen token is much less dangerous than a token valid for 30 days

---

## Refresh Token

Now imagine your access token expires every 15 minutes.

`Would you want to enter your password every 15 minutes?`

**Obviously not 😄**

That's where the refresh token comes in.

`The refresh token is a longer-lived credential that is used to obtain a new access token.`

### Characteristics

- **Long-lived** — days, weeks, or months (sometimes with rotation)
- **Used for:** Talking to the auth server's token endpoint — not your normal business APIs
- **More sensitive** than access tokens

### Why It Exists

Without it, users would have to log in again every time the access token expires. Refresh tokens keep sessions alive without re-entering passwords.

### Security

Refresh tokens should be stored carefully:

- **Web:** `httpOnly` secure cookie
- **Mobile:** Secure storage (Keychain / Keystore)

Never send refresh tokens to random third-party APIs.

---

## Why Not Just Use a Long-Lived Access Token?

Interviewers often ask this.

### Bad Design

```text
Access Token = 30 days
```

**Problems:**

- If stolen, attacker gets 30 days of access
- Hard to revoke
- Larger security risk

### Better Design

```text
Access Token  = 15 min
Refresh Token = 30 days
```

**Benefits:**

- Small attack window
- Better security
- Better user experience

---

## What Happens If Tokens Are Stolen?

### Attacker Steals an Access Token (15 min)

They can use the API until it expires.

After 15 minutes:

```http
401 Unauthorized
```

Attack ends.

### Attacker Steals a Refresh Token (30 days)

They can continuously obtain new access tokens:

```text
Refresh Token
    ↓
New Access Token
    ↓
Refresh Token
    ↓
New Access Token
```

Potentially for weeks.

That's why refresh tokens require extra protections (rotation, secure storage, revocation).

---

## Summary

| | Access Token | Refresh Token |
|---|---|---|
| **Lifetime** | Short (minutes) | Long (days/weeks) |
| **Sent to** | Business APIs | Auth server only |
| **Purpose** | Prove identity per request | Get new access tokens |
| **If stolen** | Limited damage window | Can renew access for weeks |

---

## Flow Diagram

<div align="center">
  <img src="auth-flow.png" alt="Flow Diagram" width="700">
</div>

## What if refresh tokens itself gets stolen?
This is the dangerous case. If a refresh token is stolen, an attacker may be able to keep getting new access tokens, potentially even after the original access token expires.

### 🛡️ How Do Systems Protect Against a Stolen Refresh Token?

Modern authentication systems use several techniques.

**1. Refresh-token rotation**

#### 🔄 What is Refresh Token Rotation?

Normally, you might have:

```text
Access Token  → 15 minutes
Refresh Token → 30 days
```

When the access token expires, the client sends the refresh token to get a new access token.

With **refresh token rotation**, the server also gives you a **new refresh token** and immediately invalidates the old one.

#### Without Rotation

```text
Refresh Token A
       ↓
   Refresh request
       ↓
New Access Token
       ↓
Refresh Token A is still valid
```

The same refresh token can potentially be used again and again.

That's risky if it gets stolen.

---

#### With Rotation

Suppose you initially have:

```text
Refresh Token A
```

The access token expires.

You send:

```text
Refresh Token A
        ↓
   Authentication Server
```

The server responds:

```text
Access Token B
Refresh Token B
```

And **Refresh Token A is now invalid**.

```text
Refresh Token A → ❌
Refresh Token B → ✅
```

Next time:

```text
Refresh Token B
        ↓
   Authentication Server
        ↓
Access Token C
Refresh Token C

Refresh Token B → ❌
Refresh Token C → ✅
```

So the refresh token keeps **rotating**:

```text
RT-A
 ↓
RT-B
 ↓
RT-C
 ↓
RT-D
 ↓
RT-E
```

That's where the name **refresh token rotation** comes from.


---

**2. Store refresh tokens securely**

For web applications, refresh tokens are commonly kept in a **Secure + HttpOnly cookie**. This helps prevent JavaScript from directly reading the token.

```text
Browser
  |
  | HttpOnly cookie
  ↓
Refresh Token
```

So malicious JavaScript cannot simply do:

```javascript
document.cookie
```

to retrieve an HttpOnly cookie.

---

**3. Server-side revocation**

The server can maintain refresh-token/session state:

```text
User 123
   |
   └── Refresh Token → Active
```

If the user logs out, changes their password, or suspicious activity is detected:

```text
Refresh Token → Revoked ❌
```

Then even if an attacker has the stolen token:

```text
Attacker
   ↓
Refresh Token
   ↓
Auth Server
   ↓
Revoked ❌
```

No new access token is issued.

---

**4. Expiration**

Refresh tokens aren't necessarily valid forever.

For example:

```text
Access token  → 15 minutes
Refresh token → 30 days
```

After 30 days, the user has to authenticate again.



