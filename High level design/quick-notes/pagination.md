
## Pagination

Pagination is one of those topics that comes up often in system design interviews because almost every large application needs to show data in chunks rather than loading everything at once. Pagination is used to fetch large datasets in smaller chunks instead of loading everything at once.

### Examples

- social media feeds
- product listings
- search results
- chat history
- logs/events

### What is Pagination?

Instead of returning all records:

**Posts:**
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, ...

Return only a small page:

**Page 1:** 1, 2, 3  
**Page 2:** 4, 5, 6  
**Page 3:** 7, 8, 9

### Benefits

- Faster responses
- Lower memory usage
- Less network traffic
- Better user experience

### Why Pagination Matters

Without pagination:

```sql
SELECT * FROM posts;
```

### Problems

- huge memory usage
- slow queries
- high DB load
- network overhead
- poor UX

### Two Main Pagination Approaches

1. **Offset Pagination (Easy but doesn't scale well)**  
Offset pagination retrieves records by skipping a certain number of rows and then returning the next set of rows.

**API:**

```http
GET /posts?offset=20&limit=10
```

**SQL:**

```sql
SELECT *
FROM posts
ORDER BY created_at DESC
LIMIT 10 OFFSET 20;
```

**Meaning:**

- Skip first 20 rows
- Return next 10 rows

### Example

**Data:**

1 2 3 4 5 6 7 8 9 10

**Request:**

offset = 3  
limit = 2

**Result:**

4 5

### Pros

- Very simple
- Easy to implement
- Easy to jump to page 50

### Cons

Imagine 10 million rows:

```sql
OFFSET 9000000
```

Database still scans through millions of rows before returning data.

### Performance becomes:

- Page 1  -> Fast
- Page 1000 -> Slow
- Page 100000 -> Very Slow

### Offset Pagination

Suppose:

```sql
SELECT * FROM posts
ORDER BY id
LIMIT 10 OFFSET 100000;
```

The database typically has to:

- Find the beginning of the ordered result set.
- Walk through (or count through) the first 100,000 rows.
- Discard them.
- Return the next 10 rows.

**Conceptually:**

1 -> 2 -> 3 -> ... -> 100000 -> 100001  
                          ^  
                       start here

So the larger the offset, the more work the database does.

2. **Cursor Pagination (Preferred in large systems)**  
Cursor pagination retrieves records starting from a specific reference point (cursor) instead of skipping rows.

Instead of page number:

**API:**

```http
GET /posts?cursor=12345&limit=10
```

**Meaning:**

Give me 10 posts after ID 12345

**SQL:**

```sql
SELECT *
FROM posts
WHERE id < 12345
ORDER BY id DESC
LIMIT 10;
```

### Why Faster?

Database uses index directly:

- Jump to row 12345
- Read next 10 rows
- Done

No scanning millions of rows.

### Cursor Pagination

Suppose:

```sql
SELECT *
FROM posts
WHERE id > 100000
ORDER BY id
LIMIT 10;
```

If id is indexed, the database can use the index to jump directly near id = 100000.

**Conceptually:**

Index  
 |  
 v  
100000 -> 100001 -> 100002 -> ...

Then it simply reads the next 10 records.

No need to count and discard 100,000 rows first.

This is one of the biggest reasons companies prefer cursor pagination.

### Let's use a physical book analogy.

**Offset Pagination** = "Give me items at position 4, 5, 6"

Initially:

**Position:** 1   2   3   4   5   6   7  
**Posts:**   100 99  98  97  96  95  94

**Page 1 (limit=3) shows:**

100 99 98

Now a new post 101 is inserted at the top:

**Position:** 1   2   3   4   5   6   7   8  
**Posts:**   101 100 99  98  97  96  95  94

For page 2, offset pagination says:

Skip the first 3 positions and give me the next 3.

**Skip:**

101 100 99

**Return:**

98 97 96

But 98 was already shown on page 1.

**Cursor Pagination** = "Give me posts after 98"

**Page 1 showed:**

100 99 98

The system remembers:

last_seen_post = 98

Now post 101 is inserted:

101 100 99 98 97 96 95 94

For page 2, cursor pagination says:

I already saw 98. Give me records after 98.

So it returns:

97 96 95

No duplicate.

Examples:

social media feeds
product listings
search results
chat history
logs/events

What is Pagination?

Instead of returning all records:

Posts:
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, ...

Return only a small page:

Page 1: 1, 2, 3
Page 2: 4, 5, 6
Page 3: 7, 8, 9

Benefits:

Faster responses
Lower memory usage
Less network traffic
Better user experience

Why Pagination Matters

Without pagination:

SELECT * FROM posts;

Problems:

huge memory usage
slow queries
high DB load
network overhead
poor UX

Two Main Pagination Approaches
1. Offset Pagination(Easy but doesn't scale well)
Offset pagination retrieves records by skipping a certain number of rows and then returning the next set of rows.
API:

GET /posts?offset=20&limit=10

SQL:

SELECT *
FROM posts
ORDER BY created_at DESC
LIMIT 10 OFFSET 20;

Meaning:

Skip first 20 rows
Return next 10 rows

Example

Data:

1 2 3 4 5 6 7 8 9 10

Request:

offset = 3
limit = 2

Result:

4 5


Pros
Very simple
Easy to implement
Easy to jump to page 50
Cons

Imagine 10 million rows:

OFFSET 9000000

Database still scans through millions of rows before returning data.

Performance becomes:

Page 1  -> Fast
Page 1000 -> Slow
Page 100000 -> Very Slow

Offset Pagination

Suppose:

SELECT * FROM posts
ORDER BY id
LIMIT 10 OFFSET 100000;

The database typically has to:

Find the beginning of the ordered result set.
Walk through (or count through) the first 100,000 rows.
Discard them.
Return the next 10 rows.

Conceptually:

1 -> 2 -> 3 -> ... -> 100000 -> 100001
                          ^
                       start here

So the larger the offset, the more work the database does.



2. Cursor Pagination (Preferred in large systems)
Cursor pagination retrieves records starting from a specific reference point (cursor) instead of skipping rows.
Instead of page number:

GET /posts?cursor=12345&limit=10

Meaning:

Give me 10 posts after ID 12345

SQL:

SELECT *
FROM posts
WHERE id < 12345
ORDER BY id DESC
LIMIT 10;
Why Faster?

Database uses index directly:

Jump to row 12345
Read next 10 rows
Done

No scanning millions of rows.

Cursor Pagination

Suppose:

SELECT *
FROM posts
WHERE id > 100000
ORDER BY id
LIMIT 10;

If id is indexed, the database can use the index to jump directly near id = 100000.

Conceptually:

Index
 |
 v
100000 -> 100001 -> 100002 -> ...

Then it simply reads the next 10 records.

No need to count and discard 100,000 rows first.

This is one of the biggest reasons companies prefer cursor pagination.
Let's use a physical book analogy.

Offset Pagination = "Give me items at position 4, 5, 6"

Initially:

Position: 1   2   3   4   5   6   7
Posts:   100 99  98  97  96  95  94

Page 1 (limit=3) shows:

100 99 98

Now a new post 101 is inserted at the top:

Position: 1   2   3   4   5   6   7   8
Posts:   101 100 99  98  97  96  95  94

For page 2, offset pagination says:

Skip the first 3 positions and give me the next 3.

Skip:

101 100 99

Return:

98 97 96

But 98 was already shown on page 1.

That's the duplicate.

Cursor Pagination = "Give me posts after 98"

Page 1 showed:

100 99 98

The system remembers:

last_seen_post = 98

Now post 101 is inserted:

101 100 99 98 97 96 95 94

For page 2, cursor pagination says:

I already saw 98. Give me records after 98.

So it returns:

97 96 95

No duplicate.

