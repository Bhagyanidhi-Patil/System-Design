# Design a Distributed File System (DFS)

A Distributed File System (DFS) is a file storage system where files are stored on multiple computers (machines/servers) instead of a single machine, but users see and access them as if they are in one common file system.

A DFS is primarily about storing and organizing the user's files (or an application's files) across multiple machines, while making them appear as one file system.

Examples include:

- Google File System (GFS)
- Hadoop Distributed File System
- Ceph
- Amazon S3 (object storage, but many concepts overlap)

## Example

Imagine three servers:

- Server A stores `/docs/report.pdf`
- Server B stores `/images/logo.png`
- Server C stores `/videos/demo.mp4`

Without a DFS, users would need to connect to each server separately.

With a DFS, users simply see:

```
/company
    /docs/report.pdf
    /images/logo.png
    /videos/demo.mp4
```

Even though the files are physically stored on different machines, the DFS presents them as part of one logical file system.

Google Docs and Google Sheets (Excel-like spreadsheet) are primarily cloud-based applications, not Distributed File Systems themselves.

However, they rely on distributed storage systems behind the scenes.

- Google Drive → a cloud storage service that lets users store, sync, share, and access files.
- Distributed File System → the underlying technology that stores data across multiple servers.

Suppose a user has 3 files:

- `report.pdf`
- `photo.jpg`
- `video.mp4`

The DFS might store them like this:

- Machine A → `report.pdf`
- Machine B → `photo.jpg`
- Machine C → `video.mp4`

But when the user looks at their folder, they simply see:

```
/myfiles
    report.pdf
    photo.jpg
    video.mp4
```

They don't know (and don't need to know) which machine stores which file.

However, it's not required that each file be on a different machine. The DFS decides where to place files.

For example:

- Machine A → `report.pdf`, `photo.jpg`
- Machine B → `video.mp4`
- Machine C → backup copies

Or:

- Machine A → part of `video.mp4`
- Machine B → another part of `video.mp4`
- Machine C → another part of `video.mp4`

The key idea is:

The user sees one file system, while the DFS manages where the data is actually stored across machines.

## Design

### Functional Requirements

- Upload files
- Download files
- Delete files
- Update files
- Directory structure
- Metadata management
- Replication
- Access permissions

### Non-Functional Requirements

- High availability
- Durability
- Scalability (PBs of data)
- Fault tolerance
- Low latency reads
- Strong or eventual consistency
- Cost efficiency

## Capacity Estimation

**Assume:**
- 100 million files
- Average file size = 10 MB

**Storage:**

```
100M × 10MB
= 1,000,000,000 MB
≈ 1 PB
```

**With replication factor = 3:**

```
1 PB × 3
= 3 PB
```

Need a horizontally scalable architecture.

## High-Level Architecture

```
           User / Client
                 |
                 v
          Metadata Server
      (file names, locations)
                 |
      ---------------------
      |         |         |
      v         v         v
   Storage   Storage   Storage
   Node A    Node B    Node C
```

**Two major components:**

### Metadata Layer

Stores:
- Filename
- Path
- Owner
- Permissions
- Block locations
- Replication info

### Storage Layer

Stores actual file data.

## Core Design Concept: File Chunking

Large files are usually split into fixed-size chunks (blocks) and distributed across multiple machines.

**Example:**

```
File A = 300 MB
Block Size = 128 MB

Block 1 (128 MB) -> Node 1
Block 2 (128 MB) -> Node 5
Block 3 (44 MB)  -> Node 2
```

The metadata server keeps track of:

```
File A
  Block 1 -> Node 1
  Block 2 -> Node 5
  Block 3 -> Node 2
```

When the user opens File A:

1. Client asks metadata server for block locations.
2. Client fetches blocks from Node 1, Node 5, and Node 2.
3. Client reassembles them into the original 300 MB file.

### Why chunking helps

**1. Parallel Reads**

Instead of reading from one machine:
```
Node 1 -> Entire 300 MB file
```

You can read chunks simultaneously:
```
Node 1 -> Block 1
Node 5 -> Block 2
Node 2 -> Block 3
```

This is faster.

**2. Easy Replication**

