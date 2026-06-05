 
 # Functional Requirements
 
 - One-to-one chat?
 - Group chat?
 - Online/offline status?
 - Message history?
 - Read receipts?
 - Typing indicators?
 - Attachments/images/videos?
 - Message search?
 - Message edits/deletes?
 
 # Non-Functional Requirements
 
 - How many users?
 - Daily active users?
 - Latency requirements?
 - Availability target?
 - Message durability?
 - Global users?
 
 ### Example
 
 ```
 100 million registered users
 20 million DAU
 One-to-one and group chat
 Messages should arrive within 1 second
 Message history retained forever
 High availability
 ```
 
 # High-Level Architecture
 
 ```
 +-----------+
 | Mobile/Web|
 +-----------+
       |
       v
 +----------------+
 | Load Balancer  |
 +----------------+
       |
       v
 +----------------+
 | Chat Gateway   |
 | (WebSocket)    |
 +----------------+
       |
       v
 +----------------+
 | Chat Service   |
 +----------------+
       |
       +------------------+
       |                  |
       v                  v
 +------------+   +---------------+
 | Message DB |   | Presence DB   |
 +------------+   +---------------+
       |
       v
 +----------------+
 | Notification   |
 | Service        |
 +----------------+
 ```
 
 # The Core Requirement of a Chat Application
 
## Why Do We Need WebSockets in a Chat Application?

Suppose User A wants to send a message to User B:

```
User A  ---------->  Server  ---------->  User B
         "Hello"
```

Sending the message from User A to the server is straightforward. For example:

```http
POST /sendMessage
{
  "to": "UserB",
  "message": "Hello"
}
```

The server receives and stores the message. The real challenge is: how does User B receive the message immediately?

### Option 1 — HTTP Polling

Because HTTP is request/response, the server cannot push data to User B. So User B repeatedly asks:

```
Any new messages?
```

Flow:

```
User B ------> Server
               Any new messages?

Server ------> No

User B ------> Server
               Any new messages?

Server ------> No

User A ------> Server
               Hello

User B ------> Server
               Any new messages?

Server ------> Yes, here's the message
```

Problems:

- Most requests return `No new messages`.
- Wastes CPU, network bandwidth, and server resources.
- Adds noticeable delay.

### Option 2 — Long Polling

The server keeps the client's request open until a message arrives (or a timeout):

```
User B ------> Server
               Any new messages?

               (waiting...)

User A ------> Server
               Hello

Server ------> User B
               Hello
```

This reduces useless requests, but after delivering the message the connection closes and the client must re-open a request, which still adds overhead.

### Option 3 — WebSocket

Instead of repeated requests, the client opens a persistent connection to the server.

**Handshake:**

```
User B ------> Server
               Upgrade to WebSocket

Server ------> User B
               101 Switching Protocols
```

Now a permanent connection exists:

```
User B <===================> Server
```

When User A sends a message:

```
User A ------> Server
               Hello

Server ------> User B
               Hello
```

User B never had to poll — the server pushed the message instantly. This is the main advantage of WebSockets.

#### Why WebSockets Are Preferred

Chat features demand real-time behavior:

- Instant message delivery
- Typing indicators
- Read receipts
- Online/offline presence
- Notifications

WebSockets provide:

- Persistent connections
- Full-duplex communication
- Low latency
- Reduced network overhead

Typically both sender and receiver maintain WebSocket connections to the server:

```
User A <===================> Server <===================> User B
          WebSocket                     WebSocket
```

### Typical Chat Flow

1) User A connects:

```
User A --------------------> Server
      WebSocket Handshake

User A <===================> Server
```

2) User B connects:

```
User B --------------------> Server
      WebSocket Handshake

User B <===================> Server
```

3) User A sends a message over the existing WebSocket:

```
User A ----------> Server
         "Hello"
```

Server receives a payload such as:

```json
{
  "from": "UserA",
  "to": "UserB",
  "message": "Hello"
}
```

4) Server looks up User B's connection mapping (example):

```
UserA -> Socket #101
UserB -> Socket #205
UserC -> Socket #307
```

5) Server pushes the message using User B's open WebSocket:

```
Server ----------> User B
          "Hello"
```

User B receives the message instantly.

## Why not HTTP ?

