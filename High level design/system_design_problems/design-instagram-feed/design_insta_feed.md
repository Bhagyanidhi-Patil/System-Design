# Instagram Feed Design

## Functional Requirements

- User can upload photo/video.
- User can follow other users.
- User opens app and sees feed.
- Feed contains posts from followed users.
- Infinite scrolling.
- Posts are sorted using a ranking algorithm (not strictly chronological).
- Like, comment, share.
- Refresh feed.
- New posts should appear quickly.

## Non-Functional Requirements

- Very low latency (<200ms)
- High availability (99.99%)
- Massive scalability
- Eventual consistency acceptable
- Read-heavy system
- Fault tolerant
- Highly durable
- Feed generation should be efficient

## Capacity Estimation

### Assumptions

- 1 billion users
- 300 million daily active users (DAU)
- 100 million posts/day

### Write Traffic

- Average: 100M posts/day
- ≈ 1,157 posts/sec

### Read Traffic

- 300M users
- Suppose each user refreshes 50 times/day
- 15 billion feed requests/day
- ≈ 173K requests/sec

#### Summary

- Reads >> Writes
- Therefore optimize reads.

### High Level Architecture

```
                        +------------------+
                        |      Client      |
                        | (Android/iOS/Web)|
                        +--------+---------+
                                 |
                                 v
                        +------------------+
                        |   Load Balancer  |
                        +--------+---------+
                                 |
                                 v
                        +------------------+
                        |   API Gateway    |
                        | Authentication   |
                        | Rate Limiting    |
                        +--------+---------+
                                 |
        -------------------------------------------------
        |                  |                |            |
        v                  v                v            v
+---------------+  +---------------+  +--------------+  +---------------+
| User Service  |  | Feed Service  |  | Post Service |  | Media Service |
+---------------+  +---------------+  +--------------+  +---------------+
        |                  |                |                  |
        |                  |                |                  |
        -------------------|----------------|-------------------
                            |
                            v
                  +----------------------+
                  |   Feed Generator     |
                  | Fan-out Logic        |
                  +----------+-----------+
                             |
                -------------------------------
                |                             |
                v                             v
        +---------------+             +----------------+
        |  Feed Cache   |             | Ranking Engine |
        |   (Redis)     |             | ML/Heuristics  |
        +-------+-------+             +--------+-------+
                |                              |
                -------------------------------
                             |
                             v
                     +---------------+
                     | Notification  |
                     |   Service     |
                     +---------------+
                             |
                             v
                     +---------------+
                     | Message Queue |
                     | Kafka/SQS     |
                     +---------------+
                             |
                             v
        -------------------------------------------------
        |                                               |
        v                                               v
+----------------------+                      +----------------------+
| Metadata Database    |                      | Object Storage (S3)  |
| MySQL/Cassandra      |                      | Images & Videos      |
+----------------------+                      +----------------------+
```

## Components

### 1. User Service

#### Responsibility

The User Service manages everything related to users and maintains the social graph.

#### Data stored

- User profile
- Followers
- Following
- Profile picture
- Bio
- Privacy settings
- Blocked users

#### APIs

- `POST /users` — Create a user.
- `GET /users/{id}` — Get user profile.
- `POST /users/{id}/follow` — Follow a user.
- `DELETE /users/{id}/follow` — Unfollow a user.
- `GET /users/{id}/followers` — Get followers.
- `GET /users/{id}/following` — Get following list.

#### Database

- `User` table:
  - `UserID`
  - `Name`
  - `Username`
  - `Bio`
  - `ProfilePic`

- `Followers` table:
  - `FollowerID`
  - `FollowingID`

#### Example

- A follows B
  - `FollowerID = A`
  - `FollowingID = B`

#### Scalability

- Store followers separately.
- Shard the `Followers` table by `UserID`.
- Cache follower counts in Redis.

#### Interview Question

- **Q:** Why separate `User` and `Followers` tables?
- **A:** Followers grow much faster than profile information. Separating them avoids making the `User` table extremely large.

### 2. Post Service

#### Responsibility

The Post Service manages post creation, updates, deletion, and retrieval.

#### Functions

- Create post
- Delete post
- Edit caption
- Fetch post
- Fetch user posts