Each block can have copies:
```
Block 1 -> Node 1, Node 7, Node 9
Block 2 -> Node 5, Node 3, Node 8
Block 3 -> Node 2, Node 4, Node 6
```

If a node fails, another copy is available.

**3. Load Balancing**

Storage and network traffic are spread across many machines instead of overloading one server.

### Small files

For a tiny file:
```
notes.txt = 20 KB
```

there is only one block, so it may effectively reside on one machine (plus replicas).

## Metadata Service Design

Metadata server stores information about files, not the actual file contents.

### Example

User uploads:
```
/photos/img1.jpg
```

The file gets split:
```
B1 -> Node3
B2 -> Node7
```

The metadata service stores:
```
Path: /photos/img1.jpg
Owner: User123
Size: 200 MB
Blocks:
   B1 -> Node3
   B2 -> Node7
```

### Why separate FileMetadata and BlockMetadata?

**FileMetadata**

Contains file-level information:

```
FileMetadata
-------------
fileId = 101
path = /photos/img1.jpg
owner = User123
size = 200 MB
createdAt = ...
```

Questions it answers:
- What is the file name?
- Who owns it?
- How big is it?
- When was it created?

**BlockMetadata**

Contains storage-level information:

```
BlockMetadata
--------------
blockId = B1
fileId = 101
sequenceNumber = 1
nodeIds = [3, 5, 9]

BlockMetadata
--------------
blockId = B2
fileId = 101
sequenceNumber = 2
nodeIds = [7, 4, 8]
```

Questions it answers:
- Which blocks belong to this file?
- In what order?
- Which nodes store each block?
- Where are replicas located?

### During a Read

User requests:
```
/photos/img1.jpg
```

Metadata server looks up:
```
File 101
```

Then:
```
B1 -> Node3
B2 -> Node7
```

It returns those locations to the client.

The client then directly contacts Node3 and Node7 to fetch data.

## Upload File Flow

### Why not send data through Metadata Server?

Suppose a user uploads a 10 GB file.

**Bad design:**

```
Client
   |
10 GB
   v
Metadata Server
   |
10 GB
   v
Data Nodes
```

**Problems:**

- Metadata server handles all file traffic.
- Network bandwidth gets exhausted.
- CPU and memory become bottlenecks.
- One server limits the entire system.

### Better Design

Metadata server only coordinates.

```
Client
   |
   | "I want to upload file X"
   v
Metadata Service
```

Metadata service responds:

```
Block1 -> Node3, Node8, Node12
Block2 -> Node4, Node9, Node15
```

Then:

```
Client
  |----> Node3
  |----> Node4
```

Actual file bytes never pass through the metadata server.

### Example

```
File size = 300 MB
Block size = 128 MB

Block1 = 128 MB
Block2 = 128 MB
Block3 = 44 MB
```

Metadata allocates:

```
Block1 -> Node3
Block2 -> Node4
Block3 -> Node5
```

Client uploads:

```
128 MB -> Node3
128 MB -> Node4
 44 MB -> Node5
```

## Replication

If replication factor = 3:

```
Block1:
   Node3
   Node8
   Node12
```

There are two common approaches:

### Client writes to all replicas

```
Client --> Node3
Client --> Node8
Client --> Node12
```

### Pipeline replication (used by systems like HDFS)

In pipeline replication, the client sends the block only once, to the first data node. Then the data nodes forward it to each other.

```
Client --> Node3 --> Node8 --> Node12
```

#### How it works

Suppose Block1 = 128 MB.

1. Client starts sending data to Node3.
2. As Node3 receives chunks, it immediately forwards them to Node8.
3. As Node8 receives chunks, it immediately forwards them to Node12.
4. All three nodes write the data to disk.

This reduces network traffic from the client.

## Final Metadata Update

After successful upload:

```
FileMetadata
-------------
fileId = 101
path = /photos/img1.jpg
size = 300 MB

BlockMetadata
--------------
B1 -> [3,8,12]
B2 -> [4,9,15]
B3 -> [5,11,16]
```

Now the metadata service knows:

```
File Path
   ↓
File ID
   ↓
Blocks
   ↓
Storage Nodes
```

## Acknowledgment (ACK)

After all replicas have stored the block:

```
Node12 --> ACK --> Node8
Node8  --> ACK --> Node3
Node3  --> ACK --> Client
```

