# Design Dropbox

## Problem Statement

Design a cloud file storage system similar to Google Drive or Dropbox where users can:

- Upload files
- Download files
- Sync files across multiple devices
- Create folders
- Share files
- Maintain file versions
- Delete and restore files
- Handle millions of users and petabytes of data

## Functional Requirements

### Must Have
- User authentication
- Upload files
- Download files
- Delete files
- Create folders
- Rename files
- Share files
- File synchronization
- File versioning

## Non-Functional Requirements
- Highly Available
- Highly Scalable
- Durable Storage
- Low upload/download latency
- Strong metadata consistency
- Eventual consistency for file sync
- Fault tolerant
- Secure

## Capacity Estimation

### Assumptions
- 100 Million Users
- 20 Million Daily Active Users
- Average storage per user: 50 GB

### Total Storage
```
100M × 50 GB = 5000 PB = 5 EB
```

### Daily Uploads
```
20M users × 100 MB/day = 2 PB/day
```

**Note:** Clearly we cannot store files in SQL. We need distributed object storage.

---
### Why Not SQL Databases?

#### What SQL is Good For

A SQL database (like MySQL or PostgreSQL) is designed to store **structured data**, such as:

- User information
- File names
- Folder hierarchy
- Permissions
- Upload timestamps

#### Example: Metadata Table

| User ID | File Name | Size | Created At |
|---------|-----------|------|-----------|
| 101 | photo.jpg | 5 MB | 10:30 AM |
| 102 | report.pdf | 20 MB | 11:15 AM |

This kind of information is **small and easy** for SQL databases to manage.

---
### The Problem: Storing Actual Files in SQL

**What happens if we store actual files in the database?**

**Scenario:** A user uploads a 2 GB video.

**If we store it in SQL database:**
- ❌ Database becomes extremely large
- ❌ Reading and writing large files becomes much slower
- ❌ Backups take a very long time
- ❌ Replication between servers becomes expensive
- ❌ Database performance degrades for normal queries (login, listing files)

**At scale (100M users × 50 GB each):**
```
100M × 50 GB = 5 EB (Exabytes)
```

That's an **enormous amount of binary data**. SQL databases are simply **not optimized** to store petabytes or exabytes of large files.

---
### The Better Approach: Separation of Concerns

**Store metadata in SQL, files in object storage.**

#### How It Works

Instead of storing the file itself, **SQL stores only metadata**:

| File ID | User ID | File Name | Storage URL |
|---------|---------|-----------|-------------|
| 501 | 101 | photo.jpg | s3://bucket/abc123 |

The **actual file** is stored in a **distributed object storage system** such as:
- Amazon S3
- Google Cloud Storage
- Azure Blob Storage

#### Why Object Storage is Better

Object storage is **built specifically for storing huge files** and provides:

✅ **Virtually unlimited storage** (petabytes/exabytes)  
✅ **High durability** (multiple copies of each file)  
✅ **Easy horizontal scaling** (add more storage servers)  
✅ **Fast retrieval** of large files  
✅ **Lower cost** compared to databases  

---
**NOTE** = In systems like Google Drive or Dropbox, the actual file is typically stored in **distributed object storage**, not in a traditional **NoSQL database**.
```
          Upload File
               │
               ▼
         API Server
          /       \
         /         \
        ▼           ▼
 SQL Database   Object Storage
 (Metadata)     (Actual File)
```

### Where does NoSQL fit?
- NoSQL databases are not usually used to store the file itself. Instead, they may store metadata that needs to scale horizontally.

#### When should we use SQL?
Use SQL if:
- Relationships are important (users, folders, sharing, permissions)
- ACID transactions are required
- You need joins and complex queries

**Example:**
- Banking system
- E-commerce orders
- User authentication

#### When should we use NoSQL?

Use NoSQL if:

- You need to scale horizontally to many servers
- You have a massive number of reads/writes
- Your schema changes frequently
- You don't need complex joins

**Examples:**

- Social media posts
- User activity logs
- Session storage
- File metadata at very large scale

