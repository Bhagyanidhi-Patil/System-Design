# Design Pastebin 

### Introduction

Pastebin is a web service that allows users to store text or code snippets and share them using a unique short URL. Users can create a paste, receive a short link, and anyone with that link can access the content. The system should be highly available, scalable, and optimized for read-heavy traffic.

**Example URLs:**

* pastebin.com/Ab3KxP
* pastebin.com/Xy91Qa

---

## 1. Functional Requirements

The system should support the following features:

* Users can create a text or code snippet.
* Generate a unique short URL for every paste.
* Anyone with the URL can view the paste.
* Support optional expiration time.
* Support optional password protection.
* Support syntax highlighting for different programming languages.
* Allow users to delete a paste.
* Support both anonymous and registered users.

---

## 2. Non-Functional Requirements

The system should satisfy the following qualities:

* High Availability
* High Scalability
* Low Latency
* Durable Storage
* Unique URL Generation
* Read-heavy Optimization
* Fault Tolerance

---

## 3. Capacity Estimation

Assume the following:

* 10 million new pastes are created every day.
* Read to Write ratio = 100 : 1.

### Write Traffic

10,000,000 ÷ 86,400 ≈ 116 writes/second

Round it to **150 writes/second**.

### Read Traffic

150 × 100 = **15,000 reads/second**.

This shows that Pastebin is a **read-heavy system**, so optimizing read performance using caching is essential.

### Storage Estimation

Average paste size = **10 KB**

Daily storage required:

10 Million × 10 KB = **100 GB/day**

Yearly storage:

100 GB × 365 ≈ **36 TB/year**

Since the storage requirement is very large, a **distributed storage system** is required to store paste contents efficiently.

---

## High Level Architecture

```
                Clients
                   |
             Load Balancer
                   |
        +----------+----------+
        |                     |
   Paste Service       Paste Service
        |                     |
        +----------+----------+
                   |
              Redis Cache
                   |
              Metadata DB
                   |
     Distributed Object Storage
```
---

#### `For a Pastebin System Design interview, this High-Level Architecture is the backbone of the entire system. Let's understand why each component exists, how it works, and what happens when a user creates or reads a paste.`

## High-Level Architecture Components of Pastebin

### 1. Clients

Clients are the users of the system.

#### Examples

* Browser
* Mobile App
* API Clients
* Command Line Tools

They perform operations such as:

* Create a paste
* Read a paste
* Delete a paste
* Update a paste (if allowed)

#### Example Requests

```text
POST /paste
```

or

```text
GET /abc123
```

The client never communicates directly with the database. Every request first goes to the **Load Balancer**, which forwards it to one of the Paste Service instances.

---

### 2. Load Balancer

```text
              Clients
                 |
          Load Balancer
            /        \
     Server 1      Server 2
```

#### Why Do We Need It?

Suppose **5 million users** access Pastebin simultaneously.

If there is only one server:

```text
Clients
   |
Server
```

that server will become overloaded and may crash.

Instead, multiple Paste Service instances are deployed behind a Load Balancer. The Load Balancer distributes incoming traffic across all available servers.

### Responsibilities

#### 1. Route Requests

Example:

User A

```text
GET /abc
```

is routed to

```text
Paste Service 1
```

User B

```text
POST /paste
```

is routed to

```text
Paste Service 2
```

This evenly distributes traffic and prevents any one server from becoming overloaded.

---

#### 2. Health Checks

Every few seconds, the Load Balancer checks whether each server is healthy.

```text
Load Balancer
      |
"Are you alive?"
```

Healthy server responds:

```text
200 OK
```

If a server fails to respond, it is considered unhealthy.

---

#### 3. Remove Unhealthy Servers

Before failure:

```text
Load Balancer
     |
     |---- Server 1
     |
     |---- Server 2
```

If Server 2 crashes:

```text
Load Balancer
     |
     |---- Server 1
```

The Load Balancer automatically stops sending requests to Server 2. Users continue using the system without noticing the failure.

#### Examples

* Nginx
* HAProxy
* AWS Application Load Balancer (ALB)

---

