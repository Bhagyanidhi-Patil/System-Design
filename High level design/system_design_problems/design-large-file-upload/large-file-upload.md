# Large File Upload System

## 1. Requirements

### Functional requirements

- Upload files ranging from a few megabytes to 100+ GB.
- Download files after upload.
- Resume an interrupted upload without retransmitting completed chunks.
- Detect corrupted chunks and retry them.
- Prevent duplicate chunks or duplicate file creation during retries.
- Support file deletion and metadata retrieval.

### Non-functional requirements

- High availability and durability.
- Horizontal scalability.
- High throughput for large files.
- Fault tolerance against network, server, and storage failures.
- Secure access to files.

## 2. High-Level Architecture

Imagine uploading a 100 GB video to a service such as Google Drive. The system handles it using the following components.

### 2.1 Client

The client is the user's device, such as a laptop or mobile phone.

- Splits the file into smaller pieces called **chunks**.
- Calculates a checksum for each chunk to verify data integrity.

For example, instead of uploading one huge file, the client can divide it into 1,000 smaller chunks.

### 2.2 HTTPS and API requests

HTTPS is the secure communication channel between the client and the server. The client sends requests such as:

- "I want to upload a file."
- "Here is chunk 1."
- "Is my upload complete?"

Requests and responses travel securely over the internet.

### 2.3 API Gateway and load balancer

The API Gateway acts as the entry point to the system. It handles:

- **Authentication:** Checks whether the user is allowed to upload.
- **Rate limiting:** Prevents too many requests from being sent too quickly.
- **Routing:** Sends requests to the appropriate service.

The load balancer distributes incoming requests across available servers to avoid overloading any one server.

### 2.4 Upload Service

The Upload Service manages the upload process. It interacts with the Metadata DB and Object Storage, sending different information to each:

- **Metadata DB:** Stores file information, chunk details, checksums, and upload status.
- **Object Storage:** Stores the actual file chunks.

The Upload Service:

- Creates an upload session and assigns it an upload ID.
- Tracks which chunks have been uploaded.
- Checks whether all required chunks are available.
- Marks the upload as complete when everything is ready.

For example, if 600 of 1,000 chunks have been uploaded before the internet connection drops, the service tracks that progress so the client can resume instead of starting over.

### 2.5 Metadata DB

The Metadata Database stores information about the file, not the file contents. For example:

- File name: `video.mp4`
- File size: `100 GB`
- Upload ID: `UP123`
- Uploaded chunks: `1–600`
- Chunk checksums
- Upload status: `In progress`

Think of it as a register that tracks file details and upload progress.

### 2.6 Object Storage

Object Storage holds the actual file data. Uploaded chunks are stored there:

- Chunk 1 → Object Storage
- Chunk 2 → Object Storage
- Chunk 3 → Object Storage
- And so on

The Metadata DB records information about the chunks, while Object Storage holds their contents.

### 2.7 Storage nodes

Storage nodes are the underlying resources that hold the data. The storage system can protect data using:

- **Replication:** Keeps multiple copies of data so it can survive certain failures.
- **Erasure coding:** Stores data as fragments with extra recovery information, allowing lost fragments to be reconstructed.

These techniques help protect files if some storage resources fail.

## 3. Chunk Checksums

A **checksum** is a value used to detect whether data has changed or become corrupted.
A checksum is similar to a hash, and in our file upload system, we can use a hash function such as SHA-256 to calculate a checksum.

Imagine uploading a 100 GB file divided into 1,000 chunks:

1. **The client calculates a checksum.** Before uploading a chunk, the client calculates a checksum from its data.
2. **The chunk is uploaded.** The client sends it to Object Storage.
3. **The receiving system calculates a checksum.** It calculates a checksum from the uploaded data.
4. **The checksums are compared.**
   - If they match, the data is likely correct.
   - If they do not match, the data may have been corrupted during transfer or storage, so the system can reject it and retry the upload.

### Identifying chunks

Identify each chunk uniquely within its upload session.

For example, suppose a user uploads a 100 GB file split into chunks:

| Upload ID | Chunk number | Chunk ID | Checksum |
|---|---:|---|---|
| `U123` | 0 | `U123_0` | `HASH_A` |
| `U123` | 1 | `U123_1` | `HASH_B` |
| `U123` | 2 | `U123_2` | `HASH_C` |
| `U123` | 3 | `U123_3` | `HASH_D` |

The fields work as follows:

- **Upload ID:** Identifies the entire upload session.
- **Chunk number:** Identifies a particular chunk within that session.
- **Checksum:** Verifies that the chunk's contents are correct.

Construct the chunk identifier from `uploadId` and `chunkNumber`; a separate random ID for every chunk is not necessarily needed.

## 4. Does the Client Upload a File or Chunks?

The user selects one file, but the client application splits it into smaller chunks before uploading.

