## How Block storage works? 
- Basically, both block storage and file storage ultimately store files and folders. However, in block storage, the storage is provided to the client as raw blocks (like a disk), so the client/server needs to create and manage the filesystem and mount the volume. In file storage, the storage system already manages the filesystem, and the client can directly access files and folders through a share.
- In block storage, the storage system provides a block device such as a LUN. The LUN consists of blocks of storage and is presented to the client/server like a disk. The client/server then creates and manages the filesystem on the LUN, mounts it, and stores files and folders on that filesystem.
- We create a filesystem on the LUN, create a directory that acts as a mount point, and mount the filesystem from the LUN to that directory. Then we can access and store files and folders through that directory.
### The complete flow is:
```
SAN Storage
    ↓
LUN
    ↓
Create Filesystem
(ext4 / XFS / NTFS)
    ↓
Create a Directory (Mount Point)
/data
    ↓
Mount the Filesystem at /data
    ↓
Store Files and Folders
```

**Example**

Suppose the server receives a LUN as:
```
/dev/sdb

Then conceptually:

/dev/sdb  → LUN
    ↓
Create filesystem
    ↓
ext4 filesystem
    ↓
Create directory
/data
    ↓
Mount filesystem at
/data
```
`After mounting:`
```
/data
 ├── file1.txt
 ├── images/
 └── documents/
```

## How file storage works? 
- In `NAS (Network Attached Storage)`, we use shares to provide clients access to storage.
- A share does not necessarily have to already contain files and folders. When you create a new share, it can be empty:
```
Share
 |
 └── (Empty)
```
Then clients can create files and folders inside it:
```
Share
 |
 ├── file1.txt
 ├── documents/
 └── images/
```
- In NAS, we use shares, and the share gives clients direct access to a filesystem where files and folders can be stored. The NAS manages the filesystem, while clients directly access and manage the files and folders inside the share.
- The client simply accesses the share using protocols such as:
```
NFS → Linux/Unix
SMB → Windows
```
- The `NAS administrator/storage system does create or configure a filesystem internally` before exposing storage as a share. It's just that the client accessing the NAS share doesn't have to create it.


# SAN / Block Storage

With SAN, the storage system gives the server a **LUN (raw block storage)**:

```text
SAN
 |
 ▼
LUN (Blocks)
 |
 ▼
Server
 |
 ▼
Server creates filesystem
(ext4 / XFS / NTFS)
 |
 ▼
Files and Folders
```

Here, **the server/client manages the filesystem**.

---

# NAS / File Storage

With NAS, the NAS storage system already has a filesystem and manages it.

```text
NAS Storage
 |
 ▼
Filesystem
 |
 ▼
Share
 |
 ▼
Files and Folders
 |
 ▼
Client accesses via NFS / SMB
```

The client simply accesses the share:

```text
Linux Client
     |
   NFS
     |
     ▼
NAS Share
     |
 ├── file1.txt
 ├── images/
 └── documents/
```

So the client does **not normally receive raw blocks** and does **not create a filesystem on the NAS storage**.
### Simple comparison ⭐

|                                     | SAN                  | NAS                |
| ----------------------------------- | -------------------- | ------------------ |
| Storage presented as                | LUN / blocks         | Files and folders  |
| Filesystem exists?                  | Yes                  | Yes                |
| Who usually manages the filesystem? | Server using the LUN | NAS storage system |
| Client sees                         | Disk/device          | Share/folder       |
| Common protocols                    | iSCSI, Fibre Channel | NFS, SMB           |