---
## High-Level Architecture

```mermaid
                    Client
          (Web/Mobile/Desktop)

                     |
                 Load Balancer
                     |
          -------------------------
          |                       |
     API Gateway             Auth Service
          |
    ---------------------
    |        |          |
Metadata  Upload     Download
Service   Service     Service
    |         |          |
    |         |          |
 Metadata DB  |      Object Storage
              |
         Chunk Service
              |
       Object Storage
              |
      CDN (Downloads)

Sync Service
Notification Service
Search Service
Version Service

```

---
## High level components
| **Component**            | **Responsibilities**                                                                                                                                                                                                                                     |
| ------------------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **API Gateway**          | • Authentication<br>• Rate limiting<br>• Request routing<br>• Logging & monitoring                                                                                                                                                                       |
| **Metadata Service**     | Manages **file metadata** only (not the actual file). Stores:<br>• File name<br>• Owner<br>• Parent folder<br>• Permissions<br>• Version<br>• Size<br>• Hash<br>• Created/Updated time<br>• Object storage location                                      |
| **Metadata Database**    | Stores metadata with **ACID consistency**.<br><br>**Possible databases:**<br>• MySQL<br>• PostgreSQL<br>• CockroachDB<br>• Spanner<br><br>**Example Schema:**<br>`FileID, OwnerID, FolderID, FileName, Size, Version, StoragePath, CreatedAt, UpdatedAt` |
| **Object Storage**       | Stores the **actual file data** (not metadata). Supports massive scalability.<br><br>**Examples:** S3, Google Cloud Storage, HDFS, Distributed File System.<br><br>Large files are stored as **chunks** (Chunk1, Chunk2, Chunk3, ...).                   |
| **Upload Service**       | • Validate upload request<br>• Split file into chunks<br>• Upload chunks to object storage<br>• Update metadata in the database                                                                                                                          |
| **Download Service**     | • Read metadata<br>• Locate file chunks<br>• Download chunks<br>• Merge chunks<br>• Return the complete file to the user                                                                                                                                 |
| **Sync Service**         | Synchronizes files across all user devices.<br><br>**Example:** Upload from laptop → Phone and tablet automatically receive the latest version.                                                                                                          |
| **Notification Service** | Sends real-time updates using **WebSockets** or **Push Notifications**.<br><br>Examples:<br>• File updated<br>• New version available<br>• File shared with you                                                                                          |
| **Version Service**      | Maintains file version history.<br><br>Example:<br>`Resume.pdf → v1 → v2 → v3`<br>Users can restore any previous version.                                                                                                                                |

---
## Upload Workflow
- When a user uploads a file (e.g., Vacation.mp4), the request passes through multiple services before the upload is completed.
```
Client
   │
   ▼
API Gateway
   │
   ▼
Upload Service
   │
   ▼
Split File into Chunks
   │
   ▼
Store Chunks in Object Storage
   │
   ▼
Metadata Service
   │
   ▼
Metadata Database
   │
   ▼
Upload Successful
```

**Sequence of Operations**

- The client sends a `POST /upload` request.
- The API Gateway `authenticates` the `user` and forwards the request.
- The Upload Service generates a `unique FileID`.
- The file is divided into smaller chunks (e.g., 64 MB each).
- Each chunk is uploaded to Object Storage.
- Once all chunks are uploaded successfully, the `Metadata Service stores the file metadata` (name, size, owner, storage path, version, etc.) in the Metadata Database.
- The server returns a success response to the client.


### Why Do We Use Chunking?
Instead of uploading a large file (e.g., 5 GB) as a single unit, it is divided into smaller chunks (e.g., 64 MB each).
```
5 GB File
      │
      ▼
64 MB | 64 MB | 64 MB | ... | 64 MB
```
**Benefits of Chunking:**
- **Resume uploads**: If the upload is interrupted, only the remaining chunks need to be uploaded instead of restarting the entire file.
- **Parallel uploads**: Multiple chunks can be uploaded simultaneously, reducing the total upload time.
- **Efficient retries**: If a chunk fails, only that specific chunk is retried.
- **Faster downloads**: Different chunks can be downloaded in parallel and merged on the client side.
- **Improved reliability**: Large uploads become more resilient to network failures.