For example, when the user selects a 100 GB video:

1. The user selects one file.
2. The client splits it into 1,000 chunks.
3. The client uploads those chunks separately, often in parallel.

The user uploads one file, but technically the client uploads multiple chunks.

## 5. Request Paths

| Request | Typical path |
|---|---|
| Start a new upload | Client → API Gateway → Upload Service |
| Request upload instructions | Client → API Gateway → Upload Service |
| Upload actual chunk data | Client → Object Storage directly |
| Report or confirm chunk completion | Client → API Gateway → Upload Service |
| Update metadata | Upload Service → Metadata DB |

The checksum can be sent as part of the upload request or metadata, depending on the design. Object Storage or the Upload Service can verify it against the actual chunk data.

## 6. Why Upload Chunks Directly to Object Storage?

Imagine a million users uploading large files. If all file data passes through the Upload Service, it can become a bottleneck and consume substantial server bandwidth.

Direct uploads allow Object Storage to handle the heavy data transfer while the Upload Service manages the upload process.

## 7. Why Use an Upload Service if Data Goes Directly to Object Storage?

- Object Storage knows how to store data, while the Upload Service manages the upload process.
- The Upload Service creates the upload session and provides upload instructions; the client uploads the actual file chunks directly to Object Storage.
- The client waits until it receives the upload instructions from the Upload Service before starting the upload.
- The Upload Service updates the Metadata DB before the client starts uploading chunks to Object Storage.

Think of it like sending luggage at an airport:

- **Object Storage** is where the luggage is stored.
- **Upload Service** manages the booking, provides instructions, and tracks the luggage.
- **Metadata DB** is the register that records the luggage details.

The manager does not need to physically carry every bag. Likewise, the Upload Service does not need to transfer every byte of the file.

**Here is the exact sequence:**
1. `Client → Upload Service`: Requests to upload a file.
2. `Upload Service → Metadata DB`: Creates an upload session with status IN_PROGRESS, file details, chunk size, and total chunk count.
3. `Upload Service → Client`: Returns the upload ID and upload instructions.
4. `Client → Object Storage`: Uploads the actual chunks directly.
5. `Upload Service → Metadata DB`: Records verified chunk information as the upload progresses, depending on the implementation.
6. `Upload Service`: Finalizes the upload and updates the file status to AVAILABLE after successful verification.

### What the Upload Service does

Suppose the internet disconnects after 600 of 1,000 chunks have been uploaded. The Upload Service helps the system:

1. Maintain the upload session and its ID.
2. Identify the expected chunks and track their status.
3. Provide upload instructions, such as presigned URLs.
4. Help verify that all required chunks have been uploaded.
5. Finalize the upload and mark it as complete.
6. `The Upload Service updates the Metadata DB to track the upload session and its progress.`
## 8. End-to-End Upload Flow

### Step 1: The user selects a file

The client splits the file into smaller chunks.

### Step 2: The Upload Service prepares the upload

The service creates an upload ID, records metadata, and provides upload instructions.

### Step 3: The client uploads chunks to Object Storage

The client uploads the actual data directly using the provided instructions. The Upload Service does not need to handle the file bytes.

### Step 4: The Upload Service completes the process

The service checks that all required chunks are uploaded and valid, then updates the metadata to mark the file as complete.

```text
User selects file
       |
       v
Client splits file into chunks
and calculates checksums
       |
       v
API Gateway / Load Balancer
(Authentication and Routing)
       |
       v
Upload Service
(Creates upload session)
       |
       +--------------------+
       |                    |
       v                    v
Metadata DB            Client receives
(Stores metadata)      upload instructions
                            |
                            v
                   Object Storage
                   (Stores file chunks)
                            |
                            v
                   Verify checksums
                            |
                            v
                   All chunks uploaded?
                     /           \
                   No             Yes
                   |               |
                   v               v
             Retry missing    Upload Service
                 chunks       verifies completion
                                   |
                                   v
                              Metadata DB
                           (Marks upload complete)
                                   |
                                   v
                           Upload successful

Object Storage
       |
       v
Replication / Erasure Coding
(Protects stored data)
```

## 9. Core Upload Flow

1. **Initiate upload:** The client sends the file name, file size, content type, and optional checksum.
2. **Create session:** The Upload Service generates an `uploadId`, selects a chunk size, and saves the status as `IN_PROGRESS` in the Metadata DB.
3. **Upload chunks:** The client splits the file into chunks and uploads them to Object Storage using presigned URLs or the Upload API.
4. **Validate each chunk:** Verify its checksum and record successfully uploaded chunks.
5. **Resume if needed:** The client checks the upload status and uploads only missing or invalid chunks.
6. **Complete upload:** The client requests completion. The service checks that all chunks are present and verifies their integrity.
7. **Publish file:** Finalize the object in storage, commit metadata in the Metadata DB, and mark the file as `AVAILABLE`.

