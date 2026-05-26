# Requirements

## Functional
- User asks questions in natural language
- Bot answers from PDF/wiki content
- Weekly document updates
- Source citation/page references
- Fast response (< 3 sec)

## Non-Functional
- Scalable
- Low hallucination
- Incremental indexing
- Versioning support

# High-Level Architecture

```text
+------------------+
|   User / UI      |
+--------+---------+
         |
         v
+------------------+
|  API Gateway     |
+--------+---------+
         |
         v
+------------------+
| Orchestrator     |
| (RAG Pipeline)   |
+---+----------+---+
    |             |
    |             |
    v             v
    Retrieve        Generate
+----------------+   +----------------+
| Vector DB      |   | LLM Service    |
| Pinecone/FAISS |   | GPT/Llama      |
+--------+-------+   +----------------+
        ^
        |
  +---------+-----------+
  | Embedding Pipeline  |
  +---------+-----------+
        ^
        |
  +---------------------+
  | PDF Parser + Chunker|
  +---------------------+
        ^
        |
  +---------------------+
  | Weekly PDF Upload   |
  +---------------------+
```

# Core Design: RAG (Retrieval-Augmented Generation)

Instead of fine-tuning:

- Parse PDF
- Split into chunks
- Create embeddings
- Store in vector DB
- Retrieve relevant chunks during query
- Send retrieved context to LLM

# Document Ingestion Pipeline

## Step 1: PDF Parsing

### Tools:
- PyMuPDF
- PDFPlumber
- Apache Tika

### Extract:
- text
- headings
- tables
- page number
- metadata

### Example chunk:

```json
{
  "page": 45,
  "section": "Authentication",
  "content": "OAuth token expires in 24 hours..."
}
```

## Step 2: Chunking Strategy

### Best practice:
- Semantic Chunking
- 300–500 tokens
- 50 token overlap

### Why overlap?
- preserves context

```
Example:
- Chunk 1: pages 1-2
- Chunk 2: pages 2-3
```

## Step 3: Embeddings

### Embedding models:
- OpenAI text-embedding-3-large
- BGE-large
- E5-large

```
Output:
- "OAuth token expires..." -> vector[1536]
```

## Step 4: Store in Vector DB

### Options:
- Scale	DB
- Small	FAISS
- Medium	Chroma
- Large/Prod	Pinecone / Weaviate / Milvus

### Store:

```json
{
  vector,
  chunk_text,
  page_no,
  version,
  document_id
}
```

# Query Flow

User asks:
- How long does OAuth token last?

## Retrieval

Embed query:

```python
query_vector = embedding(question)
```
```
Similarity search:
- top_k = 5 chunks
```
```
Retrieved chunks:
- Chunk A -> page 45
- Chunk B -> page 46
```

## Prompt Construction
```
- Answer ONLY from context.

Context:
- [Chunk A]
- [Chunk B]

Question:
- How long does OAuth token last?
```

## LLM Response
```
- OAuth token expires in 24 hours (Page 45).
```

# Weekly Updates Strategy

This is the important part.

## Option 1 — Full Reindex (Simple)
```
Every week:
- delete old vectors
- parse full PDF
- regenerate embeddings

Good if:
- only 1000 pages
- updates are infrequent

Cost:
- acceptable for medium scale
```

## Option 2 — Incremental Updates (Recommended)
```
Pipeline:
- Compare new PDF vs old PDF
- Detect changed pages/chunks
- Re-embed only changed chunks
- Update vector DB

Use:
- hash comparison
- page checksum
```
Example:

```python
page_hash = SHA256(page_text)
```

If unchanged:
- skip embedding

Huge cost savings.

# Preventing Hallucination

A hallucination happens when an AI gives information that sounds confident and correct but is actually false or made up.

## Techniques
```
1. Strict Prompting
   - Answer only from provided context.
   - If not found, say "Information not available."

2. Retrieval Threshold
   If similarity < threshold:
   - "I couldn't find this in the wiki."

3. Source Citation
   Return:
   - page number
   - section
   - paragraph
```

# Scaling Considerations

## For Large Usage

### Cache Layer
```
Redis:
- cache embeddings
- cache responses
```
### Async Ingestion
```
Kafka/SQS:
- process uploads asynchronously
```
### Horizontal Scaling
```
Separate services:
- ingestion
- retrieval
- inference
```

### Typical RAG Flow

```
PDF
 ↓
Chunking
 ↓
Embedding model
 ↓
Vector DB

User Question
 ↓
Embedding model
 ↓
Similarity Search
 ↓
Top K chunks
 ↓
LLM
```