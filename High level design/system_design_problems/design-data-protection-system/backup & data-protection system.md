
---
**Design a distributed backup service that can back up data from millions of machines and restore that data whenever a machine experiences failure, accidental deletion, or data corruption.**

Imagine an organization has:
- 1 million servers.
- Each server contains 500 GB of data.
- The system must take regular backups.
- Users should be able to restore either an entire machine or individual files.
- Backups must remain safe even if storage servers fail.
Think of a service similar to **AWS Backup, Google Cloud Backup, or enterprise backup systems.**

#### Functional requirements

1. Register millions of machines for backup.
2. Schedule automatic backups.
3. Support full and incremental backups.
4. Deduplicate identical data.
5. Compress data to reduce storage costs.
6. Verify backup integrity.
7. Restore data to the original or a different machine.
8. Maintain backup history and retention policies.

#### Non-functional requirements

|Requirement|Description|
|---|---|
|Scalability|Support millions of machines|
|Durability|Never lose successfully committed backups|
|Availability|Backup and restore services should remain available|
|Reliability|Detect corrupted or incomplete backups|
|Performance|Restore data quickly|
|Security|Encrypt data at rest and in transit|
|Consistency|Restore a valid snapshot of the original data|

#### High-level architecture

```

                    BACKUP / DATA PROTECTION SYSTEM
                    =================================

                         1. CLIENT MACHINES
                    +----------------------------+
                    |      Backup Agent          |
                    |                            |
                    |  +--------+  +--------+    |
                    |  |Server 1|  |Server 2|    |
                    |  +--------+  +--------+    |
                    |  |Server 3|  |  ...   |    |
                    |  +--------+  +--------+    |
                    |       Millions             |
                    +-------------+--------------+
                                  |
                                  v
                    2. API GATEWAY / LOAD BALANCER
                    +----------------------------+
                    | Authentication             |
                    | Authorization              |
                    | Request Routing            |
                    +-------------+--------------+
                                  |
                   +--------------+--------------+
                   |                             |
                   v                             v
          +-------------------+        +-------------------+
          | BACKUP SCHEDULER  |        |  RESTORE SERVICE  |
          |                   |        |                   |
          | Creates backup    |        | Handles restore   |
          | jobs              |        | requests          |
          +---------+---------+        +---------+---------+
                    |                            |
                    v                            |
                    3. MESSAGE QUEUE             |
          +--------------------------------+     |
          | Kafka / RabbitMQ / Cloud Queue |     |
          |                                |     |
          | Decouples job creation from    |     |
          | job execution                  |     |
          +---------------+----------------+     |
                          |                      |
                          v                      |
                    4. BACKUP WORKER CLUSTER     |
          +--------------------------------+     |
          |                                |     |
          |  +----------+  +----------+    |     |
          |  | Worker 1 |  | Worker 2 |    |     |
          |  +----------+  +----------+    |     |
          |       |           |            |     |
          |       +-----+-----+            |     |
          |             |                  |     |
          |        +----v----+             |     |
          |        | Worker N|             |     |
          |        +---------+             |     |
          |                                |     |
          |  Processing Pipeline:          |     |
          |                                |     |
          |  1. Chunking                   |     |
          |  2. Hashing                    |     |
          |  3. Deduplication              |     |
          |  4. Compression                |     |
          |  5. Encryption                 |     |
          +---------------+----------------+     |
                          |                      |
                 +--------+---------+            |
                 |                  |            |
                 v                  v            |
          +----------------+  +----------------+ |
          | OBJECT STORAGE |  | METADATA DB    | |
          |                |  |                | |
          |S3 / Distributed|  | PostgreSQL /   | |
          | Storage        |  | Distributed DB | |
          |                |  |                | |
          | Actual Backup  |  | Backup         | |
          | Chunks         |  | Manifests      | |
          |                |  | Chunk Mappings | |
          |                |  | Backup Status  | |
          +-------+--------+  +-------+--------+ |
                  |                   |          |
                  +---------+---------+          |
                            |                    |
                            v                    v
                    5. RESTORE ENGINE <----------+
          +--------------------------------+
          |                                |
          |  1. Read Backup Manifest       |
          |  2. Fetch Required Chunks      |
          |  3. Verify Checksums           |
          |  4. Decrypt Chunks             |
          |  5. Decompress Chunks          |
          |  6. Reconstruct Original Data  |
          |                                |
          +---------------+----------------+
                          |
                          v
                  +------------------+
                  | DESTINATION      |
                  | MACHINE          |
                  |                  |
                  | Restored Data    |
                  +------------------+

```