Only when the client receives the final ACK does it consider the block successfully written.

### Why is this better?

**Without pipeline:**

```
Client uploads 128 MB × 3 = 384 MB
```

**With pipeline:**

```
Client uploads only 128 MB
```

The replication traffic is handled by the storage nodes themselves.

## Download File Flow

The standard read/download flow in a DFS.

### Step 1: Request File

Client asks metadata service:

```
Get /photos/img1.jpg
```

### Step 2: Metadata Lookup

Metadata service finds:

```
File: img1.jpg

Block1 -> Node3, Node5, Node9
Block2 -> Node8, Node2, Node7
Block3 -> Node12, Node4, Node6
```

and returns the block locations.

### Step 3: Fetch Blocks

Client contacts data nodes directly:

```
Client
  |----> Node3 (Block1)
  |----> Node8 (Block2)
  |----> Node12 (Block3)
```

**Notice again:**

```
Client --> Metadata Service --> Data Nodes

❌ Not used for actual file data.
```

The metadata service only provides locations.

### Step 4: Reassemble

Client receives:

```
Block1
Block2
Block3
```

and reconstructs:

```
img1.jpg
```

using the sequenceNumber stored in metadata:

```
B1 -> sequence 1
B2 -> sequence 2
B3 -> sequence 3
```

## Optimization: Parallel Reads

Instead of:

```
Read B1
Read B2
Read B3
```

one after another,

the client can do:

```
Read B1 from Node3
Read B2 from Node8
Read B3 from Node12
```

simultaneously.

This is one of the biggest advantages of chunking.

## Handling Failures

Suppose:

```
Block2 -> Node8, Node2, Node7
```

and Node8 is down.

The client simply reads from another replica:

```
Block2 -> Node2
```

No file loss occurs because of replication.

## Replication

### Why Replication?

Suppose Block A exists only on Node1:

```
Block A
   |
   v
 Node1
```

If Node1 crashes:

```
Node1 ❌
```

Block A is unavailable (or lost).

### Replication Factor (RF)

With RF = 3:

```
Block A

Replica1 -> Node1
Replica2 -> Node5
Replica3 -> Node8
```

Now if Node1 fails:

```
Node1 ❌

Node5 ✓
Node8 ✓
```

The file is still accessible.

### Benefits

1. **Availability**

Users can still read files even when some machines are down.

```
Node1 down
Read from Node5
```

2. **Fault Tolerance**

Hardware failures are expected in large clusters.

Replication prevents data loss.

3. **Faster Reads**

If users are in different locations:

```
User A -> Node1
User B -> Node5
User C -> Node8
```

Reads can be served from the nearest or least busy replica.

## Rack Awareness

A rack is a group of servers connected to the same network switch.

**Bad placement:**

```
Rack1
 ├─ Node1 (Replica)
 ├─ Node5 (Replica)
 └─ Node8 (Replica)
```

**Problem:**

```
Rack1 Switch Fails ❌

All replicas disappear at once.
```

### Better Placement

```
Rack1 -> Replica1
Rack2 -> Replica2
Rack3 -> Replica3
```

Now:

```
Rack2 fails ❌

You still have:

Rack1 ✓
Rack3 ✓
```

So the data survives not just machine failures, but entire rack failures.

## What Metadata Stores

For each block:

```
BlockMetadata

blockId = B1

replicas:
   Node1
   Node5
   Node8
```

During a read, the client can choose any healthy replica.

# Heartbeats

During a read, the client can choose any healthy replica.

## Normal Operation

Every storage node periodically sends:

Heartbeat

to the metadata service.

For example:

Every 5 seconds

Node3 sends:

```text
{
  nodeId: 3,
  freeSpace: 2 TB,
  diskUsage: 60%,
  status: HEALTHY
}
```

Metadata service records:

```text
Node3 -> Alive
LastHeartbeat = 10:00:05
```

## Failure Detection

Suppose heartbeats arrive every 5 seconds:

```text
10:00:00
10:00:05
10:00:10
```

Then suddenly stop.

Metadata service checks:

```text
Current Time - LastHeartbeat
```

If:

```text
15 seconds
```

then:

```text
Node3 = DEAD
```

This is called a heartbeat timeout.

## What Happens Next?

