# Authentication vs Authorization

## Simple Definitions

**Authentication** = Checks if the user is a valid user (login step: username/password, OTP, etc.)

**Authorization** = After login, checks what the user is allowed to access or do.

Authorization is the process of checking what permissions a logged-in user has.  
It happens after authentication.

---

## Example

You log in → ✔ Authentication  

System checks:
- Can you view dashboard?
- Can you delete data?
- Can you access admin panel?

→ ✔ Authorization

---

## In Web Applications

### Authentication Technologies
- JWT
- OAuth
- OpenID Connect
- Multi-factor authentication (MFA)

### Authorization Models
- Role-Based Access Control (RBAC)
- Attribute-Based Access Control (ABAC)
- Permissions / Scopes

---

## Comparison Table

| Concept        | Authentication                    | Authorization                            |
|----------------|-----------------------------------|------------------------------------------|
| Purpose        | Verifies **who you are**          | Determines **what you can do**           |
| Main Question  | “Are you really this user?”       | “What is this user allowed to access?”   |
| Happens First? | Yes                               | After authentication                     |
| Example        | Logging in with username/password | Accessing admin dashboard permissions    |
| Common Methods | Passwords, biometrics, OTP, OAuth | Roles, permissions, access control lists |