### Chunk upload
Consider a 1 GB file.

- If each chunk is 64 MB, the file is split into approximately 16 chunks.
```
1 GB File
      │
      ▼
Chunk 1
Chunk 2
Chunk 3
...
Chunk 16
```
- Each chunk is uploaded and stored independently in the object storage system. Once all chunks are successfully uploaded, the metadata database records information about the file and the location of its chunks.

### Parallel upload
Uploading chunks one after another is slower.

**Sequential Upload**
```
Chunk 1
   │
   ▼
Chunk 2
   │
   ▼
Chunk 3
   │
   ▼
Chunk 4
```
Only one chunk is uploaded at a time, so the total upload time is the sum of the upload time for all chunks.

**Parallel Upload**
```
Chunk 1    Chunk 2    Chunk 3    Chunk 4    Chunk 5
     \         |          |          |         /
      \________Uploaded Simultaneously________/
```
Multiple chunks are uploaded concurrently, making full use of the available network bandwidth.

**Advantages of Parallel Upload**
- Reduces overall upload time.
- Utilizes network bandwidth more efficiently.
- Improves user experience, especially for large files.
- If one chunk fails, only that chunk needs to be retried while the others continue uploading.

---
## Download Workflow
When a user downloads a file, the system first retrieves the file's metadata to locate its chunks, then downloads and combines them before returning the complete file.
```
Client
   │
   ▼
API Gateway
   │
   ▼
Download Service
   │
   ▼
Metadata Service
   │
   ▼
Metadata Database
   │
   ▼
Get Chunk Locations
   │
   ▼
Object Storage
   │
   ▼
Download Chunks
   │
   ▼
Merge Chunks
   │
   ▼
Return Complete File
```
### Sequence of Operations
- The client sends a `GET /download/{fileId}` request.
- The `API Gateway authenticates the user` and forwards the request to the Download Service.
- The Download Service requests the `file metadata from the Metadata Service`.
- The Metadata Service retrieves the metadata from the Metadata Database, including the locations of all file chunks.
- The Download Service `fetches the required chunks from Object Storage` (often in parallel).
- The downloaded chunks are merged in the correct order to reconstruct the original file.
- The complete file is returned to the client.

### Why download in chunks?
- `Faster downloads`: Multiple chunks can be downloaded simultaneously.
- `Efficient retries`: If a chunk download fails, only that chunk needs to be downloaded again.
- `Supports large files`: Even very large files can be downloaded efficiently without loading the entire file at once.
- `Better reliability`: The download can often resume from the last successfully downloaded chunk if the connection is interrupted.

---
## Synchronization

- The Sync Service ensures that all devices logged into the same account stay up to date with the latest file changes.
**Example**

Suppose a user is logged in on:

- 💻 Laptop
- 📱 Phone
- 📲 Tablet
The user uploads Photo.jpg from the laptop.
```
Laptop uploads Photo.jpg
          │
          ▼
     Sync Service
          │
          ▼
 Publishes "File Updated" event
      │                │
      ▼                ▼
   Phone            Tablet
      │                │
      ▼                ▼
Download latest version automatically
```
- As soon as the upload is completed, the Sync Service notifies the other devices that the file has changed. The phone and tablet then download the latest version, keeping all devices synchronized.

### Real-Time Synchronization

To notify devices immediately when a file changes, the system can use:

- WebSockets – Maintains a persistent connection between the client and server for instant updates.
- Long Polling – The client repeatedly checks the server for updates.
- Push Notifications – The server pushes notifications to devices when a change occurs.

**Note**: Services like `Google Drive` primarily use `Push Notifications` for real-time synchronization across devices.

### Conflict Resolution

- A conflict occurs when the same file is modified simultaneously on multiple devices.