### 3. Paste Service

The Paste Service is the core backend application where all business logic is implemented.

It handles requests like:

```text
POST /paste

GET /abc

DELETE /paste
```

### Responsibilities

#### Create Paste

When a user sends:

```text
POST /paste
```

Example request body:

```text
Hello World
```

The Paste Service:

* Generates a unique Paste ID
* Generates a Short URL
* Stores metadata in the Metadata Database
* Stores the actual content in Object Storage
* Returns the generated Short URL

Example:

```text
pastebin.com/abX91
```

---

#### Read Paste

When the user requests:

```text
GET /abX91
```

The Paste Service:

* Checks Redis Cache
* If not found, reads metadata from the Metadata Database
* Reads content from Object Storage
* Returns the paste to the user

---

#### Delete Paste

```text
DELETE /paste
```

The service verifies:

* Is the requester the owner?
* Is the user authenticated?

If valid:

* Deletes metadata
* Deletes paste content

---

#### Validate Expiration

Suppose the user selects:

```text
Expire after 7 days
```

Metadata stores:

```text
Expiry: 2026-07-05
```

Whenever someone accesses the paste:

```text
Current Time > Expiry?
```

If true:

```text
404 Not Found
```

or

```text
Paste Expired
```

---

#### Password Protection

Suppose the user creates:

```text
Password = hello123
```

Instead of storing the password directly, the database stores:

```text
Hash(password)
```

When someone accesses the paste:

* User enters password
* Server hashes the entered password
* Compares hashes

If they match, access is granted.

---

#### Authentication

The service checks:

* Is the user logged in?
* Is the user anonymous?
* Is the requester the owner?

Only the owner is allowed to delete the paste.

---

#### Input Validation

The Paste Service validates:

* Maximum paste size
* Expiration period
* Programming language selection
* Empty content
* Invalid requests

---

### Why is Paste Service Stateless?

A stateless service does **not** store user session information in its own memory.

Example:

```text
Server 1

RAM
User123 Session
```

If Server 1 crashes, the session is lost.

Instead, all important information is stored in shared systems such as:

* Redis
* Metadata Database
* Object Storage

Therefore, every request can be handled by any server.

Example:

```text
Request 1 → Server 1

Request 2 → Server 2

Request 3 → Server 3
```

Since every server works independently, new servers can be added easily, enabling **horizontal scaling**.

---

### 4. Redis Cache

```text
Paste Service
      |
    Redis
      |
Metadata DB
```

Redis is an **in-memory cache**, making it much faster than querying a database.

Approximate latency:

* Redis: Microseconds to low milliseconds
* Database: Several milliseconds
* Disk/Object Storage: Even slower

### Why Cache?

Most requests are read requests.

Example:

A popular paste receives:

```text
500,000 views
```

Without Redis:

```text
Every Request
      |
Metadata DB
      |
Object Storage
```

This creates heavy load.

With Redis:

First request:

```text
Redis

Miss
  |
Metadata DB
  |
Store in Redis
  |
Return Response
```

Second request:

```text
Redis

Hit
  |
Return Response
```

No database lookup is needed.

---

### Cache Hit

```text
Redis
 |
Found
 |
Return Response
```

Very fast.

---

### Cache Miss

```text
Redis
 |
Not Found
 |
Metadata DB
 |
Store in Redis
 |
Return Response
```

Future requests become significantly faster.

---

### Benefits

#### Low Latency

Memory access is much faster than disk access.

#### Reduced Database Load

Instead of:

```text
1 Million Reads
      |
Database
```

Only a small fraction reaches the database.

Remaining requests are served directly from Redis.

#### Better User Experience

Frequently accessed pastes load almost instantly.

---

### 5. Metadata Database

The Metadata Database stores information **about** the paste, not the paste content itself.

Example schema:

| Field         | Purpose                    |
| ------------- | -------------------------- |
| PasteID       | Unique identifier          |
| ShortURL      | Short URL shown to users   |
| Creation Time | Creation timestamp         |
| Expiration    | Expiry time                |
| Password Hash | Hashed password            |
| Storage Path  | Location in Object Storage |
| Owner         | User ID                    |
| Views         | Number of views            |

