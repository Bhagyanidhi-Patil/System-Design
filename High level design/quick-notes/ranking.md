# Ranking

Ranking is the process of assigning a relevance score to each candidate item using multiple signals and ordering the items by that score.

Unlike simple sorting, ranking combines factors such as freshness, popularity, user interests, engagement history, and relationship strength to estimate what the user is most likely to engage with.

In large-scale systems like Instagram, Twitter, or YouTube, the flow is:

1. generate a candidate set
2. extract features
3. compute a ranking score using business rules or an ML model
4. sort by score
5. return the top N items

## Simple example

Suppose your Instagram feed has these posts:

| Post | Likes | Posted      | Friend      | You usually like? |
|------|-------|-------------|-------------|-------------------|
| A    | 500   | 10 min ago  | Yes         | Yes               |
| B    | 2000  | 5 min ago   | No          | No                |
| C    | 100   | 1 min ago   | Best Friend | Yes               |
| D    | 800   | 2 hours ago | Celebrity   | Sometimes         |

If Instagram only sorted by time:

- C
- B
- A
- D

If sorted by likes:

- B
- D
- A
- C

But that is not what you actually see.

Instagram computes a ranking score.

### Ranking score example

```text
Score = 0.4 × FriendScore
      + 0.3 × LikeProbability
      + 0.2 × Freshness
      + 0.1 × Engagement
```

Suppose the scores are:

| Post | Score |
|------|-------|
| C    | 96    |
| A    | 91    |
| B    | 78    |
| D    | 65    |

Final feed:

- C
- A
- B
- D

Notice:

- not sorted by likes
- not sorted by time
- sorted by ranking score

## Why ranking matters

Imagine Twitter.

You follow:

- Elon Musk
- Cricket news
- Friends
- Companies
- Memes

Every minute, 1000+ tweets arrive.

Showing everything chronologically is not useful.

Instead, Twitter asks:

> Which 20 tweets should this user see first?

Ranking answers that question.

## Ranking signals by application

Different applications use different ranking signals.

### Instagram

- Recency
- Likes
- Comments
- Shares
- Saves
- Story interactions
- Follow relationship
- Time spent viewing similar posts
- User interests
- Video completion
- Creator popularity

### YouTube

- Watch history
- Watch time
- Click-through rate (CTR)
- Video quality
- User interests
- Channel subscriptions
- Like/dislike ratio
- Comments

### Amazon

- Purchase history
- Ratings
- Reviews
- Price
- Delivery speed
- Popularity
- Seller quality

### Google Search

- Keyword relevance
- Page quality
- Authority
- Freshness
- User location
- Mobile friendliness
- Page speed
- Backlinks

## How ranking works

### Step 1: Candidate generation

Suppose there are 500 million posts. No system can rank all of them in real time.

Instead, retrieve a smaller set of candidates first.

Example candidates:

- Friends' posts
- Followed accounts
- Trending posts
- Sponsored posts

```text
500M
  ↓
1000 candidates
```

### Step 2: Feature collection

For every candidate post, gather features.

Example features:

- Friend score
- Time since posted
- Number of likes
- Comments
- Shares
- Previous interactions
- Video length
- User interests
- Language
- Location

### Step 3: Score calculation

Calculate a score for each candidate using features.

```text
Score(Post) = f(features)
```

Example:

```text
Score = 0.3 × Freshness
      + 0.4 × Engagement
      + 0.2 × FriendScore
      + 0.1 × InterestScore
```

In real systems, the ranking function may be a:

- machine learning model
- neural network
- gradient boosted tree
- deep learning ranking model

### Step 4: Sort

Sort candidates by score from highest to lowest.

Example order:

- Post C
- Post A
- Post D
- Post B

### Step 5: Return top N

Select only the top items required for the feed.

- Top 20
- Top 10
- Top 50

## Ranking pipeline

```text
                User opens feed
                       │
                       ▼
             Candidate generation
            (1000 possible posts)
                       │
                       ▼
               Feature extraction
      (likes, recency, friends, interests)
                       │
                       ▼
                Ranking model
         (calculates relevance score)
                       │
                       ▼
            Sort by highest score
                       │
                       ▼
               Return top 20 feed
```