#### Upload Flow

```
Client
  ↓
Media Service
  ↓
Upload image/video to S3
  ↓
Receive Media URL
  ↓
Post Service
  ↓
Save metadata
```

> Note: The database never stores image files. Only the media URL is stored.

#### Metadata

| Field      | Description       |
|------------|-------------------|
| `PostID`   | Unique Post ID    |
| `UserID`   | Owner             |
| `Caption`  | Text              |
| `Timestamp`| Creation Time     |
| `MediaURL` | S3 URL            |
| `Likes`    | Count             |
| `Comments` | Count             |
| `Visibility` | Public/Private  |

#### APIs

- `POST /posts` — Upload a post.
- `GET /posts/{id}` — Get post.
- `DELETE /posts/{id}` — Delete post.
- `GET /users/{id}/posts` — Get all posts of a user.

#### Database Choice

Common options include:

- MySQL
- PostgreSQL
- Cassandra

Large companies often shard posts by `UserID`.

#### Scalability

Instead of storing the image in the database:

```
Database
   ↓
Image URL
   ↓
S3
```

Benefits:

- Database stays small.
- S3 provides virtually unlimited storage.
- Lower cost.

### 3. Feed Service

The Feed Service is responsible for generating the Home Feed.

#### Responsibilities

- Build feed
- Fetch cached feed
- Pagination
- Fetch next page
- Merge sponsored posts
- Call Ranking Service

#### APIs

- `GET /feed` — First page.
- `GET /feed?page=2` — Page number.
- `GET /feed?cursor=abc123` — Cursor pagination (preferred).

#### Why Cursor Pagination?

Suppose new posts arrive while the user is scrolling.

Using page numbers:

- Page 1: Post A, Post B, Post C
- New post inserted
- Page 2 may cause duplicate or missing posts.

Cursor pagination avoids this issue by remembering the last item seen.

#### Feed Flow

```
Feed Request
  ↓
Redis Cache
  ↓
Cache Hit?
  ↓
Yes -> Return Feed
No  -> Feed Generator
         ↓
     Ranking Service
         ↓
       Database
         ↓
     Store in Cache
         ↓
       Return Feed
```

#### Scalability

- Cache feeds in Redis.
- Use CDN for media.
- Compress responses.
- Lazy load older posts.

### 4. Ranking Service

Instagram does not show posts only in chronological order. It predicts what the user is most likely to engage with.

#### Inputs

The ranking model considers signals such as:

- User interactions
- Likes
- Comments
- Shares
- Saves
- Watch time
- Relationship score
- Freshness
- Account popularity
- ML prediction score

#### Flow

```
Candidate Posts
  ↓
Ranking Service
  ↓
ML Model
  ↓
Score Every Post
  ↓
Sort
  ↓
Return Ranked Feed
```

#### Example Scores

| Post | Score |
|------|-------|
| A    | 0.98  |
| B    | 0.94  |
| C    | 0.82  |

Feed returned:

- A
- B
- C

#### APIs

Usually internal.

- `POST /rank`
  - Input: `UserID`, candidate posts
  - Output: sorted posts

#### Scalability

- Ranking is CPU intensive.
- Deploy multiple Ranking Service instances.
- Use feature stores.
- Cache ranking features.
- Perform heavy ML inference asynchronously where possible.

### 5. Feed Generator

The Feed Generator decides which posts should appear in a user's feed before ranking. It gathers candidate posts from followed users and distributes updates efficiently.

#### Responsibilities

- Collect posts from followed users.
- Push or prepare feed updates.
- Trigger cache refreshes.
- Send events for downstream services.

#### Two Approaches

##### 1. Fan-out on Write (Push Model)

When a user uploads a post:

```
Celebrity uploads post
  ↓
Feed Generator
  ↓
Push post to every follower's feed cache
```

Advantages:

- Very fast feed reads.
- Excellent user experience.
- Suitable for users with a moderate number of followers.

Disadvantages:

- If a celebrity has 100 million followers, pushing to every feed immediately is extremely expensive.

##### 2. Fan-out on Read (Pull Model)

When a user opens Instagram:

```
User opens app
  ↓
Feed Generator
  ↓
Fetch latest posts from followed users
  ↓
Ranking Service
  ↓
Return Feed
```