### Why Not Store Large Paste Content Here?

Suppose someone uploads:

```text
500 MB Log File
```

If stored directly inside the database:

* Database size increases rapidly
* Backups become slower
* Queries become slower
* Storage becomes more expensive

Databases are optimized for structured metadata, not large files.

#### Suitable Databases

* PostgreSQL
* MySQL
* DynamoDB
* Cassandra

---

### 6. Distributed Object Storage

Object Storage stores the **actual paste content**.

Examples:

* Amazon S3
* HDFS
* Azure Blob Storage

### Why Object Storage?

Users may upload:

```text
100 KB
```

or

```text
500 MB
```

or

```text
2 GB Log File
```

Object Storage is specifically designed for storing massive files.

Benefits include:

* Massive scalability
* High durability
* Low storage cost
* Easy retrieval using object paths

---

### How Metadata Database and Object Storage Work Together

Metadata Database:

| PasteID | Storage Path             |
| ------- | ------------------------ |
| 12345   | /bucket/pastes/12345.txt |

Object Storage:

```text
/bucket/pastes/12345.txt

Hello World
This is my paste...
```

When someone opens the paste:

1. Paste Service finds the metadata using the Short URL.
2. Metadata provides the Storage Path.
3. Paste Service retrieves the content from Object Storage.
4. The response is returned to the user and may also be cached in Redis.

Keeping metadata and content separate makes the system scalable, efficient, and easier to maintain.

---

### Complete Request Flow

### Creating a Paste

```text
Client
   |
POST /paste
   |
Load Balancer
   |
Paste Service
   |
Generate Paste ID + Short URL
   |
Store Metadata → Metadata Database
   |
Store Content → Object Storage
   |
(Optional) Cache in Redis
   |
Return Short URL
```

---

### Reading a Paste

```text
Client
   |
GET /abc123
   |
Load Balancer
   |
Paste Service
   |
Check Redis
   |
   |-- Cache Hit
   |      |
   |      --> Return Paste
   |
   |-- Cache Miss
          |
          --> Read Metadata Database
          |
          --> Read Object Storage
          |
          --> Store in Redis
          |
          --> Return Paste
```
---

## API Design and Database Schema for Pastebin

- A well-designed API is essential for allowing clients (web browsers, mobile applications, or third-party services) to interact with the Pastebin system. 
- The API should be simple, RESTful, scalable, and easy to understand.

---

### 1. Create Paste API

This API is used to create a new paste.

### Endpoint

```http
POST /paste
```

### Request Body

```json
{
  "content": "Hello World",
  "expiry": "24h",
  "password": "abcd"
}
```

### Request Parameters

| Field    | Description                                                                                                                      |
| -------- | -------------------------------------------------------------------------------------------------------------------------------- |
| content  | The text or code that the user wants to store.                                                                                   |
| expiry   | Specifies when the paste should expire (e.g., 1 hour, 24 hours, 7 days, or Never).                                               |
| password | Optional password to protect the paste. The password is never stored in plain text. Instead, its hash is stored in the database. |

### What Happens Internally?

When the client sends the request:

1. The request reaches the Load Balancer.
2. The Load Balancer forwards it to one of the Paste Service instances.
3. The Paste Service validates the request.
4. A unique Paste ID is generated.
5. A unique Short URL (short_code) is generated.
6. The actual paste content is stored in Distributed Object Storage.
7. Metadata is stored in the Metadata Database.
8. The generated Short URL is returned to the client.

#### Response

```json
{
  "url": "pastebin.com/Ab3KxP"
}
```

The client can now share this URL with others.

---

### 2. Get Paste API

This API retrieves an existing paste.

#### Endpoint

```http
GET /Ab3KxP
```

Here, **Ab3KxP** is the unique short code generated during paste creation.

#### Internal Flow