```text
Initiate upload
Client sends file metadata
       |
       v
Create session
Upload Service creates uploadId,
selects chunk size, and saves IN_PROGRESS
       |
       v
Upload chunks
Client sends chunks to Object Storage
using presigned URLs or Upload API
       |
       v
Validate each chunk
Verify checksum and record successful chunks
       |
       v
Resume if needed
Upload missing or invalid chunks
       |
       v
Complete upload
Verify all chunks and their integrity
       |
       v
Publish file
Finalize object and commit metadata
Mark file as AVAILABLE
       |
       v
Upload successful
```

## 10. How Do You Chunk a Large File?

Split the file into fixed-size chunks. A good starting point is 64 MB per chunk. Tune the size according to storage backend limits, network conditions, and concurrency.

The number of chunks is calculated as:

```text
Number of chunks = ceil(file size / chunk size)
```

For example, a 100 GB file using decimal GB and 64 MB chunks requires approximately 1,563 chunks.

Each chunk has its own identity and checksum:

| Chunk | Byte range | Status |
|---:|---|---|
| 0 | 0–64 MB | Uploaded |
| 1 | 64–128 MB | Uploaded |
| 2 | 128–192 MB | Failed |
| 3 | 192–256 MB | Uploaded |

The client can upload multiple chunks in parallel, subject to a concurrency limit.

### Why chunking?

- Avoids retransmitting the entire file when a connection fails.
- Allows parallel uploads for higher throughput.
- Limits memory requirements because the client can stream chunks.
- Allows individual chunks to be verified and retried.

Chunk size is a trade-off: smaller chunks mean more requests and metadata; larger chunks mean more data retransmitted when a chunk fails.

## 11. How Do You Resume a Failed Upload?

Use a resumable upload session. When the client initiates an upload, the server creates a record such as:

```yaml
uploadId: U123
fileName: video.mp4
fileSize: 100 GB
chunkSize: 64 MB
status: IN_PROGRESS
```

The server tracks each chunk independently:

```text
U123:
  Chunk 0 -> UPLOADED
  Chunk 1 -> UPLOADED
  Chunk 2 -> FAILED
  Chunk 3 -> UPLOADED
```

Suppose the client loses its network connection after uploading chunk 3:

1. The client reconnects.
2. It calls `GET /uploads/U123/status`.
3. The server returns the verified, successfully stored chunks.
4. The client uploads only missing or invalid chunks.
5. Once all chunks are uploaded, the client requests completion.

## 12. How Do You Detect Corrupted Chunks?

Use checksums, typically SHA-256 for strong content verification, or a suitable checksum supported by the storage backend.

For every chunk:

1. The client calculates the chunk checksum.
2. It sends the checksum alongside the chunk.
3. The server or storage layer calculates or independently verifies the checksum.
4. If the checksums match, the chunk is accepted.
5. If they differ, the chunk is rejected and retried.

## 13. How Do You Make Retries Idempotent?

**Idempotency** means that repeating the same request multiple times has the same effect as executing it once.

### Example: Uploading chunk 5

Suppose the client uploads chunk 5 to Object Storage:

1. The chunk is uploaded successfully.
2. The server sends a success response, but the response is lost due to a network issue.
3. The client thinks the upload failed and retries chunk 5.

The system must recognize that chunk 5 was already uploaded successfully.

### Chunk identity

Identify each chunk using its upload ID and chunk number:

```text
Chunk identity = uploadId + chunkNumber

uploadId     = UP101
chunkNumber  = 5
checksum     = ABC123
```

The service checks whether the chunk has already been uploaded:

| Situation | Action |
|---|---|
| Same upload ID, chunk number, and checksum | Return success without creating another record or unnecessarily uploading the chunk again. |
| Same upload ID and chunk number, but different checksum | Reject the request as a conflict, unless an explicit replacement operation is supported. |
| First upload of this chunk | Store the chunk and record its metadata. |

### Why not store the entire file in the Metadata DB?

Large binary payloads would increase database storage costs, backups, replication traffic, and operational load. Object Storage is designed for durable, scalable storage of large objects.

For very large uploads, a practical implementation can use native multipart upload in Object Storage rather than manually persisting every chunk as an independent object. The metadata service still tracks the upload session and its progress.

## 14. How Would You Replicate Data?

**Replication** means storing multiple copies of the same data on different storage nodes so the data remains available if one node fails.

Suppose a file contains chunk 42. The storage system may keep copies in separate availability zones:

```text
File chunk: Chunk 42
       |
       +-- Original data / Replica 1 — Zone A
       +-- Replica 2 — Zone B
       +-- Replica 3 — Zone C
```