Suppose:

```text
Block A:

Node3
Node5
Node8
```

Node3 dies.

Metadata updates:

```text
Block A:

Node5
Node8
```

Now replication factor has dropped:

```text
RF = 2
```

but desired RF is:

```text
RF = 3
```

## Automatic Re-Replication

Metadata service chooses a new node:

```text
Node12
```

and instructs:

```
Node5 --> Copy Block A --> Node12
```
Result:
```
Block A:
  Node5
  Node8
  Node12
```
Replication factor is restored.

---

### Why Include Disk Usage and Free Space?

Metadata service uses this information for future block placement.

Example:
```
Node1 -> 95% full
Node2 -> 30% full
```
New blocks will likely be placed on Node2.

This helps with load balancing.

---
## Data Integrity

A Distributed File System must detect data corruption.

To achieve this, each block is stored along with a checksum.

Example:

Block A
Checksum = SHA256(Block A)

### Write Flow

1. Client uploads Block A.
2. Storage node computes SHA256(Block A).
3. Checksum is stored along with block metadata.

### Read Flow

1. Read Block A from storage node.
2. Recompute SHA256(Block A).
3. Compare with stored checksum.

If checksums match:

**Data is valid.**

If checksums do not match:

**Data is corrupted.**

### Corruption Recovery

Since blocks are replicated:
```
Block A
Replica1 -> Node1
Replica2 -> Node5
Replica3 -> Node8
```
If Replica1 is corrupted:

1. Read another replica (Node5 or Node8).
2. Return valid data to the client.
3. Recreate the corrupted replica using a healthy copy.

### Benefits

* Detects disk corruption
* Detects transmission errors
* Ensures data integrity
* Works seamlessly with replication for automatic recovery

---

## Consistency Model

A Distributed File System must define how updates become visible across replicas.

This is known as the consistency model.

### Option A: Strong Consistency

```
Write completed 
    ↓ 
All future reads see the latest value
```

**Example:**

User updates File A.

Write is acknowledged only after the system guarantees consistency.

Any subsequent read returns the newest version.

**Pros**

- Simple mental model
- No stale reads
- Easier application development

**Cons**

- Higher latency
- Lower availability during failures
- Often requires consensus protocols

**Examples:**

- Leader-based replication
- Consensus systems such as Raft or Paxos

### Option B: Eventual Consistency

```
Write completed
    ↓ 
Replicas synchronize later
```

**Example:**

User updates File A.

Primary replica accepts the write.

Other replicas receive the update asynchronously.

For a short period, some reads may return older data.

**Pros**

- Fast writes
- High availability
- Better scalability

**Cons**

- Stale reads possible
- Applications must handle temporary inconsistency

---

## Metadata Scalability Problem

A single metadata server can become a bottleneck as the system grows.

```
           Metadata Server
                |
         Millions of Requests
```

**Problems:**

- High CPU usage
- High memory usage
- Limited throughput
- Single point of failure

To support large-scale systems, metadata must be distributed.

### Solution 1: Metadata Sharding

Split metadata across multiple metadata servers.

A common strategy is:

```
hash(path) % N
```

where `N` is the number of metadata servers.

For a file:

```
/photos/vacation/img1.jpg
```

the metadata router computes:

```
hash("/photos/vacation/img1.jpg") % N
```

If:

```
N = 3
hash(...) % 3 = 1
```

then the metadata for that file is stored on:

```
Metadata Server 1
```

**Example**

```
/photos/img1.jpg      -> hash = 25 -> 25 % 3 = 1
/videos/v1.mp4        -> hash = 41 -> 41 % 3 = 2
/docs/report.pdf      -> hash = 18 -> 18 % 3 = 0
```

**Result:**

- Server 0 -> `/docs/report.pdf`
- Server 1 -> `/photos/img1.jpg`
- Server 2 -> `/videos/v1.mp4`

### Why not shard only by directory?

Instead of:

```
/photos/* -> Server A
/videos/* -> Server B
```

many systems use hashing because:

- `/photos/` may contain billions of files
- `/videos/` may contain only a few thousand

Directory-based sharding can create hotspots.

Hashing distributes files more evenly.

**Benefits:**

- Horizontal scalability
- Higher throughput
- Reduced load per metadata server