- HTTP follows a request-response model. The client initiates communication by sending a request, and the server responds with a response. Once the response is sent, that request is complete, and the server cannot independently initiate communication or push new data to the client. As a result, in a chat application, clients must continuously poll or use long polling to check for new messages. This leads to unnecessary requests, higher latency, increased bandwidth usage, and additional server load.

- WebSockets solve this problem by establishing a persistent, full-duplex connection between the client and server. After an initial HTTP handshake and protocol upgrade, the connection remains open, allowing both the client and server to send messages at any time. This enables the server to instantly push messages, typing indicators, read receipts, and presence updates to connected users without waiting for client requests. Because of lower overhead, lower latency, and true real-time communication, WebSockets are the preferred choice for chat applications.

---

## One-to-One Messaging

Suppose User A wants to send a message to User B.

### Connections

```
User A <=======> Chat Server
User B <=======> Chat Server
```

When users connect, the server maintains a mapping:

```
UserA -> Socket 101
UserB -> Socket 205
```

### Message Flow

User A sends:

```json
{
  "from": "A",
  "to": "B",
  "message": "Hello"
}
```

Flow:

```
User A
   |
   | Hello
   v
Server
   |
   | Lookup UserB
   v
User B
```

The server:

- Receives the message.
- Stores it in the database.
- Finds User B's active WebSocket.
- Pushes the message to User B.

```
Flow:

User A sends a message.
Server stores the message in the database.
Server checks who the recipient is (User B).
Server looks up User B's active WebSocket connection.
If User B is online, server pushes the message immediately.
```

```
A -----> Server -----> B
```

## Group Messaging

Suppose a group has:

- A
- B
- C
- D

### Group Metadata

The database stores:

```
Group1
 ├─ A
 ├─ B
 ├─ C
 └─ D
```

### Message Flow

User A sends:

```
Hello Everyone
```

Server receives:

```json
{
  "groupId": "Group1",
  "message": "Hello Everyone"
}
```

The server:

- Stores the message once.
- Fetches group members.
- Pushes to all online members.

```
              B
             /
A --> Server ----> C
             \
              D
```
**NOTE:**
For small systems:
```
Fetch group members from DB
```
is acceptable.

For large-scale systems:
```
Store group membership in cache (Redis)
```
because fetching group members from the database for every message would be expensive.


## What If Recipient Is Offline?

Suppose a group has:

- A
- B
- C
- D

And the presence state is:

- A -> Online
- B -> Offline
- C -> Online
- D -> Online

User A sends:

```
Hello Group
```

### Server behavior

The server delivers to online members:

```
Server -----> C
Server -----> D
```

Delivered immediately.

### Offline user handling

User B cannot receive the message because B has no active WebSocket connection.

Instead, the server stores the message in the database:

```text
Message:
Group = G1
Recipient = B
Status = Pending
```

### When B comes online

Later:

```
B <=======> Server
```

A new WebSocket connection is established.

The server checks which messages B missed.

For example:

- Message 101
- Message 102
- Message 103

Then the server sends only those pending messages to B:

```
Server -----> B
```

## Online vs Offline Users

The server keeps track of connection status:

```
UserA -> Connected
UserB -> Connected
UserC -> Offline
```

This status is usually stored in Redis for fast lookup.

- When a WebSocket connects: `User Status = Online`
- When disconnected: `User Status = Offline`

---

# Database Choice

## What Are We Storing?

A chat application stores different kinds of data:

### User Data

```json
{
  "userId": 123,
  "name": "Alice",
  "email": "alice@gmail.com"
}
```

### Chat Metadata

```json
{
  "chatId": 1001,
  "participants": ["A", "B"]
}
```

### Messages

```json
{
  "messageId": 5001,
  "chatId": 1001,
  "senderId": "A",
  "content": "Hello",
  "timestamp": 1710000000
}
```

### Group Metadata

```json
{
  "groupId": 2001,
  "name": "Engineering Team",
  "members": [...]
}
```

## Option 1: Relational Database (MySQL/PostgreSQL)

Tables:

- Users
- Chats
- Messages
- Groups

Messages table schema:

```
message_id
chat_id
sender_id
content
created_at
```

### Advantages

**Strong Consistency:**

- Message saved = guaranteed
- No ambiguity.

**Easy Relationships:**

```sql
SELECT *
FROM messages
WHERE chat_id = 123
ORDER BY created_at;
```

Very natural.

**ACID Transactions:**

Useful for payments, orders, banking. Less critical for chat.