1. Client sends the request.
2. Request reaches the Load Balancer.
3. The Load Balancer forwards it to a Paste Service.
4. Paste Service first checks Redis Cache.
5. If found (Cache Hit), the paste is immediately returned.
6. If not found (Cache Miss):

   * Read metadata from the Metadata Database.
   * Verify whether the paste has expired.
   * Verify password protection if enabled.
   * Fetch the actual content from Object Storage.
   * Store the result in Redis for future requests.
   * Return the paste to the client.

#### Response

```text
Hello World
```

If the paste has expired or does not exist, the server returns:

```http
404 Not Found
```

If the paste is password protected and the password is incorrect:

```http
401 Unauthorized
```

---

### 3. Delete Paste API

This API deletes an existing paste.

#### Endpoint

```http
DELETE /paste/{id}
```

Example

```http
DELETE /paste/12345
```

#### Internal Flow

1. Authenticate the user.
2. Verify that the user owns the paste.
3. Delete metadata from the Metadata Database.
4. Delete the actual content from Object Storage.
5. Remove any cached entries from Redis.
6. Return a success response.

#### Response

```http
200 OK
```

or

```json
{
    "message": "Paste deleted successfully."
}
```

---

### Database Schema

The Metadata Database stores only the metadata of the paste, while the actual content resides in Object Storage.

#### Paste Table

| Column        | Description                                                                         |
| ------------- | ----------------------------------------------------------------------------------- |
| id            | Unique identifier (Primary Key). Usually a UUID or auto-increment value.            |
| short_code    | The short URL used to access the paste (e.g., Ab3KxP). This field should be unique. |
| storage_path  | Path of the actual paste inside Object Storage (e.g., S3 bucket path).              |
| created_at    | Timestamp indicating when the paste was created.                                    |
| expire_at     | Timestamp after which the paste automatically expires.                              |
| password_hash | Secure hash of the password if password protection is enabled. Null otherwise.      |
| owner_id      | Identifier of the user who created the paste. Null for anonymous users.             |
| views         | Number of times the paste has been accessed.                                        |

---

### Why Store Metadata Separately?

Instead of storing the entire paste inside the database, only metadata is stored.

For example:

| id  | short_code | storage_path             |
| --- | ---------- | ------------------------ |
| 101 | Ab3KxP     | s3://pastebucket/101.txt |

Actual content:

```text
Hello World
```

is stored inside Object Storage.

This separation provides several advantages:

* Keeps the database lightweight.
* Faster database queries.
* Lower storage costs.
* Better scalability for large files.
* Easier backup and maintenance.

---

### URL Generation in Pastebin

URL generation is one of the most critical components of a Pastebin system because every paste must have a **unique, short, and easily shareable URL**.

For example:

```text
https://pastebin.com/Ab3KxP
```

Here, **Ab3KxP** is the **short code** that uniquely identifies the paste.

A good URL generation mechanism should satisfy the following requirements:

* **Unique** – Every paste should have a different URL.
* **Short** – URLs should be easy to share and remember.
* **Fast** – URL generation should happen in milliseconds.
* **Scalable** – It should support billions of pastes without running out of IDs.
* **Distributed** – Multiple servers should be able to generate URLs simultaneously without conflicts.

---

### Option 1: Auto-Increment ID + Base62 Encoding

The simplest approach is to use an auto-incrementing integer generated by the database.

Example:

```text
Paste 1

ID = 1

Paste 2

ID = 2

Paste 3

ID = 3

Paste 4

ID = 4
```

Instead of exposing numeric IDs directly, convert them into **Base62** strings.

For example:

```text
Decimal

125

↓

Base62

cb
```

The resulting short code becomes part of the URL:

```text
https://pastebin.com/cb
```

---

### How It Works

### Step 1

Database generates an ID.

Example:

```text
ID = 125
```

### Step 2

Convert the decimal number into Base62.

```text
125

↓

cb
```

### Step 3

Return the generated URL.

```text
https://pastebin.com/cb
```

Whenever a user accesses this URL:

```text
GET /cb
```

the service decodes **cb** back into:

```text
125
```

and retrieves the corresponding paste.

---

### Advantages

* Very simple implementation.
* No collisions because every database ID is unique.
* Generates short URLs.
* Fast lookup.
* Easy to decode back to the original ID.

