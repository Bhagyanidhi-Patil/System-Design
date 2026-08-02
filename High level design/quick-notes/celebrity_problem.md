# Celebrity Problem

The Celebrity Problem is one of the most common deep-dive questions in HLD interviews, especially when designing systems like 
- Twitter/X
- Instagram 
- Facebook Feed
- LinkedIn Feed
- and YouTube Subscription Feed.

> **"What happens if Cristiano Ronaldo (600M followers) posts a tweet?"**

This is called the `Celebrity Problem`. A user with millions of followers creates an enormous spike in work for the system. The standard industry solution is a `hybrid fan-out strategy: push updates for normal users, but pull updates for celebrities at read time.`

## Feed Generation Basics

Suppose:

- A follows B
- A follows C
- A follows D

When A opens Twitter, the system has to show:

- B's tweet
- C's tweet
- D's tweet

There are two approaches.

## Approach 1: Fan-out on Write (Push Model)

Whenever someone posts, such as B posting a tweet, the system immediately copies it to every follower's feed.

```text
B posts
    |
    |---- A
    |---- X
    |---- Y
```

The system writes:

- Feed(A) += Tweet
- Feed(X) += Tweet
- Feed(Y) += Tweet

So when A opens Twitter, the system just reads Feed(A). This is very fast.

### Advantages

- Read becomes extremely fast.
- User opens the app and gets the feed immediately.
- This is perfect for normal users.

### Problem

Suppose Cristiano Ronaldo has 600 million followers and posts a tweet like "Hello".

The system must perform:

- 600 million writes
- 600 million DB writes
- 600 million Redis writes
- 600 million network requests

This creates a huge spike and may crash servers.

## Approach 2: Fan-out on Read (Pull Model)

Instead of pushing to followers, the system stores the tweet only once in the tweet database.

When the user opens the app:

1. Get the people I follow
2. Fetch the latest tweets
3. Merge them
4. Return the feed

### Advantage

A single tweet results in:

- one DB write
- one write even for 1 billion followers

This avoids write explosion.

### Problem

If 1 million users open Twitter at the same time, every request has to fetch the same celebrity tweets. This creates hot keys and increases read latency.

## Comparison

| Approach | Read Performance | Write Performance | Best For |
|---------|------------------|-------------------|----------|
| Push Model | Fast | Heavy | Normal users |
| Pull Model | Slower | Light | Celebrities |

## Industry Solution: Hybrid Approach

Companies use a combination of both:

- Normal users use push
- Celebrities use pull

### Rule

- Followers < 100K ? Push
- Followers >= 100K ? Pull

The threshold is configurable based on system capacity.

## Example

Alice has 300 followers. When she posts, pushing to 300 feeds is easy.

Ronaldo has 600 million followers. When he posts, the system does not push to all followers. Instead, it stores the tweet once in the tweet table and later fetches it during feed generation.

When the user opens the feed, the system:

1. Reads the pushed feed
2. Fetches celebrity posts
3. Merges them
4. Returns the final feed

## Why Cache Celebrity Posts?

If 10 million users refresh at the same time, they all request Ronaldo's latest post. Without caching, the database receives millions of reads and becomes overloaded.

Instead, the system uses Redis cache to serve these requests and reduce database load. Large systems often replicate these hot cache entries to spread the load.

## Interview Answer

If the interviewer asks, `"Explain the Celebrity Problem,"` you can answer:

> In a social media feed, fan-out-on-write works well for normal users because every new post is copied into followers' timelines, making reads very fast. However, if a celebrity with millions of followers posts, one write becomes millions of timeline updates, creating massive write amplification. To solve this, large systems use a hybrid approach: normal users use push, while celebrity posts are stored once and fetched during feed generation. The feed service merges the precomputed timeline with recent celebrity posts and caches celebrity content aggressively to avoid hot-key database reads.