Advantages:

- Much lower write cost.
- Better for accounts with massive follower counts.

Disadvantages:

- Higher read latency.
- More computation at request time.

---

### Difference between `Feed-service` and `Feed-Generator`

- `Feed Service` = serves the feed to the user (read path).
- `Feed Generator` = prepares/builds the feed (write path or background processing).

### Feed Service

The Feed Service is called when a user opens Instagram.

#### Responsibilities

- Receive feed requests.
- Check Redis cache.
- Fetch the user's feed.
- Apply pagination.
- Return the ranked feed to the client.

Pagination is used because a user's feed can contain millions of posts, and sending all of them in one response would be extremely slow and wasteful.

#### Example

Suppose you follow 2,000 people, and together they have 500,000 posts.

If Instagram returned every post when you opened the app:

- Response size would be huge.
- Loading would take a long time.
- Mobile data usage would be very high.
- Phone memory consumption would increase.
- Most of those posts would never be viewed because users usually don't scroll that far.

Instead, Instagram sends only a small batch of posts.

### Feed Generator

The Feed Generator prepares or updates feeds.

It usually runs in the background, often triggered by events from a message queue like Kafka.

#### Responsibilities

- Detect new posts.
- Find followers.
- Build candidate feeds.
- Update Redis.
- Trigger ranking.
- Precompute feeds (Fan-out on Write).

#### Triggered when

- Someone uploads a post.
- Someone follows/unfollows another user.
- Feed cache expires.
- Background feed refresh jobs run.

#### Real Example

Suppose:

- Alice has 1 million followers.
- Alice uploads a new photo.

`Feed Generator's job`:

```
Alice uploads
  ↓
Feed Generator
  ↓
Get Alice's followers
  ↓
Insert the new post into each follower's feed cache
  ↓
Done
```

This happens in the background.

Later, Bob opens Instagram.

`Feed Service's job`:

```
Bob opens app
  ↓
Feed Service
  ↓
Read Bob's feed from Redis
  ↓
Return posts in milliseconds
```

The Feed Service doesn't compute the feed from scratch if it's already prepared.

---

## Database design

### 1. Users Table

Stores basic profile information.

| Column     | Type    | Description           |
|------------|---------|-----------------------|
| UserID (PK)| BIGINT  | Unique user ID        |
| Name       | VARCHAR | User's name           |
| Username   | VARCHAR | Unique username       |
| Photo      | VARCHAR | Profile picture URL   |
| Bio        | TEXT    | User bio              |
| CreatedAt  | TIMESTAMP | Account creation time |

**Example**

| UserID | Name  | Username  | Photo              |
|--------|-------|-----------|--------------------|
| 101    | Alice | alice123  | s3://profile1.jpg  |

**Primary Key:** UserID

### 2. Followers Table

Represents the social graph—who follows whom.

| Column         | Type    | Description              |
|----------------|---------|--------------------------|
| FollowerID (FK)| BIGINT  | User who follows         |
| FollowingID (FK)| BIGINT | User being followed      |
| Timestamp      | TIMESTAMP | Follow time            |

**Example**

| FollowerID | FollowingID |
|------------|--------------|
| 101        | 205          |
| 101        | 300          |
| 205        | 300          |

This means:

- Alice (101) follows Bob (205)
- Alice (101) follows Charlie (300)
- Bob (205) follows Charlie (300)

**Primary Key**

Usually a composite key:

- `(FollowerID, FollowingID)`

This prevents duplicate follow relationships.

**Why separate table?**

A user may have millions of followers.

Keeping followers inside the Users table would make it extremely large and difficult to scale.

### 3. Posts Table

Stores metadata about every post.

Images and videos are NOT stored here.

Only their storage URLs are saved.

| Column        | Type    | Description      |
|---------------|---------|------------------|
| PostID (PK)   | BIGINT  | Unique post ID   |
| UserID (FK)   | BIGINT  | Owner            |
| Caption       | TEXT    | Caption          |
| MediaURL      | VARCHAR | S3/Blob URL      |
| Timestamp     | TIMESTAMP | Creation time  |

**Example**

| PostID | UserID | Caption      |
|--------|--------|--------------|
| 9001   | 101    | Sunset 🌅    |