- Each replica contains the same chunk data.
- We can replicate individual chunks across different storage nodes instead of replicating the entire file as one unit.It depends on how we design the storage system.

### How replication works

1. **Placement:** Store copies on different storage nodes, preferably in different availability zones.
2. **Write acknowledgement:** Confirm the write according to durability rules. For example, the system may wait for multiple replicas to be stored successfully.
3. **Read:** If one storage node fails, read from another healthy replica.
4. **Repair:** If a replica is lost or corrupted, create a replacement copy using a healthy replica.
5. **Consistency:** Mark the file `AVAILABLE` only after the required storage and finalization conditions are satisfied.

### What if Zone A fails?

Suppose Replica 1 becomes unavailable:

- Replica 1 cannot serve the data.
- The system reads Chunk 42 from Replica 2 or Replica 3.
- The system can rebuild the missing replica when the failed storage becomes available or replacement capacity is ready.

The user does not necessarily need to upload the file again.

## 15. Where Would You Use Erasure Coding?

**Erasure coding** splits data into fragments and creates extra parity fragments so the original data can be recovered if some fragments are lost.

Instead of storing three complete copies of a file, the system stores the original data as fragments plus recovery information. For example, a 6+3 scheme creates six data fragments and three parity fragments:

```text
                 9 TOTAL FRAGMENTS
                (6 + 3 Erasure Coding)

        +-----------+  +-----------+  +-----------+
        |    D1     |  |    D2     |  |    D3     |
        |   DATA    |  |   DATA    |  |   DATA    |
        +-----------+  +-----------+  +-----------+

        +-----------+  +-----------+  +-----------+
        |    D4     |  |    D5     |  |    D6     |
        |   DATA    |  |   DATA    |  |   DATA    |
        +-----------+  +-----------+  +-----------+

        +-----------+  +-----------+  +-----------+
        |    P1     |  |    P2     |  |    P3     |
        |   PARITY  |  |   PARITY  |  |   PARITY  |
        +-----------+  +-----------+  +-----------+

        6 data fragments + 3 parity fragments
```

- **Data fragments (D1–D6):** Contain the original file's encoded data.
- **Parity fragments (P1–P3):** Contain mathematically calculated recovery information.

In a `K + M` erasure-coding scheme:

- `K` is the number of data fragments.
- `M` is the number of parity fragments.

| Scheme | Total fragments | Fragments that can be lost |
|---|---:|---:|
| `2+1` | 3 | 1 |
| `4+2` | 6 | 2 |
| `6+3` | 9 | 3 |
| `10+4` | 14 | 4 |

### Large File Upload and Erasure Coding: Four Steps

1. **Upload in chunks**
   - The client splits the large file into smaller chunks and uploads them directly to Object Storage.
   - Each chunk is verified using its checksum.
2. **Finalize the file**
   - Once all chunks are uploaded and verified, Object Storage finalizes the complete file.
   - The Upload Service updates the Metadata DB and marks the file as `AVAILABLE` when the durability requirements are met.
3. **Apply erasure coding**
   - The storage system divides the data into data fragments and generates parity fragments.
   - These fragments are distributed across different storage nodes to save storage space while providing fault tolerance.
4. **Handle node failure**
   - If a storage node fails, the system uses the remaining fragments to reconstruct the missing data.
   - It stores replacement fragments on healthy nodes to restore protection.
### What happens if three fragments are lost?

Suppose D1, P1, and D5 become unavailable. The system still has six fragments available. With a 6+3 Reed–Solomon scheme, any six of the nine fragments are sufficient to reconstruct the original data.

### Why is erasure coding cheaper?

For a 100 GB original file:

| Storage method | Approximate storage needed |
|---|---:|
| One copy | 100 GB |
| Three replicas | 300 GB |
| 6+3 erasure coding | 150 GB |

That means 100 GB of original data needs approximately 150 GB of storage, excluding other overheads.

## 16. How Do You Handle a Storage Node Failure?

Assume storage node N2 fails while holding a chunk of a large file.

1. **Detect the failure:** Heartbeats, health checks, timeouts, and failed storage requests identify the unhealthy node.
2. **Route around it:** Reads use healthy replicas or reconstructable fragments instead.
3. **Verify recovered data:** Check checksums before trusting a replacement copy or reconstructed chunk.
4. **Repair durability:** Copy from healthy replicas or reconstruct the missing fragment and store it on a replacement node.

If a node fails during an upload, the client retries the affected chunk against a healthy storage destination. The upload session remains `IN_PROGRESS`, and already verified chunks remain available.

If the storage backend already manages replication, erasure coding, and repair internally, the Upload Service should rely on those guarantees rather than independently implementing a second storage-repair mechanism.

If all copies or enough erasure-coded fragments are lost, the data may be unrecoverable. Independent failure domains, monitoring, backups where required, and periodic integrity checks reduce this risk.