**Example**
- Laptop edits Report.pdf
- Phone also edits Report.pdf
- Both users save their changes at nearly the same time.

The system must decide how to handle these conflicting updates.

#### Option 1: Last Write Wins (LWW)

The most recent update overwrites the previous one.
```
Laptop saves
      │
      ▼
Phone saves later
      │
      ▼
Phone version replaces Laptop version
```
**Advantages**

- Simple to implement.
- Minimal storage overhead.

**Disadvantage**

- Changes from the earlier update may be permanently lost.

#### Option 2: Version Numbers

Each file has a version number.

**Example:**

Current Version = 5
```
Laptop downloads Version 5
Phone downloads Version 5

Laptop uploads → Version 6 ✔

Phone uploads → Based on Version 5 ❌
               Conflict Detected
```
Since the phone is uploading an outdated version, the system detects a conflict instead of silently overwriting the latest file.

**Advantages**

- Prevents accidental overwrites.
- Makes conflict detection straightforward.

#### Option 3: Keep Both Copies (Recommended)

Instead of overwriting either version, the system saves both copies.

**Example:**
```
Report.pdf
Report (Laptop Copy).pdf
Report (Phone Copy).pdf
```
The user can later compare the two versions and decide which one to keep.

**Advantages**

- No data is lost.
- Safest approach for collaborative editing.

**Real-world example**: `Dropbox` uses this approach by creating a `conflicted copy` when simultaneous edits cannot be merged automatically.

---
## Versioning
Instead of overwriting a file every time it is updated, the system creates a new version. This allows users to restore previous versions if needed.

**Example**
```
Resume.pdf

Version 1
      │
      ▼
Version 2
      │
      ▼
Version 3
```
Every upload increments the version number (Version++), while older versions are retained.

**Benefits**

- Restore previous versions.
- Recover accidentally deleted or modified content.
- Maintain an audit/history of changes.

---
## File Deduplication
Deduplication avoids storing multiple copies of the same file.

**Example**

Suppose 100 users upload the same file (Ubuntu.iso).

`Without deduplication:`
```
100 Users
    │
    ▼
100 Copies Stored
```
`With deduplication:`
```
100 Users
    │
    ▼
1 File Stored
    │
    ▼
100 Metadata Entries → Same Object
```
The system computes a SHA-256 hash for every uploaded file.

- If the hash already exists, the file is not uploaded again.
- Only a new metadata entry is created for that user.

**Benefit**: Saves a significant amount of storage space.

---
## Upload Workflow with Deduplication

- Before uploading a file, the system checks whether an identical file already exists.
- If the file already exists, only the metadata is stored, avoiding duplicate uploads.
Suppose User A uploads Ubuntu.iso.

`First upload`
```
User A uploads Ubuntu.iso
        │
        ▼
Calculate SHA-256 Hash
        │
        ▼
Hash not found
        │
        ▼
Store file in Object Storage
        │
        ▼
Store metadata
```
***Metadata stored:***

| FileID | UserID | FileName   | StoragePath | Hash   |
| ------ | ------ | ---------- | ----------- | ------ |
| F101   | User A | Ubuntu.iso | object123   | abc123 |


The actual file is stored once in object storage as `object123`.

Now User B uploads the same file.

`Second upload`
```
User B uploads Ubuntu.iso
        │
        ▼
Calculate SHA-256 Hash
        │
        ▼
Hash already exists ✔
        │
        ▼
Do NOT upload file again
        │
        ▼
Create new metadata entry
```
New metadata stored:

| FileID | UserID | FileName   | StoragePath | Hash   |
| ------ | ------ | ---------- | ----------- | ------ |
| F205   | User B | Ubuntu.iso | object123   | abc123 |

**Notice:**
- A `new FileID` is created.
- The `Owner/UserID` is different.
- The `StoragePath points to the same object` (object123).
- `No new copy` of the file is stored.

#### Object Storage
Only one copy exists.
```
Object Storage

object123  ---> Ubuntu.iso
```