### 4. Likes Table

Stores which users liked which posts.

| Column      | Type    | Description      |
|-------------|---------|------------------|
| UserID (FK) | BIGINT  | User who liked   |
| PostID (FK) | BIGINT  | Liked post       |
| Timestamp   | TIMESTAMP | Like time      |

**Example**

| UserID | PostID |
|--------|--------|
| 205    | 9001   |
| 300    | 9001   |

**Primary Key**

- Composite key: `(UserID, PostID)`

This ensures a user cannot like the same post multiple times.

### 5. Comments Table

Stores comments on posts.

| Column        | Type    | Description        |
|---------------|---------|--------------------|
| CommentID (PK)| BIGINT  | Unique comment ID  |
| PostID (FK)   | BIGINT  | Related post       |
| UserID (FK)   | BIGINT  | Comment author     |
| Text          | TEXT    | Comment content    |
| Timestamp     | TIMESTAMP | Creation time   |

**Example**

| CommentID | PostID | UserID | Text         |
|-----------|--------|--------|--------------|
| 5001      | 9001   | 205    | Nice photo!  |

### 6. Feed Cache (Redis)

This is not a database table. It's an in-memory cache used for very fast reads.

For each user, Redis stores a precomputed feed.

**Example:**

**Key:** `feed:101`

**Value:**

```
[
 9001,
 9002,
 9003,
 9004,
 9005
]
```

When User 101 opens Instagram:

```
Feed Service
      │
      ▼
Redis

↓

Returns

9001
9002
9003
9004
```

The Feed Service then fetches the corresponding post details (or uses cached post metadata) and returns them to the client.

**Why Redis?**

- Extremely fast (sub-millisecond reads).
- Reduces database load.
- Ideal for frequently accessed data like home feeds.

---

## Storage Choice

### 1. User Data

Can be stored in:

- MySQL
- PostgreSQL

Stores:

- User Profile
- Authentication
- Settings

**Why?**

- Strong consistency
- ACID transactions
- Relationships are manageable

### 2. Posts, Likes, Comments, Feed

At Instagram scale, use a distributed NoSQL database like:

- Cassandra
- DynamoDB

**Why?**

#### 1. Huge Scale

Instagram has:

- Billions of users
- Billions of posts
- Millions of feed requests every second

A single relational database cannot handle this efficiently.

#### 2. Horizontal Scaling

Need to add more servers easily.

- Server 1
- Server 2
- Server 3
- Server 4

Cassandra and DynamoDB automatically distribute data across multiple machines.

#### 3. Fast Writes

Millions of posts, likes, and comments are created every minute.

Cassandra is optimized for very high write throughput.

#### 4. High Availability

If one server fails:

```
Server A ❌
↓
Replica Server B
↓
Application keeps running
```

Data is replicated across multiple nodes.

#### 5. Partitioning (Sharding)

Example:

- User 1–1M → Node 1
- User 1M–2M → Node 2
- User 2M–3M → Node 3

Each server stores only part of the data.

### Image Storage

Never store images in the database.

Instead:

```
Client
↓
Media Service
↓
S3
↓
URL
↓
Posts Table
```

The database stores:

- `PostID`
- `Caption`
- `MediaURL`

Image stored in:

- Amazon S3
- Google Cloud Storage
- Azure Blob Storage

---

## If user info in stored in MqSQL and likes, comments are stored in NoSQL, then how do we get user's likes and comments information? Do we join sql and nosql ?

You don't perform SQL joins across MySQL and Cassandra/DynamoDB. Instead, the application or dedicated services combine the data.

### Why not join?

Suppose:

- MySQL stores users.
- Cassandra stores posts, likes, and comments.

A query like:

```sql
SELECT *
FROM Users u
JOIN Likes l ON u.UserID = l.UserID;
```

is not possible because the data lives in different databases.

Instead, the application does multiple lookups.

### Example 1: Display a Post

Suppose a user opens a post.

#### Step 1: Get the post

From Cassandra:

- `PostID = 101`
- `Caption`
- `MediaURL`
- `UserID = 25`

#### Step 2: Get the user

Now the application calls the User Service:

- `GET /users/25`

MySQL returns:

- `Name = Alice`
- `Photo = alice.jpg`

#### Final response

- Alice
- Photo
- Sunset 🌅

The application merged the data.

### Example 2: Show Comments

Cassandra returns:

- Comment1, UserID = 10
- Comment2, UserID = 20
- Comment3, UserID = 30

Instead of a join, the application asks the User Service for those users.

- `GET /users?ids=10,20,30`

It receives:

- `10 → John`
- `20 → Mike`
- `30 → Emma`

Then it combines them.

---

## What is Feed Generation?

When you open Instagram, you see posts from people you follow.

The question is:

- How does Instagram prepare this feed?

There are two main ways.

### Option 1: Fan-out on Write (Push Model)

Imagine:

- Alice uploads a photo.
- Alice has 4 followers: Bob, Charlie, David, Emma.

```
Alice uploads post
       |
       v
Feed Generator
       |
---------------------
|    |     |       |
v    v     v       v
Feed(B) Feed(C) Feed(D) Feed(E)
```

The Feed Generator immediately copies Alice's new post into each follower's feed.

Later...

- Bob opens Instagram.

```
Bob
↓
Feed Service
↓
Redis
↓
Return Feed
```

Bob's feed is already prepared. No extra work is needed.

#### Example

Before upload:

- Bob Feed
  - Post 10
  - Post 9
  - Post 8

Alice uploads Post 11.

Immediately:

- Bob Feed
  - Post 11
  - Post 10
  - Post 9
  - Post 8

The same happens for Charlie, David, and Emma.

#### Advantage

- Very fast reads.

When Bob opens Instagram:

```
Redis
↓
Return Feed
```

- Done in milliseconds.

#### Problem

Suppose Cristiano Ronaldo has 100 million followers.

He uploads one photo.

Instagram now has to insert that photo into:

- 100 million feeds.

That means 100 million write operations.

- This is extremely expensive and can overload the system.

### Option 2: Fan-out on Read (Pull Model)

Instead of copying the post to everyone's feed, Instagram stores it only once.

```
Alice uploads
↓
Posts Database
```

Nothing is pushed to followers.

Now Bob opens Instagram.

```
Bob opens app
↓
Feed Service
↓
Who does Bob follow?
↓
Alice
Charlie
David
↓
Fetch their latest posts
↓
Merge
↓
Sort
↓
Return Feed
```

So the feed is created only when Bob requests it.

#### Advantage

- Uploading is very cheap.
- Only one write is needed.

```
Alice uploads
↓
Posts Database
```

That's all.

#### Problem

Every time Bob opens Instagram:

- Find everyone he follows.
- Fetch their latest posts.
- Merge them.
- Sort them.
- Rank them.

This makes reading slower.

### Comparison

| Fan-out on Write | Fan-out on Read |
|------------------|-----------------|
| Expensive writes | Cheap writes |
| Fast reads | Slower reads |
| Feed is precomputed | Feed is generated on demand |
| Good for users with fewer followers | Good for celebrities |

### Hybrid Approach (Used by Instagram)

Instagram combines both strategies.

#### Normal User

Suppose Alice has 200 followers.

She uploads a photo.

```
Alice uploads
↓
Feed Generator
↓
Push to 200 feeds
```

- 200 writes are manageable.

#### Celebrity

Suppose Virat Kohli has 300 million followers.

He uploads a photo.

Instead of pushing to 300 million feeds:

```
Virat uploads
↓
Store post only
```

Later, when someone opens Instagram:

```
Open Feed
↓
Fetch Virat's latest posts
↓
Merge
↓
Return Feed
```

- This avoids hundreds of millions of writes.

---

## What is Pagination?

Pagination means returning only a small number of posts at a time instead of the entire feed.

### Example

- Request 1 → 20 posts
- When the user scrolls, Request 2 → next 20 posts

### Problem with Page Number Pagination

Suppose your feed initially looks like this (newest first):

- Page 1: 1, 2, 3, 4

You request:

- `GET /feed?page=1`

and receive:

- 1, 2, 3, 4

Now, before you request Page 2, someone uploads a new post.

New feed:

- 0 ← new post
- 1
- 2
- 3
- 4
- 5
- 6
- 7
- 8

Now you request:

- `GET /feed?page=2`

