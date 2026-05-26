# Embeddings — Simple Explanation

Embeddings convert text into numbers (vectors) so machines can understand similarity and meaning.

Example:

```text
"cat" → [0.12, -0.55, 0.91 ...]
"dog" → [0.10, -0.50, 0.88 ...]
```

Since cat and dog are similar, their vectors are close.

## Types of Embeddings:

### 1. Dense / Semantic Embeddings

“Convert sentences into numbers such that similar meanings are close together.”

### Example:
```
Take these two sentences:

1. "How long does a token last?"
2. "What is the expiry time of a token?"

They use different words, but mean the same thing.

A semantic embedding model will convert them into vectors like:

Sentence 1 → [0.12, 0.88, -0.33, ...]
Sentence 2 → [0.11, 0.86, -0.30, ...]

These vectors are very close to each other.
```
### Used For
- RAG chatbots
- semantic search
- recommendation systems
- document retrieval

### Pros

- ✅ Understands synonyms
- ✅ Natural language friendly
- ✅ Good semantic matching

### Cons

- ❌ Weak exact keyword matching
- ❌ Can miss IDs/codes/errors
---

### 2. Sparse Embeddings (Keyword-Based)

Sparse embeddings represent text using exact words and their importance, not meaning.

### They focus on:

keyword matching
,word frequency
,exact terms


### Example

Document:

OAuth token expires in 24 hours

Sparse vector stores:

```text
OAuth → 1
token → 1
expires → 1
24 → 1
```

Everything else is zero →

```text
[0,0,1,0,0,1,0,0...]
```

Hence called sparse.

### BM25

Most popular sparse retrieval algorithm.

### Used in:

- search engines
- Elasticsearch
- Solr

BM25 ranks based on:

- keyword frequency
- rarity
- document length

### Pros

- ✅ Excellent exact match
- ✅ Great for technical terms
- ✅ Fast and cheap

### Cons

- ❌ Doesn't understand meaning
- ❌ Synonyms fail

---

### 3. Hybrid Search

Combines:

- dense semantic retrieval
- sparse keyword retrieval

Best production approach.

### Why Hybrid?

```
User may ask:

"Why is ERR_AUTH_401 happening?"

Semantic search:

- may understand authentication issue

BM25:

- find s exact error code

Together:

- ✅ better accuracy
```
---

### Question : I want to make a query to get exact number of data from pdf , which embedding needs to be used?

### Example query:
```
"What is the maximum retry timeout?"

and PDF contains:

Maximum retry timeout = 120 seconds

You need more precise retrieval.
```

### Problem with Pure Semantic Embeddings

Dense embeddings are good at meaning, but numbers can get weakly represented.

Example:
```
120
102
210
```
Vectors may become similar.

So semantic search may retrieve wrong chunks.

---

### Better Approaches
### 1. Hybrid Search (Recommended)

Combine:

- semantic embeddings
- (sparsh embeddings) - BM25 keyword search Why?

BM25 preserves exact tokens:
```
120
retry timeout
ERR_401
v2.3.1
```

### 2. Metadata Filtering

Store structured metadata during ingestion.

Instead of only:

```json
{
  "text": "Maximum retry timeout = 120 seconds"
}
```

Store:

```json
{
  "key": "retry_timeout",
  "value": 120,
  "page": 45
}
```

Then exact retrieval becomes easier.

## Production Architecture

```text
               User Query
                    |
       +------------+------------+
       |                         |
       v                         v
Semantic Search            BM25 Search
(Dense embeddings)        (Exact keywords)
       |                         |
       +------------+------------+
                    |
                Reranker
                    |
               Best Chunks
                    |
                   LLM
```
---

### Question : If we store apple in embedding and orange in embedding , will they store same lenght embedding?

All words/sentences stored using the same embedding model will have the same vector length (dimensions).

Example using an embedding model:

```text
"apple"  -> [0.12, -0.44, 0.91, ...]
"orange" -> [0.33,  0.11, 0.72, ...]
```

Both vectors may have:

1536 dimensions

---

## Common Chunking Methods
### 1. Fixed-Size Chunking

Most basic.

Split every:
```
N tokens
```

Example:
```
500-token chunks
```
---

### 2. Recursive Chunking (Very Popular)

Used by:
```
- LangChain
- LlamaIndex
- Idea
```

Split gradually:
```
- Heading
- ↓
- Paragraph
- ↓
- Sentence
- ↓
- Words
```
Until chunk size fits limit.

---

### 3. Sentence-Based Chunking
```
Split by sentences.
```
Example:
```
- Sentence 1
- Sentence 2
- Sentence 3
```
Combine until token limit reached.

---