#### Each component details

In this Backup / Data Protection System, Client Machines primarily means the machines whose data needs to be backed up.

It can include both end-user devices and enterprise machines.

|Type|Examples|
|---|---|
|End-user devices|Laptops, desktops, personal computers|
|Enterprise servers|Application servers, database servers, file servers|
|Virtual machines|AWS EC2 instances, VMware VMs|
|Infrastructure|On-premises servers, cloud workloads|
##### A. Backup Agent

A lightweight software agent runs on every client machine.

Responsibilities:
- Collect files and directories to back up.
- Identify modified files.
- Divide data into chunks.
- Communicate with the backup server.
- Upload data securely.
- Report backup progress.

*Why do we need an agent?*
Because the central backup server cannot efficiently inspect the filesystem of millions of remote machines. The agent collects the data locally and communicates with the backup service.

##### B. API Gateway

All client requests enter through the API Gateway.

Example APIs:

| API                       | Purpose            |
| ------------------------- | ------------------ |
| `POST /machines/register` | Register machine   |
| `POST /backups`           | Trigger backup     |
| `GET /backups/{id}`       | Get backup status  |
| `POST /restores`          | Start restoration  |
| `GET /restores/{id}`      | Get restore status |

It also handles authentication, rate limiting and request validation.

##### C. Backup Scheduler

This component decides when a machine should be backed up.

For example:
- Machine A → Every 6 hours.
- Machine B → Every 24 hours.
- Machine C → Every Sunday.
It reads backup policies and generates jobs when they become due.


**Important: The scheduler should not perform the actual backup. It should only create and dispatch jobs.**

*Why?*
Suppose 100,000 machines need backups at 2 AM. If the scheduler performs all the work itself, it becomes a bottleneck.
Instead, it places jobs into a message queue, and multiple workers process them concurrently.

##### D. Message Queue

We can use Kafka, RabbitMQ or a cloud-managed queue.

Example message:

```
{
  "job_id": "job_123",
  "machine_id": "machine_456",
  "backup_type": "INCREMENTAL",
  "snapshot_id": "snapshot_789"
}
```

Why do we need a queue?
1. Decouples the scheduler from workers.
2. Buffers jobs during traffic spikes.
3. Supports parallel processing.
4. Helps recover jobs after worker failures.
5. Provides backpressure when workers cannot process jobs fast enough.

*One important interview point: A queue generally provides at-least-once delivery, so workers must be idempotent. Receiving the same job twice must not create an incorrect or duplicate committed backup.*

##### E. Backup Workers

These are the actual processing machines.

They:
1. Receive backup jobs.
2. Fetch data from the backup agent.
3. Divide data into chunks.
4. Calculate hashes.
5. Perform deduplication.
6. Compress data.
7. Encrypt data.
8. Upload chunks to object storage.
9. Update metadata.
We can horizontally scale workers by adding more instances when the queue backlog increases.

---
### Full backup vs Incremental backup

Suppose a machine contains 100 GB of data.

**Full backup:** Copies all 100 GB every time.
**Incremental backup:** Copies only data changed since the previous successful backup.

**But how do we know which data has changed?**
There are multiple approaches:
- Filesystem change journal (e.g. USN Journal on Windows).
- Filesystem snapshot comparison.
- File modification metadata.
- Block-level change tracking.
    
For a production backup system, relying only on modification timestamps is unsafe because timestamps can be changed or preserved. Change journals and snapshot-based tracking are more reliable.

**Important**: Incremental backups are dependent on previous backup data. To restore Day 3, we need Day 1's full backup and the subsequent incremental changes.

### Chunking and Deduplication

These two concepts are closely related, but they are not the same.
##### A. Chunking
Instead of storing a 100 GB file as one huge object, we divide it into smaller chunks.

Chunking provides:
- Parallel uploads.
- Easier retries.
- Better deduplication.
- Easier recovery of individual pieces.

In practice, we can use fixed-size or content-defined chunking.

Content-defined chunking is particularly useful because it identifies boundaries based on data content rather than fixed offsets. This helps when data is inserted into the middle of a file.

