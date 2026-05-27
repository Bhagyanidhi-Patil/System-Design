# API Versioning

API versioning is a way to manage changes to an API over time without breaking applications that already use it.

Think of an API like a restaurant menu used by many customers (apps). If the restaurant suddenly renames dishes or changes ingredients, regular customers get confused. Versioning lets the restaurant introduce a “new menu edition” while keeping the old one available for existing customers.

## Why API Versioning Exists

When APIs evolve, changes can break existing clients.

### Examples of breaking changes

- Renaming a field
- Removing an endpoint
- Changing response structure
- Changing authentication rules
- Altering data types

### Without versioning

- Old mobile apps may stop working
- Third-party integrations can fail
- Frontend/backend compatibility breaks

### Versioning allows

- Backward compatibility
- Gradual migration
- Safer deployments
- Multiple client generations to coexist

## Simple Example

### Version 1

**Request:**

```http
GET /api/v1/users/42
```

**Response:**

```json
{
  "name": "Alice"
}
```

### Version 2

Later you need more structured data.

**Request:**

```http
GET /api/v2/users/42
```

**Response:**

```json
{
  "firstName": "Alice",
  "lastName": "Johnson"
}
```

Old apps keep using `v1`. New apps move to `v2`.

## Common API Versioning Methods

### 1. URL Versioning (Most Common)

Version is placed in the URL.

- `/api/v1/products`
- `/api/v2/products`

**Advantages**

- Simple
- Easy to understand
- Easy routing
- Popular in REST APIs

**Disadvantages**

- URL changes
- Can duplicate routes

Used by many public APIs.

### 2. Header Versioning

Version is passed in headers.

```http
GET /api/products
Accept: application/vnd.myapi.v2+json
```

or

```http
API-Version: 2
```

**Advantages**

- Cleaner URLs
- More RESTful

**Disadvantages**

- Harder to test manually
- Less visible

### 3. Query Parameter Versioning

```http
/api/products?version=2
```

**Advantages**

- Easy to implement

**Disadvantages**

- Less standard
- Often discouraged for major versions

### 4. Content Negotiation

Client requests a specific representation:

```http
Accept: application/json;version=2
```

Common in enterprise APIs.

## Types of API Changes

### Non-Breaking Changes

Usually do NOT require a new version.

**Examples:**

- Adding optional fields
- Adding new endpoints
- Improving performance
- Adding pagination metadata

Example:

```json
{
  "name": "Alice",
  "email": "alice@example.com"
}
```

If old clients ignore unknown fields, this is safe.

### Breaking Changes

Usually REQUIRE a new version.

**Examples:**

- Removing fields
- Renaming fields
- Changing data format
- Changing required parameters

Example:

```json
{
  "fullName": "Alice Johnson"
}
```

If old clients expect `name`, they break.

## Semantic Versioning vs API Versioning

People often confuse these.

### Semantic Versioning (SemVer)

Used for software packages:

- `MAJOR.MINOR.PATCH`
- `2.4.1`

- `MAJOR` → breaking changes
- `MINOR` → new backward-compatible features
- `PATCH` → bug fixes

### API Versioning

Usually simpler:

- `v1`
- `v2`
- `v3`

Some APIs use:

- `v1.2`

But many avoid overly detailed API versions.