### Problems

At huge scale with billions of messages, a single SQL server becomes difficult to scale and sharding becomes complex.

## Option 2: NoSQL (Most Common Choice)

Examples:

- MongoDB
- Cassandra
- DynamoDB

### Why NoSQL Fits Chat

Messages are naturally document-like:

```json
{
  "messageId": 1,
  "chatId": 123,
  "senderId": 45,
  "text": "Hello",
  "timestamp": 123456
}
```

No complex joins needed.

### Cassandra Example

**Partition Key:** `chatId`

**Clustering Key:** `timestamp`

Data layout:

```
Chat 123

10:01 Hi
10:02 Hello
10:05 How are you?
```

Reading recent messages becomes efficient.

### Advantages

**Massive Write Throughput:**

Chat is write-heavy (user sends message → store message). Millions of times per second. Cassandra excels here.

**Horizontal Scaling:**

```
Node1
Node2
Node3
Node4
```

Data automatically distributes.

**High Availability:**

If one node fails, a replica serves requests and chat continues working.

## Recommendation

For a chat application, messages are generated continuously and the system is extremely write-heavy. We mostly perform simple operations: storing a message and retrieving messages for a particular chat ordered by timestamp. We rarely need complex joins or multi-table transactions.

A NoSQL database such as Cassandra is ideal because it provides:

- High write throughput
- Horizontal scalability
- Replication
- Data partitioning across multiple nodes

A relational database like MySQL or PostgreSQL can store messages, but at scale, sharding and scaling become complex.

**Common design:** Use NoSQL for message storage and a relational database for user profiles, group metadata, authentication, and other relational data.

## SQL vs NoSQL: Joins

> **Note:** NoSQL databases do not support joins the way SQL databases do, or joins are discouraged because they are expensive in distributed systems.

### SQL Example

Suppose you have:

```
Users
------
user_id
name

Messages
--------
message_id
sender_id
text
```

To get messages along with sender names:

```sql
SELECT m.text, u.name
FROM Messages m
JOIN Users u
ON m.sender_id = u.user_id;
```

SQL databases are designed for this.

### NoSQL Approach — Denormalization

In NoSQL, data is often denormalized.

Instead of storing:

```json
{
  "senderId": 123,
  "message": "Hello"
}
```

You might store:

```json
{
  "senderId": 123,
  "senderName": "Alice",
  "message": "Hello"
}
```

When reading messages:

```json
{
  "senderId": 123,
  "senderName": "Alice",
  "message": "Hello"
}
```

You already have everything you need — no join required.

### Why Avoid Joins in NoSQL?

Imagine a chat system with billions of messages:

- Message data might be on **Node 1**
- User data might be on **Node 7**

A join would require:

```
Node 1 <----> Node 7
```

Network communication for every query. At scale, that's slow and expensive.

**Design principle:** Store together what you read together.

---

# Scaling Chat Servers

## Single Chat Server

Initially:

```
User A <=======> Chat Server
User B <=======> Chat Server
User C <=======> Chat Server
```

Works fine for a few thousand users.

### Problem

Suppose 10 million users are online. One server cannot maintain 10 million WebSocket connections due to:

- Memory limits
- CPU limits
- Network limits

So we add more servers.

## Multiple Chat Servers

```
                 Load Balancer
                       |
        --------------------------------
        |              |              |
        v              v              v

    Chat Server1   Chat Server2   Chat Server3
```

Users get distributed. For example:

```
User A -> Server1
User B -> Server2
User C -> Server3
```

## Inter-Server Communication

### The Challenge

> If User A is connected to Server 1 and User B is connected to Server 2, how does the message reach User B?

With a single chat server, it's easy:

```
User A <=======> Chat Server <=======> User B
```

But at scale:

```
                Load Balancer
                      |
        -----------------------------
        |                           |
        v                           v

     Server 1                   Server 2

        ^                           ^
        |                           |
     User A                      User B
```

User A and User B are connected to different servers.

### The Problem

User A sends: `Hello`

```
User A ---> Server1
```

Server1 receives:

```json
{
  "to": "UserB",
  "message": "Hello"
}
```

But UserB is connected to Server2. Server1 cannot directly write to User B's WebSocket because the WebSocket exists on Server2.

### Finding the Recipient Server

We maintain a user-to-server mapping, usually in Redis:

```
user:A -> Server1
user:B -> Server2
user:C -> Server3
```

When a user connects, the server registers in Redis:

```
user:B = Server2
```

## Message Flow (5 Steps)

**Step 1:** User A sends `Hello` to Server1.

```
A -----> Server1
```

**Step 2:** Server1 stores the message in the database.

```
Server1 ---> Database
```

> **Important:** Store first. Deliver second. This prevents message loss.

**Step 3:** Server1 checks where UserB is.

```
Redis says: UserB -> Server2
```

**Step 4:** Server1 sends an event to Server2.

There are multiple ways. The most common is **Redis Pub/Sub:**

```
Server1 ---> Redis Pub/Sub
```

Event payload:

```json
{
  "to": "UserB",
  "message": "Hello"
}
```

Redis forwards to Server2:

```
Redis ---> Server2
```

**Step 5:** Server2 finds User B's WebSocket and pushes the message.

```
UserB -> Socket #205
```

```
Server2 ---> UserB
```

Done.

### Visualization

```
User A
   |
   v
Server1
   |
   | Store Message
   v
Database
   |
   | Publish Event
   v
Redis / Kafka
   |
   v
Server2
   |
   v
User B
```

## Why Not Direct Server-to-Server Calls?

You could call Server1 → Server2 directly, but imagine 100 chat servers. Every server must know:

- Where every user is
- How to reach every server

This becomes a mess.

Using Redis/Kafka:

```
Server1 --> Redis --> Server2
```

Servers become loosely coupled and much easier to scale.

## Group Message Example

Suppose a group has A, B, C, D with connections:

```
A -> Server1
B -> Server2
C -> Server3
D -> Server2
```

User A sends: `Hello Group`

Server1:

- Stores the message
- Finds all members
- Publishes the event

```
Server1
    |
    v
  Redis
    |
    +-------+-------+
    |       |
    v       v
Server2  Server3
```

Redis delivers to Server2 and Server3:

```
Server2 pushes to: B, D
Server3 pushes to: C
```

## Offline Users

If UserB is offline:

```
Redis lookup: UserB -> Not Found
```

No active WebSocket exists. The server:

- Stores the message in the database
- Marks it as pending

Later, when UserB reconnects:

```
UserB <=======> Server
```

The server loads all undelivered messages and sends them.

## Redis vs Kafka for Inter-Server Communication

### Key Question

If a server misses the message, do I care?

### Redis Pub/Sub

Redis Pub/Sub is like a live radio broadcast:

```
Server1 ---> Redis ---> Server2
```

- If Server2 is listening → Server2 gets the message
- If Server2 is down → Message is lost

Redis Pub/Sub does not keep a permanent copy for consumers.

**Use Redis when:**

You only need real-time delivery.

**Examples:**

- **Typing Indicator:** Alice is typing. If Bob misses this event because his server restarted, no big deal.
- **Online/Offline Presence:** Alice came online. If one event is missed, the next heartbeat/update will fix it.

**Routing Chat Messages:** Why Redis is okay here:

```
Server1 ---> Redis ---> Server2
```

Because the actual message is already stored in the database:

1. Save message to DB
2. Publish via Redis

Even if Redis delivery fails, the message still exists in the DB. The recipient can get it later.

### Kafka

Kafka is like a post office:

```
When Server1 sends: Hello

Kafka stores it:
Topic: messages

Offset 1
Offset 2
Offset 3
```

If Server2 is down, no problem. When Server2 comes back:

```
Read from offset 3
and gets the message.

Nothing is lost.
```

---

# Reliability and ACKs

## The Problem

Suppose User A sends a message: `Hello`

Flow:

```
A -----> Server -----> B
```

But what if:

```
A -----> Server   ❌ Network failure
```

or

```
Server -----> B   ❌ Connection lost
```

How do we know whether the message was actually delivered?

## ACK (Acknowledgement)

An ACK is simply: **"I received it."**

### Step 1: User A Sends Message

```json
{
  "messageId": 101,
  "text": "Hello"
}
```

```
A -----> Server
```

### Step 2: Server Stores Message

The sequence is critical:

```
Receive
  ↓
Store in DB
  ↓
ACK
```

**Never ACK before storing.** Always:

1. Receive message
2. Save to database
3. Send ACK back

```
A -----> Server
        Save to DB

Server -----> A
          ACK
```

Now User A knows the message is safely stored:

```
✓ Sent
```

### Step 3: Deliver to B

```
Server -----> B
```

User B receives the message, then sends ACK back:

```
B -----> Server
```

The server updates the message status:

```
✓✓ Delivered
```

---

# Ordering Messages

## The Problem

Suppose User A sends:

- Message 1
- Message 2
- Message 3

Network delays happen. Possible arrival order:

```
Message 2
Message 1
Message 3
```

Wrong order!

## Solution 1: Timestamp

Store messages with timestamps:

```json
{
  "messageId": 101,
  "timestamp": 1001
}
{
  "messageId": 102,
  "timestamp": 1002
}
```

Sort by timestamp.

### Problem with Timestamps

Different devices have different clocks:

```
A clock = 10:00
B clock = 9:58
```

This can cause ordering issues.

## Better Solution: Server Sequence Numbers

The server assigns sequence numbers to ensure ordering:

**Chat 123:**

```
Msg1 -> Sequence 1
Msg2 -> Sequence 2
Msg3 -> Sequence 3
```

Stored:

```json
{
  "chatId": 123,
  "sequence": 1
}
{
  "chatId": 123,
  "sequence": 2
}
{
  "chatId": 123,
  "sequence": 3
}
```

Clients display messages by **sequence number**, not arrival time.

### Example

Messages arrive in wrong order:

```
Seq 2
Seq 1
Seq 3
```

Client sorts by sequence:

```
Seq 1
Seq 2
Seq 3
```

Correct order!

## Group Chat Ordering

Suppose multiple users send messages simultaneously:

- A sends: `Hi`
- B sends: `Hello`
- C sends: `Hey`

The server assigns sequences:

```
Hi     -> Seq 100
Hello  -> Seq 101
Hey    -> Seq 102
```

Everyone sees the same order everywhere:

```
100 Hi
101 Hello
102 Hey
```

---

# Handling Duplicate Messages

## The Problem

User A sends: `Hello`

```
A -----> Server
```

The server stores it but **the ACK is lost**:

```
ACK lost
```

User A thinks the message failed and **retries**:

```
A -----> Server
      Hello again
```

Now the server receives the same message twice. Without protection:

```
Hello
Hello
```

Duplicate message!

## Solution: Message ID

The client generates a unique message ID:

```json
{
  "messageId": "abc123",
  "text": "Hello"
}
```

The server stores it with this ID.

### Retry Scenario

When the client retries:

```json
{
  "messageId": "abc123",
  "text": "Hello"
}
```

The server checks:

```
Already processed? Yes.
Ignore duplicate.
Send ACK again.
```

### Result

Only one copy of the message exists:

```
✓ One message, one ID
```

# Caching in Chat Applications

## Why Do We Need Caching?

Suppose every time a user opens a chat, we query the database:

```
User -> Server -> Database
```

For millions of users, the database becomes overloaded.

Many queries are repetitive:

- Get user profile
- Get group info
- Get recent messages
- Get online status

Instead, we keep frequently accessed data in a cache (usually Redis).

# Media Attachments

Now let's talk about photos, videos, PDFs, voice notes, etc.

### The Wrong Approach

Suppose User A sends a 100 MB video.

Don't do:

```
User A
   |
100 MB Video
   |
Chat Server
   |
Database
```

Problems:

- Chat servers become overloaded.
- Databases become huge.
- Message delivery slows down.

### Better Approach

Store files separately.

Use object storage such as:

- Amazon Web Services's object storage service
- Google Cloud object storage
- Microsoft blob storage

The idea is the same regardless of provider.

#### Upload Flow

Step 1

User selects image:

```
photo.jpg
```

Step 2

Upload file to object storage.

```
User A
    |
    v
Object Storage
```

File stored:

```
/images/photo123.jpg
```

Step 3

Storage returns URL:

```
https://storage/.../photo123.jpg
```

Step 4

Chat message contains only metadata.

```json
{
  "messageId": 100,
  "type": "image",
  "url": "https://storage/.../photo123.jpg"
}
```

Step 5

Message is sent normally.

```
A -> Server -> B
```

Only a small JSON message travels through the chat system.

#### Receiving Side

User B receives:

```json
{
  "type": "image",
  "url": "..."
}
```

App downloads image directly from storage.

```
Object Storage
      |
      v
   User B
```

The chat server does not transfer the actual image.