If each page contains 4 posts, Page 2 is now:

- 4, 5, 6, 7

#### Notice the problem

- Post 4 appears twice (it was already on Page 1).
- You may also miss some posts depending on how the feed changed.

This happens because page numbers assume the data never changes, but social media feeds are constantly changing.

### Cursor Pagination

Instead of saying:

- "Give me Page 2"

you say:

- "Give me posts after the last one I saw."

The cursor usually contains information like:

- Last post ID
- Last timestamp
- Encoded position

For example:

- `Cursor = PostID 4`
- or `Cursor = Timestamp 10:35:22`

### Example

Initial feed:

- 1
- 2
- 3
- 4
- 5
- 6
- 7
- 8

You receive:

- 1
- 2
- 3
- 4

The server also returns:

- `Next Cursor = PostID 4`

Now a new post arrives:

- 0
- 1
- 2
- 3
- 4
- 5
- 6
- 7
- 8

Your next request is:

- `GET /feed?cursor=4`

The server understands:

- "Return posts after Post 4."

Response:

- 5
- 6
- 7
- 8

- No duplicates.
- No missing posts.

### Why Timestamp?

Feeds are usually ordered by time.

Instead of storing a page number, we store the timestamp of the last post seen.

Example:

- Post A — 10:30
- Post B — 10:25
- Post C — 10:20
- Post D — 10:15

User has seen up to 10:15.

Next request:

- `cursor=10:15`

Server returns:

- Posts older than 10:15

This keeps scrolling stable even if new posts arrive.

### Cursor pagination behavior

With cursor pagination, new posts inserted at the top are not included in the current scrolling session. This is intentional because the cursor provides a consistent snapshot of the feed and avoids duplicates or skipped posts.

New content is typically shown when the user refreshes the feed or taps a “New Posts” indicator, rather than being inserted into the middle of an active scrolling session.

There are two different actions:

- Load next page (continue scrolling)
- Refresh feed (reload from the top)

They are not the same request.

### Case 1: Scrolling (Next Page)

Initial feed:

- 1
- 2
- 3
- 4
- 5
- 6
- 7
- 8

You load the first page:

- 1
- 2
- 3
- 4

Cursor = 4

Now a new post arrives:

- 0
- 1
- 2
- 3
- 4
- 5
- 6
- 7
- 8

When you ask for the next page:

- `GET /feed?cursor=4`

You get:

- 5
- 6
- 7
- 8

You don't see Post 0, because you're continuing the same scrolling session.

### Case 2: User refreshes the feed

Refreshing does not call:

- `GET /feed?cursor=4`

Instead, it starts a brand new feed request:

- `GET /feed`
- or `GET /feed?cursor=null`

The server ignores the old cursor and returns the newest posts:

- 0
- 1
- 2
- 3

Now the new post is visible.

---

## Handling Videos

Videos are much larger than images.

For example:

- Image → 2 MB
- Video → 100 MB

If videos were stored in the database:

```
Database
↓
100 MB video
```

The database would become huge and slow.

Instead:

```
User uploads video
↓
Media Service
↓
Amazon S3
↓
Returns Video URL
↓
Save URL in Database
```

The database stores only:

| PostID | VideoURL |
|--------|-----------|
| 101    | s3://videos/video1.mp4 |

### Why CDN?

Suppose the original video is stored in an AWS data centre in the US.

A user in India requests the video.

Without CDN:

```
India User
↓
US Server
↓
Video
```

Every request travels all the way to the US.

This causes:

- High latency
- Slow video start
- Heavy load on the origin server

With CDN:

A CDN (Content Delivery Network) keeps copies of popular videos at edge servers close to users.

```
              S3 (Origin)
                   |
          ------------------
          |       |        |
       USA CDN  India CDN Europe CDN
                   |
                User
```

Now an Indian user gets the video from the India CDN, not from the US.

#### Benefits

- Low latency
- Faster video streaming
- Reduced load on S3
- Better user experience

## Load Balancer

Suppose there are three Feed Service servers.

- Feed Server 1
- Feed Server 2
- Feed Server 3

If all requests go to Server 1:

```
10000 Users
↓
Server 1
```

Server 1 becomes overloaded while the others are idle.