---

### Disadvantages

The generated IDs are predictable.

For example:

```text
Paste 1

https://pastebin.com/a

Paste 2

https://pastebin.com/b

Paste 3

https://pastebin.com/c
```

An attacker could guess future or previous URLs simply by incrementing the short code.

This exposes information such as:

* Total number of pastes.
* Ability to crawl public pastes.

For privacy-sensitive systems, this approach is not ideal.

---

### Option 2: Random String Generation

Instead of sequential IDs, generate a completely random string.

Example:

```text
A3DxP9
```

Generated URL:

```text
https://pastebin.com/A3DxP9
```

---

### How It Works

#### Step 1

Generate a random 6-character string.

Example:

```text
A3DxP9
```

#### Step 2

Check the Metadata Database.

```text
Does A3DxP9 already exist?
```

If **No**:

Store the paste.

If **Yes**:

Generate another random string.

---

### Collision Handling

Suppose the generated code is:

```text
A3DxP9
```

and the database already contains the same code.

Generate another one:

```text
K8LpQ2
```

Since there are billions of possible combinations, collisions are extremely rare.

---

#### Advantages

* URLs are difficult to guess.
* Better security.
* Users cannot determine how many pastes exist.
* Easy to implement.

---

#### Disadvantages

* Every generated string requires a database lookup.
* Rare collisions require regeneration.
* Slightly slower than the auto-increment approach.

---

### Option 3: Distributed ID Generator

In distributed systems, many Paste Service instances create pastes simultaneously.

Example:

```text
            Load Balancer
                 |
       +---------+---------+
       |                   |
   Server A           Server B
       |                   |
   Create Paste       Create Paste
```

If each server independently generates IDs, duplicate IDs may occur.

To solve this problem, distributed systems use a **Distributed ID Generator**, such as **Snowflake IDs**.

---

### How It Works

A Snowflake ID contains multiple components.

Example:

```text
Timestamp

+

Machine ID

+

Sequence Number
```

This guarantees that every generated ID is globally unique.

Example:

```text
9156328741268
```

Convert this large number into Base62.

```text
9156328741268

↓

Kf92Ax
```

Generated URL:

```text
https://pastebin.com/Kf92Ax
```

---

### Advantages

* Works efficiently in distributed systems.
* No database coordination required for ID generation.
* No collisions.
* Extremely scalable.
* High throughput.

---

### Disadvantages

* More complex to implement.
* Requires synchronized clocks across servers.
* Additional infrastructure is needed.

---

### Why Do We Use Base62?

Suppose we expose numeric IDs directly.

Example:

```text
12548963
```

The URL becomes:

```text
https://pastebin.com/12548963
```

This is long and not user-friendly.

Instead, Base62 uses only URL-safe characters.

Character set:

```text
0 - 9

A - Z

a - z
```

Total characters:

```text
10 + 26 + 26 = 62
```

Since every character has 62 possible values, very large numbers can be represented using only a few characters.

---

### Example of Base62 Encoding

Suppose the generated numeric ID is:

```text
125
```

After Base62 encoding:

```text
125

↓

cb
```

Generated URL:

```text
https://pastebin.com/cb
```

Instead of storing:

```text
125
```

the system stores:

```text
cb
```

making the URL significantly shorter.

---

### Capacity of Base62

Suppose every URL contains **6 characters**.

Each position has:

```text
62 possibilities
```

Total combinations:

```text
62 × 62 × 62 × 62 × 62 × 62

=

62^6
```

Which is approximately:

```text
56 Billion
```

possible unique URLs.

This means that a **6-character Base62 short code can support approximately 56 billion unique pastes**, which is sufficient for most large-scale Pastebin systems.

If the system needs even more capacity in the future, the URL length can simply be increased to **7 or 8 characters**, exponentially increasing the number of possible combinations.

---

### Comparison of URL Generation Approaches