##### B. Deduplication

Suppose two machines contain the same file.
Machine A → `report.pdf`
Machine B → `report.pdf`

Without deduplication, we store the same file twice.
With deduplication, we store the chunk only once and maintain references to it.

**How?**
We calculate a cryptographic hash such as SHA-256 for every chunk.

Example:

|Chunk|SHA-256 hash|Action|
|---|---|---|
|A1|`abc123`|Store|
|B1|`abc123`|Reuse existing chunk|
|C1|`xyz456`|Store|
**Interview question: What happens if two chunks have the same hash?**

We should not blindly assume that a matching hash proves identical data. We can verify chunk length and, where required, compare actual content before reusing the existing object. SHA-256 collisions are extremely unlikely, but correctness matters.

### Compression

After deduplication, we compress chunks to reduce storage usage.

For example:
- Original chunk: 100 MB.
- After compression: 65 MB.
    
Possible compression algorithms:
- LZ4 – very fast compression and decompression.
- Zstandard (Zstd) – good balance between speed and compression ratio.
- Gzip – widely supported.

**Why compress after deduplication?**

Because deduplication first identifies duplicate chunks. We only need to compress and store new unique chunks.
This avoids wasting CPU resources compressing data that already exists.

### Encryption

Backup data may contain sensitive company information.

***We need encryption at two levels:***
**Encryption in transit**
- Use TLS when transferring data between agents, workers and storage.

**Encryption at rest**
- Encrypt backup chunks before storing them.
- Use AES-256-GCM or another authenticated encryption scheme.
- Manage encryption keys using KMS (Key Management Service).
    
An important detail: If we encrypt identical chunks with different random encryption keys, their encrypted representations will differ. Therefore, deduplication should happen before encryption, or use a carefully designed tenant-scoped deduplication scheme.

For security, avoid global cross-tenant deduplication where it can expose whether another customer has stored particular data.

---
### How do we store Full and Incremental backups?

This is where we introduce an important concept called a Backup Manifest.
##### First, understand the problem

Imagine an enterprise server contains three files:
- `employee.txt`
- `salary.xlsx`
- `attendance.pdf`
We want to back up these files.

Instead of storing the entire backup as one huge file, our system divides the data into smaller chunks and stores them in Object Storage.

```

                  ORIGINAL SERVER DATA
              +----------------------------+
              |                            |
              |  employee.txt              |
              |  salary.xlsx               |
              |  attendance.pdf            |
              |                            |
              +-------------+--------------+
                            |
                            v
                     CHUNKING PROCESS
                            |
             +--------------+--------------+
             |              |              |
             v              v              v
       +-----------+  +-----------+  +-----------+
       |  File 1   |  |  File 2   |  |  File 3   |
       |           |  |           |  |           |
       |employee.txt| |salary.xlsx| |attendance |
       |           |  |           |  |   .pdf    |
       +-----+-----+  +-----+-----+  +-----+-----+
             |              |              |
             v              v              v
       +-----------+  +-----------+  +-----------+
       | Chunk C1  |  | Chunk C3  |  | Chunk C5  |
       | Chunk C2  |  | Chunk C4  |  | Chunk C6  |
       +-----+-----+  +-----+-----+  +-----+-----+
             |              |              |
             +--------------+--------------+
                            |
                            v
                       OBJECT STORAGE
              +----------------------------+
              |                            |
              |  +----------------------+  |
              |  | Chunk C1             |  |
              |  +----------------------+  |
              |  | Chunk C2             |  |
              |  +----------------------+  |
              |  | Chunk C3             |  |
              |  +----------------------+  |
              |  | Chunk C4             |  |
              |  +----------------------+  |
              |  | Chunk C5             |  |
              |  +----------------------+  |
              |  | Chunk C6             |  |
              |  +----------------------+  |
              |                            |
              | Stores actual chunk data   |
              +----------------------------+

```

How will the system know:
- Which chunks belong to which file?
- Which chunks should be combined to reconstruct a file?
- Where are those chunks stored?
- Which chunks belong to a particular backup?
    
This is exactly why we need a Backup Manifest!

#### What is a Backup Manifest?

**A Backup Manifest is a metadata file that contains the information required to reconstruct a particular backup.**

Think of it as an instruction book or index.
It doesn't generally contain the actual file contents. Instead, it tells the restore service where the data is and how to put it back together.