With Load Balancer:

```
            Users
              |
      Load Balancer
      /     |      \
     /      |       \
Feed1    Feed2    Feed3
```

The load balancer distributes requests across all servers.

#### Benefits

- No single server is overloaded.
- Higher availability.
- Easy horizontal scaling by adding more servers.

## Sharding

Imagine Instagram has 10 billion posts.

Keeping everything on one database server is not practical.

Instead, split the data into multiple shards.

### Example

- Shard 1: Users 1–10M
- Shard 2: Users 10M–20M
- Shard 3: Users 20M–30M

If User 15M uploads a post:

```
User 15M
↓
Shard 2
```

If User 5M uploads a post:

```
User 5M
↓
Shard 1
```

### Why shard by UserID?

Because most operations are based on the user:

- Get a user's posts
- Upload a post
- Delete a post

Using the UserID makes it easy to locate the correct shard.

#### Benefits

- Horizontal scaling
- Smaller databases
- Faster queries
- Higher throughput

## Message Queue (Kafka)

Suppose Alice uploads a post.

Many things need to happen:

- Save post
- Update followers' feeds
- Send notifications
- Generate thumbnails
- Update analytics

If everything happens synchronously:

```
Upload
↓
Save Post
↓
Update 1M Feeds
↓
Send Notifications
↓
Generate Thumbnail
↓
Return Success
```

The upload may take several seconds.

With Kafka:

```
Upload
↓
Save Post
↓
Publish Event to Kafka
↓
Return Success
```

In the background:

```
           Kafka
        /     |      \
       /      |       \
Feed Gen Notification Analytics
```

Each service consumes the event independently.

### Why Kafka?

#### 1. Retry on Failure

Suppose the Notification Service crashes.

```
Kafka
↓
Notification Service ❌
```

The event stays in Kafka.

When the service recovers:

```
Kafka
↓
Notification Service
↓
Send Notification
```

The event isn't lost.

#### 2. Decouples Services

The Post Service doesn't need to know about:

- Feed Generator
- Notification Service
- Analytics

It simply publishes an event.

Any interested service can consume it.

#### 3. Handles Traffic Spikes

Imagine a celebrity with 100 million followers uploads a post.

Instead of processing everything immediately:

```
100M Feed Updates
↓
Kafka Queue
↓
Feed Generator processes gradually
```

Kafka buffers the work, preventing sudden overloads.ost.

---

## Instagram Feed End-to-End Flow
```
                    User Opens Instagram
                            │
                            ▼
                     Load Balancer
                            │
                            ▼
                      API Gateway
                            │
                            ▼
                      Feed Service
                            │
                ┌───────────┴───────────┐
                │                       │
          Check Redis Cache        Cache Miss
                │                       │
          Cache Hit?                    ▼
                │               Feed Generator
        ┌───────┴────────┐             │
        │                │             ▼
      Yes               No      Get Following List
        │                      (User Service)
        │                            │
        ▼                            ▼
 Return Cached Feed         Fetch Posts (Post Service)
        │                            │
        ▼                            ▼
                   Ranking Service (ML Ranking)
                            │
                            ▼
                   Store Feed in Redis
                            │
                            ▼
                     Return Feed
                            │
                            ▼
                Client loads Images/Videos
                            │
                            ▼
                          CDN
                            │
                            ▼
                     S3 Object Storage
```

## Complete Architecture Flow
```
                          User
                            │
                            ▼
                     Load Balancer
                            │
                            ▼
                      API Gateway
                            │
      ┌───────────────┬───────────────┬──────────────┐
      │               │               │              │
      ▼               ▼               ▼              ▼
 User Service    Feed Service    Post Service   Media Service
      │               │               │              │
      │               │               │              ▼
      │               │               │             S3
      │               │               │
      │               ▼               ▼
      │        Feed Generator      Kafka
      │               │        ┌────┼────┐
      │               ▼        ▼    ▼    ▼
      │        Ranking Service Feed Notification Analytics
      │               │
      │               ▼
      │            Redis
      │               │
      └───────────────┼──────────────────────────┐
                      ▼                          ▼
              Cassandra/DynamoDB              CDN
                 (Metadata)                    │
                                               ▼
                                            Images/
                                             Videos

```