#### Metadata Database
```
FileID   User      StoragePath
--------------------------------
F101     User A    object123
F205     User B    object123
F350     User C    object123
```
Three users have uploaded the same file, but all metadata entries point to the same object in storage.

#### What if User A deletes the file?

We don't delete object123 immediately, because User B and User C are still using it.

The system usually maintains a reference count.
```
object123
Reference Count = 3
User A deletes → count becomes 2
User B deletes → count becomes 1
User C deletes → count becomes 0
```
Only when the reference count reaches 0 is the actual file removed from object storage.

---
## Caching
Frequently accessed metadata is stored in a cache such as Redis to reduce database load.

**Example**
```
Client
   │
   ▼
Redis Cache
   │
(Cache Hit)
   │
   ▼
Return Metadata
```
If the metadata is not in the cache (cache miss), it is fetched from the database and stored in the cache for future requests.

---
## Content Delivery Network (CDN)

- A CDN stores copies of frequently downloaded files on servers close to users.

**Workflow**
```
Client
   │
   ▼
CDN
   │
(Cache Hit)
   │
   ▼
Return File

(Cache Miss)
   │
   ▼
Object Storage
```
**Benefits**

- Lower latency.
- Faster downloads.
- Reduced load on object storage.

---
## Folder structure
Files are organized using a hierarchical folder structure.
```
Root
├── Photos
├── Movies
└── Docs
     ├── Resume.pdf
     └── Notes.docx
```
Instead of storing the full path, each file stores its `ParentFolderID`.
**Example**
| FileID | ParentFolderID | FileName   |
| ------ | -------------- | ---------- |
| 201    | 15             | Resume.pdf |

This makes moving and renaming folders efficient.

---
## Consistency

Different parts of the system require different consistency models.

`1. Metadata → Strong Consistency`

- Metadata must always be accurate.
- This information must always be correct because the application depends on it to locate the file.

**Example:**

- User uploads a file.
- Now suppose the metadata database is not updated due to a server failure.
- **Now what happens?** 
The user opens `Google Drive.`

The application asks:

`"Where is Resume.pdf?"`

- It searches the metadata database.
- The file exists in storage but cannot be found.
```
SELECT * FROM Files
WHERE FileName='Resume.pdf';
```
- Result - `No rows found`
- Therefore, metadata requires strong consistency.
#### Why Strong Consistency?

- When an upload succeeds, every client should immediately see the correct metadata.

- There should never be a situation where:
```
Upload Successful

↓

Metadata missing

↓

File cannot be found
```
- This is why metadata databases usually use ACID transactions and strong consistency.


`2. File Replication → Eventual Consistency`

Now consider the actual file stored in object storage.

Suppose each chunk is replicated to three storage servers.
```
Chunk A

│
├── Node 1
├── Node 2
└── Node 3
```
`Suppose the upload reaches Node 1 first.`
```
Time = 10:00

Node1 ✔
Node2 ✘
Node3 ✘
```
`A few milliseconds later:`
```
Time = 10:01

Node1 ✔
Node2 ✔
Node3 ✘
```
`Finally:`
```
Time = 10:02

Node1 ✔
Node2 ✔
Node3 ✔
```
- The replicas were not updated at exactly the same instant.

- Instead, they synchronized asynchronously.

- Eventually, all three replicas contain the same data.

`This is called eventual consistency.`

#### Why is Eventual Consistency acceptable?

Because the file is already safely stored.

Even if one replica is slightly behind for a few milliseconds:

- another replica has the latest copy,
- the user can still download the file,
- the remaining replicas will catch up shortly.
- There is no permanent data loss.

---
## Scaling

A cloud storage system should scale independently for different components.

Metadata Database
```
Shard data using UserID.
Add Read Replicas for read-heavy workloads.
```
Object Storage
```
Add more storage servers as capacity grows.
Stateless Services
```
Services such as:
```
Upload Service
Download Service
Sync Service
```
do not maintain session state, so additional instances can simply be added behind a Load Balancer to handle increased traffic.

---