So, the manifest tells us :
- `employee.txt` → Retrieve C1 and C2 and combine them.
- `salary.xlsx` → Retrieve C3 and C4 and combine them.
- `attendance.pdf` → Retrieve C5 and C6 and combine them.
The restore service uses these instructions to reconstruct the original files.

#### Why do we need a manifest for incremental backups?

This is where manifests become even more useful.

Suppose we take backups on three days.

**Day 1 – Full backup**

All files are backed up.

| File           | Chunks |
| -------------- | ------ |
| employee.txt   | C1, C2 |
| salary.xlsx    | C3, C4 |
| attendance.pdf | C5, C6 

*Manifest B001 contains all these mappings.*

**Day 2 – Incremental backup**

Suppose only `salary.xlsx` changes.

The system generates new chunks C7 and C8.

| File           | Chunks |
| -------------- | ------ |
| employee.txt   | C1, C2 |
| salary.xlsx    | C7, C8 |
| attendance.pdf | C5, C6 |

*Manifest B002 describes the complete Day 2 recovery point, including references to unchanged chunks.*

**Day 3 – Incremental backup**

Suppose only `employee.txt` changes.

New chunks C9 and C10 are generated.

| File           | Chunks  |
| -------------- | ------- |
| employee.txt   | C9, C10 |
| salary.xlsx    | C7, C8  |
| attendance.pdf | C5, C6  |

*Manifest B003 describes the complete Day 3 recovery point.*

Notice:
- Day 1 stores C1–C6.
- Day 2 physically adds C7 and C8.
- Day 3 physically adds C9 and C10.
    

But each manifest describes the complete logical state of the backup.

```

                 BACKUP MANIFESTS - INCREMENTAL BACKUP
                 =====================================

                  DAY 1 - FULL BACKUP
                         |
                         v
                  +---------------+
                  | MANIFEST B001 |
                  +---------------+
                         |
             +-----------+-----------+
             |           |           |
             v           v           v
          +------+    +------+    +------+
          |  C1  |    |  C2  |    |  C3  |
          +------+    +------+    +------+
             |           |           |
          +------+    +------+    +------+
          |  C4  |    |  C5  |    |  C6  |
          +------+    +------+    +------+
                         |
                         v
                  OBJECT STORAGE
                  Stores C1 - C6


             DAY 2 - INCREMENTAL BACKUP
                         |
                         v
                  +---------------+
                  | MANIFEST B002 |
                  +---------------+
                         |
             +-----------+-----------+
             |           |           |
             v           v           v
          +------+    +------+    +------+
          |  C1  |    |  C2  |    |  C7  |
          +------+    +------+    +------+
          Reused      Reused       NEW
             |           |           |
          +------+    +------+    +------+
          |  C8  |    |  C5  |    |  C6  |
          +------+    +------+    +------+
           NEW        Reused      Reused
                         |
                         v
                  OBJECT STORAGE
                  Existing: C1 - C6
                  Newly added: C7, C8


             DAY 3 - INCREMENTAL BACKUP
                         |
                         v
                  +---------------+
                  | MANIFEST B003 |
                  +---------------+
                         |
             +-----------+-----------+
             |           |           |
             v           v           v
          +------+    +------+    +------+
          |  C9  |    | C10  |    |  C7  |
          +------+    +------+    +------+
           NEW         NEW        Reused
             |           |           |
          +------+    +------+    +------+
          |  C8  |    |  C5  |    |  C6  |
          +------+    +------+    +------+
          Reused      Reused      Reused
                         |
                         v
                  OBJECT STORAGE
                  Existing: C1 - C8
                  Newly added: C9, C10


                 FINAL STORAGE CONTENTS
                 ======================

                  +-------------------+
                  |   OBJECT STORAGE  |
                  +-------------------+
                  | C1                |
                  | C2                |
                  | C3                |
                  | C4                |
                  | C5                |
                  | C6                |
                  | C7                |
                  | C8                |
                  | C9                |
                  | C10               |
                  +-------------------+

                  LEGEND
                  ------
                  Reused = Previously stored chunk
                  NEW    = Newly stored chunk

                  Each manifest represents a complete
                  logical recovery point.

```

#### Where do we store the manifest?

We can store the manifest as a separate metadata object in Object Storage, while maintaining its ID and location in our Metadata Database.

#### What happens during restoration?