| Method                                        | Collision | Predictable | Database Lookup | Best Use Case                                            |
| --------------------------------------------- | --------- | ----------- | --------------- | -------------------------------------------------------- |
| Auto-Increment + Base62                       | No        | Yes         | No              | Small or single-server systems                           |
| Random String                                 | Very Rare | No          | Yes             | Medium-scale applications requiring non-predictable URLs |
| Distributed ID Generator (Snowflake + Base62) | No        | No          | No              | Large-scale distributed systems                          |

---

### Paste Creation Flow

When a user creates a new paste, the request follows these steps:

```text
Client
   │
   ▼
Load Balancer
   │
   ▼
Paste Service
   │
   ▼
Generate Short Code
   │
   ▼
Store Content in Object Storage
   │
   ▼
Store Metadata in Database
   │
   ▼
Return Short URL to Client
```


---

### Paste Read Flow

When someone accesses the short URL, the request follows these steps:

```text
Client
   │
GET /Ab3KxP
   │
   ▼
Load Balancer
   │
   ▼
Paste Service
   │
   ▼
Check Redis Cache
   │
  ┌───────────────┐
  │               │
Cache Hit     Cache Miss
  │               │
  ▼               ▼
Return        Metadata DB
Content           │
                  ▼
          Object Storage
                  │
                  ▼
         Update Redis Cache
                  │
                  ▼
          Return Content
```

---

### Expiration Handling in Pastebin

Most Pastebin services allow users to create pastes that automatically expire after a specified period. Expiration handling ensures that outdated or temporary pastes are no longer accessible and helps reclaim storage space.

Every paste has an **expiration timestamp**, stored in the Metadata Database.

Example:

| Paste ID | Short Code | Expire At           |
| -------- | ---------- | ------------------- |
| 101      | Ab3KxP     | 2026-07-10 10:00 AM |
| 102      | Xy9LmN     | Never (NULL)        |

The **expire_at** field indicates the exact date and time when the paste should become unavailable.

---

### What Happens If the Paste Has Not Expired?

Example:

```text
Current Time

2026-07-08 09:00 AM

Expire At

2026-07-10 10:00 AM
```

Since:

```text
Current Time < Expire At
```

the paste is still valid.

The Paste Service retrieves the content from Object Storage and returns it to the user.

---

### Why Not Delete the Paste Immediately?

Suppose a paste expires at exactly:

```text
10:00:00 AM
```

Deleting it immediately at that exact moment is difficult because millions of pastes may expire at different times every second.

Continuously scanning the database every second would be inefficient and would increase database load.

Instead, the system performs two separate operations:

1. **Reject access immediately** if the paste has expired.
2. **Delete expired pastes later** using a background cleanup service.

This approach is much more efficient and scalable.

---

### Background Cleanup Service

A dedicated background worker periodically scans the Metadata Database for expired pastes.

Example schedule:

```text
Every 5 minutes

or

Every 1 hour
```

The cleanup service performs the following steps:

1. Find all records where:

```text
expire_at < Current Time
```

2. Delete the metadata from the Metadata Database.

3. Delete the actual paste content from Distributed Object Storage.

4. Remove any cached entries from Redis.

This frees up storage space and prevents expired data from accumulating.

---

### Cleanup Flow

```text
Background Cleanup Service
           |
           |
Scan Metadata Database
           |
Find Expired Pastes
           |
Delete Metadata
           |
Delete Content from Object Storage
           |
Remove Cache from Redis
           |
Storage Reclaimed
```

---

### Example

Suppose the Metadata Database contains:

| Paste ID | Short Code | Expire At |
| -------- | ---------- | --------- |
| 101      | Ab3KxP     | Yesterday |
| 102      | Xy9LmN     | Tomorrow  |
| 103      | Pq8RtS     | Last Week |

When the cleanup service runs:

* Paste **101** is deleted.
* Paste **103** is deleted.
* Paste **102** remains because it has not yet expired.

---

### Why Use a Background Cleanup Service?

Using a background cleanup service provides several benefits:

* Prevents expired pastes from consuming storage.
* Keeps the Metadata Database clean and efficient.
* Frees space in Object Storage.
* Removes stale entries from Redis.
* Avoids expensive real-time deletion operations during user requests.
* Improves overall system performance and scalability.

---