**Challenges:**

- Cross-shard operations
- Rebalancing when adding servers

### Solution 2: Distributed Metadata Cluster

Instead of a single metadata server, run a metadata cluster.

```
          Metadata Cluster

             Leader
            /      \
      Follower   Follower
```

**Responsibilities:**

**Leader:**

- Handles metadata writes
- Updates cluster state
- Replicates changes

**Followers:**

- Serve metadata reads
- Maintain replicated state
- Take over if leader fails

**Benefits:**

- High availability
- Fault tolerance
- Read scalability

---

# Data Placement Strategy

The goal of data placement is to determine where blocks should be stored in the cluster.

## Objectives

* Even distribution of data
* Avoid storage hotspots
* Balance disk usage
* Maximize performance
* Simplify scaling

## Technique 1: Random Placement

Blocks are assigned to randomly selected storage nodes.

### Example

```text
Block1 -> Node3
Block2 -> Node7
Block3 -> Node1
Block4 -> Node9
```

### Advantages

* Simple implementation
* Good distribution in small systems

### Disadvantages

* Can become uneven at scale
* Some nodes may receive significantly more blocks than others
* Difficult to predict load distribution

## Technique 2: Consistent Hashing

Each node is assigned a position on a hash ring.

```text
BlockID -> Hash Function -> Position on Ring
```

The block is stored on the first node encountered while traversing the ring.

### Example

```text
Hash Ring

       Node1
      /     \
 Node4       Node2
      \     /
       Node3
```

If:

```text
hash(BlockA) = position X
```

BlockA is assigned to the next node clockwise on the ring.

### Benefits of Consistent Hashing

* Even distribution of blocks
* Minimal data movement when nodes are added or removed
* Easy horizontal scaling
* Reduced rebalancing cost
* Widely used in distributed systems

### Example: Adding a New Node

Without consistent hashing:

```text
100 TB of data
Add one node
```

Most data may need to be redistributed.

With consistent hashing:

```text
100 TB of data
Add one node
```

Only the portion of data owned by the affected section of the ring moves to the new node.

---

These are two different hashing problems.

## 1. Metadata Sharding

Question:

Which metadata server stores the metadata for this file?

Use:

```text
hash(filePath) % N
```

Example:

```text
/photos/img1.jpg

↓

hash("/photos/img1.jpg") % 3

↓

Metadata Server 1
```

This determines where metadata lives.

## 2. Data Placement

Question:

Which storage node stores Block B1?

Use something like:

```text
hash(blockId)
```

or consistent hashing:

```text
hash(B1) -> position on ring

↓

Node7
```

This determines where actual data lives.

---

# Caching

Caching is one of the most effective techniques for improving Distributed File System performance.

Instead of repeatedly fetching data from storage nodes, frequently accessed data is served from faster cache layers.

## 1. Client Cache

Clients store recently accessed blocks locally.

### Example

```text
Client
   |
Local Cache
   |
Storage Nodes
```

### Read Flow

1. Client requests Block A.
2. Check local cache.
3. If present, return immediately.
4. Otherwise fetch from storage node and cache it.

### Benefits

* Lower latency
* Reduced network traffic
* Reduced load on storage nodes

## 2. Edge Cache

Frequently accessed files are cached closer to users.

```text
User
  |
Edge Cache
  |
Storage Cluster
```

### Example

A popular video is requested by thousands of users.

Instead of:

```text
Every User
     |
Storage Cluster
```

Use:

```text
Users
   |
Edge Cache
   |
Storage Cluster
```

### Benefits

* Faster file delivery
* Lower bandwidth usage
* Reduced storage-node load

## 3. Metadata Cache

Frequently requested metadata is cached.

### Examples

```text
/photos/img1.jpg
/videos/movie.mp4
```

Instead of querying the metadata service every time:

```text
Client
   |
Metadata Cache
   |
Metadata Service
```

### Benefits

* Faster file lookups
* Reduced metadata-server load
* Improved scalability

---

# Security

Security is a critical component of a Distributed File System.

The system must ensure that only authorized users can access data and that data remains protected both during storage and transmission.

## 1. Authentication

Authentication verifies the identity of a user or service before allowing access.

### Common authentication mechanisms

* OAuth
* Kerberos
* JWT (JSON Web Tokens)

