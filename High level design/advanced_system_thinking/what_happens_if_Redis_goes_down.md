# What is Redis?

Redis is an open-source, in-memory data store used as:

Cache → stores frequently accessed data for faster performance  
Database → can persist data to disk  
Message broker → supports queues and pub/sub systems  
Session store → commonly used for login sessions, tokens, carts, etc.

Because Redis keeps data mainly in RAM (memory), it is extremely fast compared to traditional disk-based databases.

Redis is a key-value store, but the “value” can be advanced structures like lists and sets, allowing applications to use Redis as:

cache,  
queue,  
session store,  
leaderboard,  
and more.

Because of these data structures, Redis can behave like different systems.

## Basic Key-Value Idea

Normally you think:

key → value

Example:

"name" → "Rahul"

Simple.

## But Redis Values Can Be Complex

Example:

"emails_queue" → [job1, job2, job3]

Now the value is a list.

That list can behave like a queue.

## 1. Redis as Cache

Redis and the main database are two different systems.

🧠 How it actually works

When Redis is used as a cache:

Client → Application → Redis (cache)  
                     ↓ miss  
                   Database

So:

Redis = fast temporary storage (cache layer)  
Database (DB) = permanent storage (source of truth)

So Redis becomes a cache simply because:

it is fast  
data is temporary  
data can expire automatically

Example:

SETEX user:101 60 "John"

Meaning:

Store for 60 seconds

That is caching behavior.

## 2. Redis as Queue

Queues need:

insert item  
remove item  
FIFO order

Redis Lists support this naturally.

Internally  
"email_queue" → [email1, email2, email3]

Redis key:

email_queue

Redis value:

a LIST

## 3. Redis as Session Store

Web apps need temporary user data.

Example:

logged-in state  
shopping cart  
OTP verification

Redis stores them using keys.

Example  
SET session:user123 "logged_in"

When user makes request:

GET session:user123

If found:

User is authenticated

## 4. Redis as Rate Limiter

Redis can count requests quickly.

Example

Track API requests:

INCR api:user123

Set expiry:

EXPIRE api:user123 60

Meaning:

Count requests for 1 minute

If count exceeds limit:

Block request

## What Happens If Redis Goes Down?

The impact depends on how Redis is being used in the system.

## 1. If Redis Is Used as a Cache

What Happens

Cached data becomes unavailable  
Application falls back to the main database  
Database load increases heavily  
APIs become slower  

Effects

Increased response time  
Higher DB CPU usage  
Possible database overload  
Temporary performance degradation  

Example

Before:

User Request → Redis Cache → Fast Response

After Redis failure:

User Request → Database → Slower Response  

Usually:

The application still works, but slower.

## 2. If Redis Stores User Sessions

What Happens

User sessions are lost or inaccessible  
Users may get logged out suddenly  
Authentication may fail  

Effects

Forced re-login  
Broken shopping carts  
Interrupted user activity  

Example

Many web applications store:

JWT blacklists  
Session IDs  
OTPs  
Temporary login states  

inside Redis.

If Redis crashes:

Session lookup fails → User appears unauthenticated

## 3. If Redis Is Used for Queues

Redis is often used with systems like:

Celery  
BullMQ  
Sidekiq  

What Happens

Background jobs stop processing  
Tasks may remain stuck  
Some jobs may be lost  

Effects

Emails not sent  
Payments delayed  
Notifications fail  
Data synchronization pauses  

## 4. If Redis Is Used for Rate Limiting

What Happens

The application cannot track request counts properly.

Possible Outcomes

Configuration	Result  
Fail-open	Everyone allowed  
Fail-closed	Everyone blocked  

Risk

API abuse  
DDoS vulnerability  
Legitimate users blocked  

## 5. If Redis Is Used as Primary Database

This is more serious.

What Happens

Application data may become unavailable  
Possible data loss if persistence is weak  
Writes may fail  

Risks

Downtime  
Permanent data loss  
Service outage