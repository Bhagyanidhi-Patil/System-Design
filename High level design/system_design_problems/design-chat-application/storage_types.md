# Storage Types Overview

There are three major storage types:

1. Block Storage
2. File Storage
3. Object Storage

## Think of Them as Storage Models

These are ways of organizing data, not locations.

- Block Storage
- File Storage
- Object Storage

## 1. Block Storage

### Idea

Data is broken into small blocks.

```text
Disk

Block 1
Block 2
Block 3
Block 4
...
```

The storage system doesn't know:

- This is an image
- This is a video
- This is a database record

It only knows:

- Blocks

The operating system combines those blocks into files.

### Real Life Example

Your laptop SSD / Harddisk.

```text
SSD
 ├─ Block 1
 ├─ Block 2
 ├─ Block 3
```

Examples:

- Database disks
- VM disks
- SSDs
- Cloud volumes

### Cloud Example

Your laptop:

- SSD

The SSD is block storage.

On top of that SSD, Windows/Linux creates:

```text
Documents/
Pictures/
Videos/
```

(files and folders).

### What About the Cloud?

Suppose you create a VM (virtual machine) in the cloud.

That VM also needs a disk, just like your laptop needs an SSD.

So the cloud provider gives:

```text
Cloud VM
    |
    +---- Virtual Disk
```

This virtual disk is block storage.

### Visualize It

Your laptop:

```text
Laptop
   |
 SSD (Block Storage)
   |
 Windows/Linux
   |
 Files/Folders
```

Cloud:

```text
VM
  |
Virtual Disk (Block Storage)
  |
Linux
  |
Files/Folders
```

So block storage exists both:

- Your Laptop ✅
- Cloud ✅

## 2. File Storage

### Idea

Organize data using files and folders.

```text
Documents/
   report.pdf

Images/
   photo.jpg

Videos/
   movie.mp4
```

This is what humans are familiar with.

### Why Is It Useful?

- Easy organization.
- Easy sharing.
- Multiple systems can access the same files.

Example:

```text
Team Folder
    |
    +-- shared.docx
```

Everyone can access it.

### Cloud Example

What you see on your laptop:

```text
Documents/
Photos/
Videos/
```

is file storage.

### File Storage in the Cloud

Suppose you have 3 application servers:

- Server1
- Server2
- Server3

All need access to the same files.

For example:

- company_logo.png
- invoice.pdf
- config.json

If every server stores its own copy:

```text
Server1 -> logo.png
Server2 -> logo.png
Server3 -> logo.png
```

management becomes difficult.

Solution: Shared File Storage

```text
             Shared File Storage
                    |
         ------------------------
         |          |           |
         v          v           v

      Server1   Server2    Server3
```

All servers see:

```text
/shared/logo.png
/shared/report.pdf
```

as if it were a local folder.

So file storage exists both:

- Laptop ✅
- Cloud ✅

## 3. Object Storage

### Idea

Store data as objects.

Each object contains:

```text
Object
 ├─ Data
 ├─ Metadata
 └─ Unique Key
```

Example:

```text
Key:
images/photo123.jpg

Data:
actual image

Metadata:
size = 2 MB
owner = UserA
```

### How Is It Different?

Instead of:

```text
Folder
   File
```

you access using:

Unique Object Key

Example:

- `images/photo123.jpg`
- `https://storage/.../photo123.jpg`

### Why Is It Useful?

Designed for:

- Massive Scale
- Huge Files
- High Durability
- Low Cost

Object storage systems can store:

- Billions of files
- Petabytes of data

### Cloud Association

Most people encounter object storage through cloud services, so it feels like:

`Object Storage = Cloud Storage`

But that's not technically true.

You can run object storage yourself in your own data center.

Object storage is not inherently cloud storage, but it became extremely popular because it is a perfect fit for cloud environments.

---

## Why Object Storage Became Popular in the Cloud?

Cloud providers need to store:

- Billions of images
- Billions of videos
- Backups
- Logs
- Documents

for millions of users.

Traditional file systems don't scale as easily to that size.

Object storage was designed for:

- ✅ Massive scalability
- ✅ High durability
- ✅ Low cost
- ✅ Easy access via APIs/URLs

### Why Not File Storage?

Imagine:

```text
10 billion photos
```

A traditional file system with folders and subfolders becomes difficult to manage and scale.

Object storage handles this naturally because every object has a unique key:

```text
images/photo123.jpg
images/photo124.jpg
videos/video999.mp4
```

No need for complex file system hierarchies.

## Why Block Storage?

Block storage is popular whenever an application needs very fast, low-level read and write access to data.

Block storage gives raw disk-like access.

```text
Application
     |
Operating System
     |
Block Storage
```

The application can read and write data very efficiently.

This is why databases love block storage.

### Most Common Use Case: Databases

Suppose you have:

- Users Table
- Messages Table
- Orders Table

The database constantly does:

- Read row
- Write row
- Update row
- Delete row

These operations require:

- Low latency
- Fast random access

Block storage is ideal.

### Chat Application Example

Your message database:

- PostgreSQL
- MongoDB
- Cassandra

will ultimately store data on:

- SSD / Disk
At the lowest level, every database ultimately stores its data on storage devices such as SSDs or HDDs.
which is block storage.

```text
Messages
      |
   Database
      |
 Block Storage
```