Suppose the user wants to restore Day 3.
1. User requests Backup B003.
2. Restore service looks up B003 in the Metadata Database.
3. It retrieves the B003 manifest.
4. The manifest tells it which chunks are required.
5. Restore service fetches those chunks from Object Storage.
6. It combines the chunks in the correct order.
7. Original files are reconstructed.

The most important distinction:

|Component|Stores|
|---|---|
|Object Storage|Actual backup data (chunks)|
|Backup Manifest|Instructions for reconstructing backup data|
|Metadata Database|Information about backups and where their manifests are located|

---
### Database design

We need to store metadata separately from the actual backup data.
Actual backup chunks go into object storage. Metadata goes into a database.

**Let's design some important tables.**
#### A. Machines table

| Column           | Description            |
| ---------------- | ---------------------- |
| machine_id (PK)  | Unique machine ID      |
| machine_name     | Name of machine        |
| owner_id         | User/organization      |
| last_backup_time | Last successful backup |
| status           | Active / Inactive      |

#### B. Backup_Jobs table

| Column          | Description                            |
| --------------- | -------------------------------------- |
| job_id (PK)     | Unique job ID                          |
| machine_id (FK) | Source machine                         |
| backup_type     | Full / Incremental                     |
| status          | Pending / Running / Failed / Completed |
| created_at      | Job creation time                      |
| completed_at    | Completion time                        |

#### C. Backups table

| Column           | Description             |
| ---------------- | ----------------------- |
| backup_id (PK)   | Unique backup ID        |
| machine_id (FK)  | Machine ID              |
| parent_backup_id | Previous recovery point |
| snapshot_id      | Source snapshot         |
| manifest_id      | Manifest reference      |
| backup_type      | Full / Incremental      |
| created_at       | Backup time             |
| status           | Backup status           |

#### D. Chunks table

| Column           | Description            |
| ---------------- | ---------------------- |
| chunk_id (PK)    | Unique chunk ID        |
| content_hash     | SHA-256 hash           |
| storage_location | Object storage key     |
| compressed_size  | Size after compression |
| original_size    | Original size          |
| reference_count  | Number of references   |

#### E. Backup_Chunks table

This table connects backups with chunks.

| backup_id | chunk_id | sequence_number |
| --------- | -------- | --------------- |
| B001      | C001     | 1               |
| B001      | C002     | 2               |
| B002      | C001     | 1               |
| B002      | C002     | 2               |
| B002      | C003     | 3               |

For a production implementation, we'd also maintain immutable or versioned manifests and reliable chunk-reference accounting. The simplified table above illustrates the relationship.

#### Database choice

| Data                        | Possible technology  | Reason                       |
| --------------------------- | -------------------- | ---------------------------- |
| Machine and backup metadata | PostgreSQL           | Relational consistency       |
| Backup job queue            | Kafka / RabbitMQ     | Asynchronous processing      |
| Chunk data                  | S3 / object storage  | Large-scale durable storage  |
| Deduplication index         | Distributed KV store | Fast hash lookup             |
| Cache                       | Redis                | Frequently accessed metadata |

For our Backup / Data Protection System, I would choose SQL (PostgreSQL) for the primary Metadata Database, and optionally use NoSQL for specific high-scale metadata workloads.

#### What does our Metadata DB store?

It stores information about:
- Machines and their owners.
- Backup jobs and their statuses.
- Backup IDs and parent backup IDs.
- Manifest locations.
- Chunk mappings and references.
- Retention policies.

It does NOT store the actual backup file contents.

#### SQL vs NoSQL

| Criteria       | SQL (PostgreSQL)              | NoSQL (DynamoDB/Cassandra)          |
| -------------- | ----------------------------- | ----------------------------------- |
| Data structure | Tables and relations          | Key-value / documents / wide-column |
| Relationships  | Excellent                     | Limited or application-managed      |
| Transactions   | Strong ACID support           | Depends on database                 |
| Schema         | Structured                    | Flexible                            |
| Scaling        | Vertical + horizontal options | Designed for distributed scaling    |
| Queries        | Complex joins supported       | Usually access-pattern driven       |

For example, when a backup is completed, we need to ensure that:
1. All required chunks are uploaded.
2. The manifest is available.
3. The backup status is updated to COMPLETED.

SQL transactions and constraints help maintain metadata consistency.