### Example

```text
User
  |
Authentication Service
  |
Identity Verified ✓
```

### Benefits

* Prevents unauthorized access
* Establishes user identity
* Enables secure access control

## 2. Authorization

After authentication, the system determines what actions the user is allowed to perform.

### Common permissions

* Read
* Write
* Execute

### Example

```text
User123

/photos/img1.jpg

Permissions:
Read    ✓
Write   ✓
Execute ✗
```

Authorization can be implemented using:

* Access Control Lists (ACLs)
* Role-Based Access Control (RBAC)
* File ownership and permission models

### Benefits

* Fine-grained access control
* Protection against unauthorized operations
* Multi-user security

## 3. Encryption

Encryption protects data from unauthorized access.

### Encryption at Rest

Data stored on disks is encrypted.

### Write Flow

User uploads a file:

```text
File
  ↓
Split into Blocks
  ↓
Encrypt Blocks
  ↓
Store on Disk
```

Example:

```text
Block1 -> Encrypt -> EncryptedBlock1
Block2 -> Encrypt -> EncryptedBlock2
```

Storage node stores:

```text
EncryptedBlock1
EncryptedBlock2
```

not the plaintext blocks.

### Read Flow

When the user requests the file:

```text
Disk
  ↓
Read Encrypted Block
  ↓
Decrypt
  ↓
Return Original Block
```

Then:

```text
Block1 + Block2 + Block3
  ↓
Assemble File
  ↓
Send to Client
```

### Benefits

* Protects data if disks are stolen
* Prevents unauthorized access to stored data

### Encryption in Transit

Data transferred across the network is encrypted.

### Example

```text
Client
   |
TLS
   |
Metadata Service

Client
   |
TLS
   |
Storage Node
```

### Common protocol

* TLS (Transport Layer Security)

### Benefits

* Prevents eavesdropping
* Prevents man-in-the-middle attacks
* Protects sensitive data during transmission

---

# Multi Data Center Design

Large-scale Distributed File Systems often replicate data across multiple data centers.

### Example

```text
DC1
DC2
DC3
```

Each data center stores copies of critical data.

## Why Use Multiple Data Centers?

A single data center can fail due to:

* Power outages
* Network failures
* Natural disasters
* Hardware failures
* Human errors

If all data exists only in one location:

```text
DC1 ❌
```

The service becomes unavailable.

Multi-region replication prevents this problem.

## Replication Across Regions

### Example

```text
Block A

Replica1 -> DC1
Replica2 -> DC2
Replica3 -> DC3
```

If DC1 becomes unavailable:

```text
DC1 ❌

DC2 ✓
DC3 ✓
```

The file remains accessible.

## Read Flow

Users are typically served from the nearest data center.

### Example

```text
User (India)
      |
     DC1

User (Europe)
      |
     DC2

User (US)
      |
     DC3
```

### Benefits

* Lower latency
* Reduced network costs
* Better user experience

## Benefits

### Disaster Recovery

Entire data center failures can be tolerated.

```text
DC1 ❌
```

Service continues from:

```text
DC2
DC3
```

### Geographic Redundancy

Copies exist in multiple geographic regions.

This protects against:

* Regional outages
* Earthquakes
* Floods
* Large-scale infrastructure failures

### Higher Availability

Users can continue accessing files even if one region is unavailable.

---

```text
                         ┌─────────────┐
                         │   Client    │
                         └──────┬──────┘
                                │
                                │ Auth (OAuth/JWT/Kerberos)
                                ▼
                    ┌───────────────────────┐
                    │ Metadata Service      │
                    │ / Metadata Cluster    │
                    └───────┬───────────────┘
                            │
                            │ Metadata:
                            │ File -> Blocks
                            │ Block -> Nodes
                            ▼
        ┌─────────────────────────────────────────────┐
        │               Storage Nodes                 │
        │                                             │
        │  Node1   Node2   Node3   Node4   Node5      │
        │                                             │
        │  Blocks + Replicas + Checksums + AES        │
        └───────────────┬─────────────────────────────┘
                        │
                        │ Heartbeats
                        ▼
                 ┌───────────────┐
                 │ Metadata      │
                 │ Monitoring    │
                 └───────────────┘
```