### Password Protection in Pastebin

Some users may want to protect their pastes so that only people who know the password can access them. To achieve this securely, the system should **never store passwords in plain text**.

Instead, only a **hashed version** of the password is stored in the Metadata Database.

---

### Why Never Store Passwords Directly?

Suppose a user creates a password-protected paste with the password:

```text
abcd1234
```

**Incorrect Approach (Plain Text Storage):**

| Paste ID | Password |
| -------- | -------- |
| 101      | abcd1234 |

If the database is compromised, attackers can immediately see every user's password.

This is a major security risk.

---

### Correct Approach: Store Password Hash

Instead of storing the actual password, the server computes a cryptographic hash.

Example:

```text
Password

abcd1234

↓

Hash Function

↓

8d969eef6ecad3c29a3...
```

Only the hash is stored.

Database:

| Paste ID | Password Hash          |
| -------- | ---------------------- |
| 101      | 8d969eef6ecad3c29a3... |

Even if someone gains access to the database, they cannot easily determine the original password.

---

### Hashing Algorithms

Common hashing algorithms include:

* **SHA-256**
* **bcrypt**
* **Argon2**
* **PBKDF2**

### Which One Should Be Used?

For password storage, **bcrypt**, **Argon2**, or **PBKDF2** are preferred because they are intentionally slow and include features such as salting, making brute-force attacks much harder.

**SHA-256** is a cryptographic hash function but is generally **not recommended by itself for password storage**, as it is designed to be fast and is more vulnerable to password-cracking attacks unless combined with techniques such as salting and key stretching.

---

### Password Protection Flow

When the user creates a password-protected paste:

#### Step 1

User enters:

```text
Password

abcd1234
```

#### Step 2

Paste Service hashes the password.

```text
abcd1234

↓

Hash Function

↓

8d969eef6ecad3c29a3...
```

#### Step 3

Store only the hash.

```text
Metadata Database

Password Hash

8d969eef6ecad3c29a3...
```

The original password is discarded and never stored.

---

### Reading a Password-Protected Paste

When another user opens the paste:

#### Step 1

The user enters the password.

```text
abcd1234
```

#### Step 2

The Paste Service hashes the entered password.

```text
abcd1234

↓

Hash Function

↓

8d969eef6ecad3c29a3...
```

#### Step 3

Compare the generated hash with the stored hash.

```text
Generated Hash

=

Stored Hash ?
```

#### Step 4

If both hashes match:

```text
Access Granted

↓

Return Paste
```

Otherwise:

```text
401 Unauthorized

or

Incorrect Password
```

---

### Complete Authentication Flow

```text
User Opens Paste
        |
Enter Password
        |
Paste Service
        |
Hash Password
        |
Retrieve Stored Password Hash
        |
Compare Hashes
        |
 +--------------------------+
 |                          |
 | Match                    | No Match
 |                          |
Return Paste          401 Unauthorized
                      Incorrect Password
```

---

#### Example

Suppose the stored password hash is:

```text
8d969eef6ecad3c29a3...
```

User enters:

```text
abcd1234
```

After hashing:

```text
8d969eef6ecad3c29a3...
```

Since both hashes are identical:

```text
Access Granted
```

If the user enters:

```text
password123
```

Hash becomes:

```text
7c6a180b36896a0a8c02...
```

The hashes do not match.

The server returns:

```http
401 Unauthorized
```

or

```text
Incorrect Password
```

---

### Why Hashing Instead of Encryption?

Hashing is a **one-way operation**, meaning the original password cannot be recovered from the hash.

Encryption is **reversible**, which means the original password can be decrypted if the encryption key is compromised.

Since the application only needs to **verify** the password—not recover it—hashing is the correct approach.

---
### Scaling, Fault Tolerance, Bottlenecks, and Security in Pastebin

As the number of users grows from thousands to millions, a single server and a single database are no longer sufficient. The system must be designed to scale horizontally, remain highly available, tolerate failures, and protect user data. This section explains how a Pastebin system achieves scalability, fault tolerance, and security.

---

### 1. Scaling the Database