### 4. Semantic Chunking (Advanced)

Split based on:
```
- meaning/topic change
```

Example:
```
- Paragraph changes from:

- OAuth

- to:

- Database replication
```
Semantic model detects topic shift.

---

### 5. Heading/Subheading Chunking

Very common for:
```
- wiki
- documentation
- PDFs
```
Example:
```
- Authentication
-   OAuth
-   JWT

- Database
-   Replication
```

Chunks follow structure.

---

### 6. Sliding Window / Overlap Chunking
```
Adds overlap between chunks.
```
Example:
```
Chunk1 = tokens 1–500
Chunk2 = tokens 450–950
```
Overlap:
```
50 tokens
```

---

## 7. AST-Based Chunking (Code)

For source code.

Uses:
```
- parser
- syntax tree
```

Chunks:
```
- function
- class
- logical blocks
```

---

### Question : suppose some subheading is say 3-4 pages then wont be be too long chunk?

That’s why we do hierarchical chunking.

If a subheading spans 3–4 pages, creating one giant embedding chunk is bad because:
```
- embedding becomes diluted
- retrieval accuracy drops
- too much irrelevant context
- higher token cost
```

Suppose one large function is split into multiple chunks.
```
Then:

- metadata can remain same/shared
- chunk text differs
- each chunk gets its own embedding
```

### Example Problem

Suppose:
```
- Heading: Authentication
- Subheading: OAuth
```

and OAuth section is:
```
- 4 pages long

- Creating ONE embedding for all 4 pages is inefficient.
```

### Correct Approach

### Step 1 — Preserve Structure

Keep metadata:

```json
{
  "heading": "Authentication",
  "subheading": "OAuth"
}
```
### Step 2 — Split Into Smaller Chunks

### Example:
```
OAuth section
   ↓

Chunk 1 → token creation
Chunk 2 → expiry
Chunk 3 → refresh token
Chunk 4 → revocation
```

Each:
```
~500 tokens
```

Final Stored Chunks
```json
{
  "heading": "Authentication",
  "subheading": "OAuth",
  "chunk_id": 1,
  "text": "OAuth access token creation..."
}
{
  "heading": "Authentication",
  "subheading": "OAuth",
  "chunk_id": 2,
  "text": "Token expiry duration..."
}
{
  "heading": "Authentication",
  "subheading": "OAuth",
  "chunk_id": 3,
  "text": "And token ..."
}
```

---

### Queation : same embedding is used to create vector db embedding and input converting embedding?

Yes — in a correct RAG system, the same embedding model must be used for both.

### Key Rule

The embedding model used to build the vector database must be the same one used for query embedding

### Why?

Because vector search is basically:
```
compare(query_vector, document_vector)

If they come from different embedding models, they are in different vector spaces, so comparison becomes meaningless.
```

---
### Question : If your RAG source is code instead of PDF, which chunking and embedding is used?

If your RAG source is code instead of PDF, the pipeline is similar — but chunking is structure-based, not paragraph-based.

### Best chunking units for code

### 1. Function-level chunking (most common)

```python
def process_payment():
    ...
```

👉 1 function = 1 chunk

### 2. Class-level chunking

```python
class PaymentService:
    def pay():
        ...
    def refund():
        ...
```

👉 class or methods inside class = chunks

### Example

Code:

```python
def order():
    validate()
    process_payment()
    retry_payment()
    send_email()
```

Chunks:
- Chunk 1 → validate logic
- Chunk 2 → payment logic
- Chunk 3 → retry logic
- Chunk 4 → email logic

### Which embedding model to use for code?

Option 1 (simple, production-ready):

- `text-embedding-3-small`
- or `text-embedding-3-large`

👉 works for both code + comments + queries

### Metadata is very important

Each chunk stores:

```json
{
  "file": "payment.py",
  "function": "retry_payment",
  "class": "PaymentService"
}
```

This helps:

- grouping results
- tracing origin
- improving LLM context

---

### Question : If we have a function which is huge then won't that chunk be too large ?

### Example

Big function:

```python
def process_order():
    validate_user()
    check_stock()
    make_payment()
    retry_payment()
    send_email()
```

Chunks created:

- Chunk 1 → validation
- Chunk 2 → stock check
- Chunk 3 → payment
- Chunk 4 → retry logic
- Chunk 5 → email

Each chunk:

- small enough
- meaningful
- independent


### What if still too large?

Large function split into chunks:

Chunk 1

```json
{
  "file": "order.py",
  "function": "process_order",
  "chunk_id": 1
}
```

Chunk 2

```json
{
  "file": "order.py",
  "function": "process_order",
  "chunk_id": 2
}
```