#### Can we use NoSQL?

Absolutely!
Suppose we have millions of machines and billions of chunk metadata records.

A distributed NoSQL database such as DynamoDB or Cassandra can be useful for:
- Fast chunk-hash lookups.
- Large-scale chunk metadata.
- Distributed access across regions.
- High-throughput key-based queries.

```
"I would initially choose PostgreSQL as the primary metadata database because our backup system has structured entities and relationships between machines, backup jobs, manifests and chunks. ACID transactions are useful for maintaining metadata consistency. As the system grows and we have billions of chunks, we may need a NoSQL database to perform chunk lookups faster and handle large amounts of traffic.

We would store the actual backup data in object storage like Amazon S3, rather than directly in the database."
```

---
### How do we verify backup integrity?

Backup integrity means confirming that saved data is complete, uncorrupted, and actually restorable, rather than just verifying that a backup file was created.

Imagine a backup operation completed successfully, but one chunk became corrupted during transfer or storage.

**How do we detect this?**
We use checksums.

At backup time:
1. Calculate the SHA-256 hash of the original chunk.
2. Upload the chunk.
3. Verify the uploaded data against the expected checksum.
4. Record its checksum in the metadata.

During periodic verification :
1. Read stored chunks.
2. Recalculate their checksums.
3. Compare with the original checksum.
4. Mark corrupted objects.
5. Repair them from another healthy replica, if available.

---
### How do we restore data quickly?

Suppose Machine A crashes and the user requests restoration of its Day 3 backup.

The process:
1. User sends a restore request.
2. Restore service identifies the requested backup.
3. Fetches the corresponding manifest.
4. Reads all required chunk IDs.
5. Fetches chunks from object storage in parallel.
6. Verifies checksums.
7. Decrypts and decompresses chunks.
8. Reconstructs files in the correct order.
9. Writes data to the destination machine.
10. Marks restore job as completed.

#### How do we make restore faster?

- Fetch chunks in parallel.
- Use high-throughput network connections.
- Keep frequently accessed metadata cached.
- Use storage close to the destination.
- Prioritize restore jobs over normal backup jobs.
- Support selective file-level restore rather than always restoring the entire machine.
-
A useful concept here is RTO and RPO.

| Term | Meaning                                       | Example    |
| ---- | --------------------------------------------- | ---------- |
| RPO  | Maximum acceptable data loss measured in time | 1 hour     |
| RTO  | Maximum acceptable recovery time              | 30 minutes |


If RPO is 1 hour, the backup policy must ensure that the maximum gap between recoverable points is no greater than 1 hour.

---

### How do we schedule backups for millions of machines?

Imagine 1 million machines are configured to take backups every day at midnight.
If all machines start simultaneously, we experience a huge traffic spike called a thundering herd problem.

We can solve this using:

1. Distributed schedulers.
2. Time-window scheduling.
3. Randomized jitter.
4. Queue-based job dispatch.
5. Worker autoscaling.

For example, instead of backing up all machines at midnight:
- 12:00 AM – Group A.
- 12:15 AM – Group B.
- 12:30 AM – Group C.
- 12:45 AM – Group D.

We can partition machines using machine IDs and distribute scheduling responsibility across scheduler instances.

**How do we prevent duplicate jobs?**

Use a unique scheduling key such as:

```
machine_id + backup_policy_id + scheduled_time
```

Enforce uniqueness in the job store or use an idempotent job-creation operation.

Distributed scheduler instances can use leases or partition ownership to avoid competing over the same workload.

---

### How do we handle millions of backup objects?

To handle millions or billions of backup objects, we need to think about scalability, storage, database load, availability and retrieval performance.

#### 1. Use Object Storage for backup objects

We should store actual backup chunks in object storage such as Amazon S3 instead of storing them in PostgreSQL.

Why?
- Object storage can handle a massive number of objects.
- It scales without us manually adding storage servers.
- It provides high durability.
- We can organize objects using bucket names and object keys.
- 
#### 2. Use database scaling techniques

Our metadata database stores information about backup objects, such as chunk IDs, hashes and storage locations.

We can use:

Read replicas:
- Create copies of the primary database.
- Read requests can be distributed across replicas.
- This reduces the read load on the primary database.
- Writes still go to the primary database in a typical read-replica setup.

Partitioning / Sharding:

- Partition large tables into smaller parts.
- For example, partition chunk metadata based on machine ID or tenant ID.
- If one database cannot handle the load, shard data across multiple database instances.

#### 3. Use Availability Zones

Availability Zones help us maintain availability when infrastructure fails.
If one Availability Zone fails, services running in another AZ can continue operating.

For object storage such as S3, AWS manages the underlying storage infrastructure across multiple Availability Zones within a Region.

#### 4. Use asynchronous processing

Imagine one million machines request backups at the same time.

We shouldn't process all backup requests immediately.

Instead:
1. Receive backup requests.
2. Put them into a message queue such as Kafka or SQS.
3. Backup workers consume jobs from the queue.
4. Workers process backups in parallel.
5. Scale the number of workers based on queue size and workload.

#### 5. Use deduplication and compression

- Deduplication: Avoid storing the same chunk multiple times.
- Compression: Reduce the size of chunks before storing them.

This reduces storage consumption and network bandwidth.

#### 6. Use caching

Frequently accessed metadata, such as chunk hash-to-storage-location mappings, can be cached using Redis.

This reduces repeated database queries and improves lookup speed.

---
### Failure handling

In a distributed system, failures are expected. Let's consider the important scenarios.

| Failure                | Solution                                   |
| ---------------------- | ------------------------------------------ |
| Worker crashes         | Reassign job to another worker             |
| Queue unavailable      | Persist pending jobs and retry dispatch    |
| Network interruption   | Resume upload from last acknowledged chunk |
| Duplicate job          | Idempotency key                            |
| Storage node failure   | Replication / erasure coding               |
| Database failure       | Replication and point-in-time recovery     |
| Corrupted chunk        | Checksum verification and repair           |
| Client machine crashes | Retry when machine becomes available       |

##### Important: What if a worker crashes after uploading chunks but before updating the database?

This is a classic distributed systems problem.

Consider:
1. Worker uploads 10 chunks.
2. Worker crashes before marking the backup completed.
3. Queue redelivers the job.
4. Another worker processes it again.

We need an idempotent backup workflow.

My approach:

- Give every backup operation a unique job ID.
- Use deterministic chunk identities based on content hashes.
- Make chunk uploads idempotent.
- Store backup metadata in a staging state.
- Verify all required chunks exist.
- Atomically publish the completed manifest and mark the backup committed.
- Allow recovery workers to resume incomplete jobs.

---

### quick revision

**1. How do you schedule backups?

We should understand:
- Periodic scheduling (daily, weekly, monthly).
- Backup Scheduler.
- Message queue.
- Handling missed or overlapping backup jobs.
- Retry mechanisms.

"To handle millions of machines, I would use time-based scheduling instead of triggering all backups simultaneously. The Backup Scheduler distributes jobs across different time slots based on each machine's backup configuration.

When a job becomes due, the scheduler pushes it into a message queue such as Kafka or SQS. Multiple backup workers consume jobs from the queue and process them in parallel.

If a worker fails, the job can be retried. We also use idempotency to prevent duplicate processing. Additionally, we can dynamically scale workers based on the queue size."

**2. How do you verify backup integrity?

We should understand:
- SHA-256 checksums.
- Verifying chunks after upload.
- Verifying data during restoration.
- Detecting corrupted chunks.
- Replication and recovery of corrupted data.

**3. How do you deduplicate data?

"I would implement chunk-level deduplication. First, the backup agent divides files into smaller chunks. We calculate a SHA-256 hash for each chunk and check the hash in our chunk index.
A checksum is calculated based on the actual content (bytes) of the data.

If the hash already exists, we reuse the existing chunk instead of storing it again. Otherwise, we store the new chunk in object storage and update the chunk index.

Finally, the manifest maintains references to the required chunks. This reduces duplicate storage and saves storage costs and network bandwidth."

**4. How do you compress data?

Compression means reducing the size of data before storing it.

For example, suppose a chunk is 10 MB. After compression, it might become 6 MB.
This saves storage space and reduces the amount of data transferred over the network.

"I would use compression to reduce the storage space required for backup data. After chunking and deduplication, we compress the newly stored chunks using an algorithm such as Zstandard.

Then we encrypt the compressed chunks and store them in object storage like Amazon S3.

During restoration, we decrypt and decompress the chunks before reconstructing the original files using the manifest. This reduces storage costs and network bandwidth."