Initially, a single database may be enough to store metadata. However, as traffic increases, the database becomes a bottleneck because it must handle millions of reads and writes.

To scale the database, two common techniques are used:

* **Sharding (Horizontal Partitioning)**
* **Replication**

---

#### Sharding

Sharding means splitting one large database into multiple smaller databases called **shards**.

Instead of storing all pastes in one database:

```text
Database
|
All Pastes
```

the data is divided across multiple databases.

Example:

```text
                Database
                    |
      +-------------+-------------+
      |             |             |
   Shard 1      Shard 2      Shard 3
```

Each shard stores only a subset of the data.

---

#### Sharding by Short Code

One simple strategy is to shard based on the first character of the short code.

Example:

```text
Shard 1

A - H
```

```text
Shard 2

I - P
```

```text
Shard 3

Q - Z
```

If the generated short URL is:

```text
B7KdLp
```

it is stored in **Shard 1**.

If the short URL is:

```text
R9AxQW
```

it is stored in **Shard 3**.

This distributes the data across multiple databases.

---

#### Sharding by Paste ID

Instead of using the short code, the system can shard using the numeric Paste ID.

Example:

```text
Paste IDs

1 – 100 Million

↓

Shard 1
```

```text
100 Million – 200 Million

↓

Shard 2
```

```text
200 Million+

↓

Shard 3
```

Each shard stores a different range of IDs.

---

#### Consistent Hashing

Simple alphabetical or range-based sharding may create uneven data distribution.

Example:

Suppose most short codes begin with:

```text
A
```

Then:

```text
Shard 1
```

becomes overloaded while other shards remain underutilized.

To solve this, large-scale systems use **Consistent Hashing**.

The hash function evenly distributes pastes across all shards, resulting in:

* Better load balancing
* Easier addition or removal of database nodes
* Minimal data movement when scaling

---

#### Benefits of Sharding

* Stores billions of pastes.
* Reduces database size per server.
* Improves write throughput.
* Enables horizontal scaling.
* Distributes load across multiple databases.

---

### 2. Database Replication

While sharding increases capacity, **Replication** improves availability and read performance.

A primary database handles writes, while multiple replicas handle reads.

Architecture:

```text
             Primary Database
                    |
         +----------+----------+
         |                     |
     Replica 1            Replica 2
```

---

### Write Operations

Every write request goes to the Primary Database.

Example:

```text
Create Paste

↓

Primary Database
```

The Primary stores the data and replicates it to all replicas.

---

#### Read Operations

Read requests are distributed among the replicas.

Example:

```text
User 1

↓

Replica 1
```

```text
User 2

↓

Replica 2
```

This significantly reduces the load on the Primary Database.

---

### Benefits of Replication

#### High Availability

If one replica fails, other replicas continue serving requests.

---

#### Read Scaling

Millions of read requests can be distributed across multiple replicas.

---

#### Fault Tolerance

Copies of the data exist on multiple servers, reducing the impact of hardware failures.

---

### Fault Tolerance

A distributed system must continue operating even if individual components fail.

---

### Scenario 1: Redis Cache Failure

Suppose Redis crashes.

```text
Redis Down
```

The Paste Service simply bypasses Redis.

```text
Read Request
      |
Metadata Database
      |
Object Storage
      |
Return Paste
```

The system becomes slower because cache is unavailable, but users can still access their pastes.

Once Redis recovers, frequently accessed pastes are gradually cached again.

---

### Scenario 2: Paste Service Failure

Suppose one application server crashes.

Before:

```text
Load Balancer
      |
 +----+----+
 |         |
Server 1  Server 2
```

Server 2 fails.

After:

```text
Load Balancer
      |
Server 1
```

The Load Balancer automatically routes all new requests to healthy servers.

Users experience little or no downtime.

---

### Scenario 3: Database Replica Failure

Suppose Replica 1 becomes unavailable.

Before:

```text
Primary
   |
+--+--+
|     |
R1    R2
```

After failure:

```text
Primary
   |
Replica 2
```

Read traffic is redirected to another healthy replica.

The Primary Database continues accepting writes.

---
