# Problem Statement

Design a notification service that can send notifications through multiple channels:

- Push Notifications (Android/iOS)
- Email
- SMS
- In-App Notifications
- WhatsApp (optional)

Examples:

- Order shipped
- Payment successful
- Password reset
- Promotional campaigns
- Social media alerts

# Functional Requirements

- Send notifications to users.
- Support multiple channels.
- Support immediate and scheduled notifications.
- Users can configure notification preferences.
- Retry failed notifications.
- Track notification status.

# Non-Functional Requirements

- Highly scalable.
- Reliable delivery.
- Low latency.
- Fault tolerant.
- Extensible for new channels.
- Eventually consistent.

# Capacity Estimation

Assume:

- 50 million users
- 10 notifications/user/day

Total notifications/day:

```
50M × 10
= 500M notifications/day
```

Per second:

```
per second , 24 h * 60 min * 60 sec = 86400
500M / 86400
≈ 5800 notifications/sec
```

Peak traffic (10x):

- ≈ 60K notifications/sec

Design should comfortably support:

- 50K–100K notifications/sec

# High-Level Architecture

***The Client Service can be any business service from applications such as WhatsApp, Instagram, PhonePe, or Flipkart that generates a notification event. The Notification Service receives these events and delivers them to end users through different notification channels such as Email, SMS, Push Notifications, or In-App Notifications, depending on the notification type and user preferences.***
```text
                    +----------------+
                    | Client Service |
                    +--------+-------+
                             |
                             v
                 +----------------------+
                 | Notification API     |
                 +----------+-----------+
                            |
                            v
                 +----------------------+
                 | Notification Service |
                 +----------+-----------+
                            |
            +---------------+----------------+
            |                                |
            v                                v
 +----------------------+       +----------------------+
 | User Preference      |       | Template Service     |
 | Service              |       |                      |
 +----------+-----------+       +----------+-----------+
            |                              |
            v                              v
   +----------------+             +----------------+
   | Preference DB  |             | Templates DB   |
   +----------------+             +----------------+

                            |
                    Publish Notification
                            |
                            v
                    +---------------+
                    | Message Queue |
                    +-------+-------+
                            |
        -----------------------------------------
        |                |              |        |
        v                v              v        v

 +------------+   +------------+  +---------+ +---------+
 | Email      |   | SMS        |  | Push    | | In-App  |
 | Worker     |   | Worker     |  | Worker  | | Worker  |
 +------+-----+   +------+-----+  +----+----+ +----+----+
        |                |             |           |
        v                v             v           v

 Email Provider   SMS Gateway    APNS/FCM   Notification DB
        |                |             |           |
        +----------------+-------------+-----------+
                             |
                             v
                          End User
```

Here, "Client Service" usually does not mean the end-user client (mobile app, web app, etc.). It typically means another backend service that wants to send a notification.

For example:

- Order Service
- User Service
- Payment Service
- Booking Service

Any of these services can act as the Client Service and call the Notification API.

The workers read messages from the queue, and the workers send notifications to end users by calling external providers (email provider, SMS gateway, APNS/FCM, etc.).

#### Real Example: Order Shipped

Suppose the Notification Service decides:
```
Send EMAIL
Send PUSH
Create IN-APP notification
```
It publishes 3 messages:

Queue
```
Message 1 -> EMAIL
Message 2 -> PUSH
Message 3 -> IN_APP
```
Workers process them independently:
```
Email Worker
   |
   v
Send Email

Push Worker
   |
   v
Send Push

In-App Worker
   |
   v
Insert DB Record
```
"Send Push" means sending a push notification to a user's device (mobile app, tablet, sometimes desktop browser).

**For example, when an order is shipped, the Notification Service decides:**

User Preferences:
```
✓ Email
✓ Push
✗ SMS
```
Then it creates two independent tasks:
```
Order Shipped
      |
      +--> Email Queue --> Email Worker --> Email
      |
      +--> Push Queue  --> Push Worker  --> Push Notification
```
The push notification is not notifying the user that an email was sent.

Instead, both channels are delivering the same business event:

**Event: Order Shipped**
```
Email:
"Your order #123 has been shipped."

Push:
"Your order has been shipped."
```

## User Preference Service

Stores:

```json
{
  "email": true,
  "sms": false,
  "push": true
}
```

Notification Service checks this before publishing messages.

## Template Service

Instead of sending full content:

```json
{
  "template":"ORDER_SHIPPED"
}
```

Store template:

```text
Hello {{name}}
Your order {{orderId}}
has been shipped.
```

## Notification DB

Tracks status:

```json
{
  "notificationId": "123",
  "channel": "EMAIL",
  "status": "DELIVERED"
}
```


# Why separate workers?

Suppose 1 million emails need to be sent.

If Notification Service sends emails directly:

```text
Notification Service
    |
    +--> Send Email
    +--> Send Email
    +--> Send Email
```

it becomes slow and can fail under load.

Instead:

```text
Notification Service
      |
      v
   Queue
      |
      +--> 10 Email Workers
      +--> 20 SMS Workers
      +--> 15 Push Workers
```

Workers can be scaled independently.

# API Design

## Send Notification

`POST /notifications`

Request:

```json
{
  "userId": "123",
  "templateId": "ORDER_SHIPPED",
  "channels": ["EMAIL", "PUSH"],
  "data": {
    "orderId": "A123"
  }
}
```

Response:

```json
{
  "notificationId": "N567",
  "status": "QUEUED"
}
```

## Get Status

`GET /notifications/{id}`

Response:

```json
{
  "notificationId": "N567",
  "status": "DELIVERED"
}
```

## User Preference Service

Users may choose:

```json
{
  "email": true,
  "sms": false,
  "push": true
}
```

Before sending:

- Check preference
- Allowed?
- Send

This prevents unwanted notifications.

### There are two different flows happening:

### Flow 1: User sets preferences

The end user chooses notification settings in the app:

```json
{
  "email": true,
  "sms": false,
  "push": true
}
```

This is typically saved in a User Preference Service or database.

```text
User
  |
  v
Mobile/Web App
  |
  v
User Preference Service
  |
  v
Database
```

The preferences are stored centrally.

### Flow 2: Another service wants to send a notification

Later, suppose an order is shipped.

```text
Order Service
   |
   v
Notification Service
```

The Order Service usually does not send the user's preferences.

Instead it sends something like:

```json
{
  "userId": 123,
  "event": "ORDER_SHIPPED"
}
```

Then the Notification Service looks up the user's preferences:

```text
Notification Service
        |
        +--> User Preference Service
                |
                v
         email=true
         sms=false
         push=true
```

Based on that:

- Send Email  ✓
- Send Push   ✓
- Send SMS    ✗



# Step-by-step flow: Complete Scenario

## Scenario

User preferences:

```json
{
  "email": true,
  "sms": false,
  "push": true
}
```

Event:

```text
Order #12345 has been shipped
```

## Step 1: Business Service Creates Event

The Order Service detects that an order has been shipped.

```text
Order Service
      |
      v
Notification API
```

Request:

```json
{
  "userId": 101,
  "eventType": "ORDER_SHIPPED",
  "orderId": "12345"
}
```

At this point, the Order Service does not care whether the notification will be email, SMS, or push.

Its only job is:

> "Tell the Notification System that Order #12345 was shipped."

## Step 2: Notification API Receives Request

The Notification API acts as the entry point.

It may:

- Authenticate the caller
- Validate request format
- Check required fields

Example validation:

```json
{
  "userId": 101,
  "eventType": "ORDER_SHIPPED"
}
```

If valid:

```text
Notification API
      |
      v
Notification Service
```

## Step 3: Notification Service Starts Processing

Now the Notification Service becomes the orchestrator.
- An Orchestrator is a component that coordinates and controls a workflow involving multiple services or steps.

- Think of it as a conductor in an orchestra. The conductor doesn't play the instruments; it tells each musician when and how to play. Similarly, an orchestrator doesn't perform all tasks itself—it tells different services what to do and in what order.
- An orchestrator is a service that coordinates a multi-step workflow. Instead of each service calling other services directly, the orchestrator controls the execution order, handles retries and failures, tracks workflow state, and decides the next action. In a notification system, the orchestrator can fetch user preferences, determine delivery channels, render templates, and dispatch notifications to channel-specific workers.
It receives:

```json
{
  "userId": 101,
  "eventType": "ORDER_SHIPPED",
  "orderId": "12345"
}
```

## Step 4: Fetch User Preferences

Notification Service asks:

```text
Notification Service
        |
        v
User Preference Service
```

Query:

```text
Get preferences for User 101
```

Response:

```json
{
  "email": true,
  "sms": false,
  "push": true
}
```

Meaning:

- Email → Allowed
- SMS → Blocked
- Push → Allowed

## Step 5: Fetch Notification Template

Notification Service now needs the actual message.

It asks:

```text
Notification Service
        |
        v
Template Service
```

Request:

```json
{
  "eventType": "ORDER_SHIPPED"
}
```

Template Service returns:

```text
Subject:
Your order has shipped

Body:
Hello {name},
Your order #{orderId} has been shipped.
```

## Step 6: Personalize Template

Notification Service fills placeholders.

Result:

```text
Subject:
Your order has shipped

Body:
Hello John,
Your order #12345 has been shipped.
```

Now the message is ready.

## Step 7: Create Channel-Specific Messages

Based on preferences:

```json
{
  "email": true,
  "sms": false,
  "push": true
}
```

Notification Service creates:

**Email Message**

```json
{
  "channel": "EMAIL",
  "userId": 101,
  "subject": "Your order has shipped",
  "body": "Hello John..."
}
```

**Push Message**

```json
{
  "channel": "PUSH",
  "userId": 101,
  "title": "Order Shipped",
  "body": "Order #12345 shipped"
}
```

**SMS**

Not created because:

```json
{
  "sms": false
}
```

## Step 8: Publish to Queue

Notification Service publishes messages.

```text
Notification Service
         |
         v
      Queue
```

Queue now contains:

- EMAIL Message
- PUSH Message
- No SMS message exists

## Step 9: Return Response

The Notification API responds immediately after sending to the queue.

```json
{
  "status": "QUEUED",
  "notificationId": "N1001"
}
```

**Important:**

- The email is not sent yet.
- Only queued.
- This keeps the API fast.

## Step 10: Email Worker Consumes Message

Email Worker continuously listens.

```text
Queue
   |
   v
Email Worker
```

It reads:

```json
{
  "channel": "EMAIL",
  "userId": 101
}
```

## Step 11: Optional Recheck Preferences

Large systems often verify again.

```text
Email Worker
      |
      v
Preference Service
```

Suppose the user disabled email a few seconds ago.

Then:

```json
{
  "email": false
}
```

Worker drops the message.

Otherwise continue.

## Step 12: Email Worker Calls Provider

```text
Email Worker
      |
      v
Email Provider
```

Examples:

- SendGrid
- Amazon SES
- Mailgun

Request:

```json
{
  "to": "john@gmail.com",
  "subject": "Your order has shipped",
  "body": "Hello John..."
}
```

Provider sends email.

## Step 13: Push Worker Processes Push Message

Simultaneously:

```text
Queue
   |
   v
Push Worker
```

Push Worker consumes:

```json
{
  "channel": "PUSH"
}
```

Calls:

- FCM (Android)
- or APNS (iPhone)

Push notification appears on the phone.

## Step 14: Update Notification Status

Workers update Notification DB.

```text
Email Worker
      |
      v
Notification DB
```

Record:

```json
{
  "notificationId": "N1001",
  "channel": "EMAIL",
  "status": "DELIVERED"
}
```

Similarly:

```json
{
  "notificationId": "N1002",
  "channel": "PUSH",
  "status": "DELIVERED"
}
```

Similarly:

{
  "notificationId": "N1002",
  "channel": "PUSH",
  "status": "DELIVERED"
}


# Why Use Message Queue?

## Without a Queue

```text
Order Service
      |
      v
Notification API
      |
      v
Email Provider
```

Suppose the Email Provider takes 5 seconds to respond.

The flow becomes:

```text
Order Service
    waits 5 seconds
         |
         v
Email Provider
```

### 1. Slow API

User places an order.

```text
Place Order
    |
    v
Send Email
    |
    v
Wait 5 seconds
```

The API response is delayed because it's waiting for the email provider.

### 2. Provider Outage

Suppose SendGrid/SES is down.

```text
Notification API
       |
       v
Email Provider (DOWN)
```

Now notification requests fail immediately.

### 3. Traffic Spike

Imagine:

Black Friday Sale

1 million orders arrive in 5 minutes.

Without a queue:

```text
1,000,000 requests
        |
        v
Email Provider
```

The provider may throttle or reject requests.

## With a Queue

```text
Order Service
      |
      v
Notification API
      |
      v
Message Queue
      |
      v
Email Worker
      |
      v
Email Provider
```

Now the API only needs to do:

- Receive Request
- Publish Message
- Return Success

Usually in milliseconds.

## Benefit 1: Decoupling

Without queue:

```text
Notification API
      |
      v
Email Provider
```

Strong dependency.

If provider is slow:

- API becomes slow

With queue:

```text
Notification API
      |
      v
Queue
```

API doesn't care when the email is actually sent.

The Email Worker handles that later.

So:

- Producer = Notification Service
- Consumer = Email Worker

They are independent.

## Benefit 2: Buffer Traffic Spikes

Imagine:

- 100 notifications/sec normally
- Suddenly: 10,000 notifications/sec

Queue absorbs the burst.

```text
10,000 Messages
      |
      v
Queue
```

Workers process gradually:

```text
Queue
  |
  +--> Worker 1
  +--> Worker 2
  +--> Worker 3
```

No notifications are lost.

Think of the queue as a waiting line.

## Benefit 3: Retry Support

Suppose Email Provider fails temporarily.

```text
Email Worker
      |
      v
Provider Timeout
```

Worker can do:

- Retry #1
- Retry #2
- Retry #3

Or move the message to:

- Dead Letter Queue (DLQ)

Example:

```text
Main Queue
     |
     v
Worker
     |
     X Failed
     |
     v
Retry Queue
```

Without a queue, the request simply fails.

## Benefit 4: Scalability

Suppose email volume grows.

Initially:

```text
Queue
  |
  v
Email Worker
```

One worker handles:

- 100 msgs/sec

Traffic increases.

Just add workers:

```text
Queue
  |
  +--> Email Worker 1
  +--> Email Worker 2
  +--> Email Worker 3
  +--> Email Worker 4
```

Now throughput increases.

No API changes needed.

## When would you choose which queue?

### RabbitMQ

Good for:

- Task processing
- Notification systems
- Reliable message delivery
- Complex routing

```text
Notification Service
      |
      v
RabbitMQ
      |
      v
Email Worker
```

Very common for notification systems.

### Apache Kafka

Good for:

- Massive scale
- Event streaming
- Millions of events/sec
- Event retention and replay

- Order Events
- Payment Events
- Notification Events

Many large companies use Kafka as the backbone for event-driven architectures.

### Amazon SQS

Good for:

- AWS-based systems
- Managed service
- No server management

```text
Notification Service
      |
      v
SQS
      |
      v
Workers
```

Popular when your infrastructure is already on AWS.

Channel Workers

Each channel gets dedicated workers.
```
Queue
 |
 +---- Email Worker
 |
 +---- SMS Worker
 |
 +---- Push Worker
 |
 +---- In-App Worker
```

Advantages:

Independent scaling
Independent deployment

Example:

Push traffic spike

Scale:

Push Workers = 50
Email Workers = 10

## Database Design

In a real notification system you almost always need multiple databases, not just the queue. The queue is for temporary message transport. Databases are for persistent storage.

### 1. User Preference Database (Required)

Stores:

```json
{
  "userId": 101,
  "email": true,
  "sms": false,
  "push": true
}
```

Why?

When Notification Service receives:

```json
{
  "userId": 101,
  "eventType": "ORDER_SHIPPED"
}
```

it must know:

- Can I send Email?
- Can I send SMS?
- Can I send Push?

Without a DB, preferences would be lost after a restart.

Good choice:

Relational DB:

- PostgreSQL
- MySQL

Because preferences are structured and transactional.

### 2. Notification Database (Strongly Recommended)

Stores notification history.

Example:

```json
{
  "notificationId": "N1001",
  "userId": 101,
  "channel": "EMAIL",
  "status": "DELIVERED",
  "createdAt": "2026-06-07T10:00:00Z"
}
```

Why?

Users often want:

- Show my notifications

Support teams want:

- Was the email sent?
- Did it fail?

Analytics teams want:

- How many notifications were delivered today?

### 3. Template Database (Usually Needed)

Stores templates.

Example (ORDER_SHIPPED_EMAIL):

```text
Subject:
Your order has shipped

Body:
Hello {name},
Your order #{orderId} has been shipped.
```

Why?

Without it, every template must be hardcoded.

Marketing teams often need to update templates without deployments.

### 4. User Device Token Database (For Push)

Push notifications need device tokens.

Example:

```json
{
  "userId": 101,
  "deviceToken": "abc123xyz"
}
```

The Push Worker needs this token to call FCM/APNS.

---

## Why Start with PostgreSQL?

In the beginning, most notification systems store everything in PostgreSQL because:

- Simpler architecture
- Strong consistency
- ACID transactions
- Rich querying support
- Easier operations

Let's look at each table.

### 1. User Preferences

Example:

**UserPreferences**

- user_id
- email_enabled
- sms_enabled
- push_enabled
- updated_at

When a user disables email notifications:

- User clicks: "Disable Email Notifications"
```
Update Preference
        ↓
Next Notification
        ↓
Must NOT send Email
```
If stale data is read:
```
User disabled Email
       ↓
System still sends Email
```
This is a bad user experience and may violate regulations.

Therefore:

**Strong Consistency Required**

PostgreSQL is ideal.

### 2. Templates

Example:

**Templates**

- template_id
- channel
- content
- version

Templates change infrequently.

Example:

```text
Hello {{name}}
Your order {{orderId}} has shipped.
```

Requirements:

- Transactional updates
- Versioning
- Admin management

Volume is tiny.

Maybe:

- 100 templates
- 1,000 templates

Not billions.

PostgreSQL handles this easily.

### 3. Notification Metadata

Example:

**Notification**

- id
- user_id
- status
- channel
- created_at

You may need queries like:

```sql
SELECT *
FROM Notification
WHERE user_id = ?
ORDER BY created_at DESC
LIMIT 20;
```

PostgreSQL excels at:

- indexing
- joins
- filtering
- ordering

### When Does PostgreSQL Become a Problem?

Consider:

- 500 million notifications/day

Per year:

- 500M × 365 = 182.5 Billion notifications

Now imagine storing every notification record.

Table size:

- Hundreds of billions of rows

Problems appear.

#### Problem 1: Storage Growth

Notification history keeps growing.

- Day 1: 10 GB
- Month 1: 300 GB
- Year 1: Several TB

Large indexes become expensive.

#### Problem 2: Write Throughput

Every notification generates:

- QUEUED
- SENT
- DELIVERED
- FAILED

Status updates.

Example:

- 100K notifications/sec

Potentially:

- 300K–400K writes/sec

A single PostgreSQL cluster may struggle.

#### Problem 3: Horizontal Scaling

PostgreSQL scales primarily by:

- Vertical Scaling

Meaning:

- More CPU
- More RAM
- Bigger Machine

Eventually you hit limits.

### Why Cassandra?

Cassandra is designed for:

- Huge write volume

Characteristics:

- Distributed
- Partitioned
- Multi-node
- Write optimized

Example:

- Node 1
- Node 2
- Node 3
- Node 4
- ...

Adding capacity:

- Add more nodes
- No major redesign.

### Cassandra Write Path

Write: Notification History

Cassandra performs:

- Append
- Memtable
- SSTable

Very efficient.

This is why:

- logs
- events
- notification history

fit Cassandra well.

### Which Data Moves to NoSQL?

Usually only notification history.

Keep in PostgreSQL:

- User Preferences
- Templates
- Campaign Configurations
- Notification Rules

Move to Cassandra/DynamoDB:

- Historical Notifications
- Delivery Logs
- Event Streams
- Audit Records

I would initially use PostgreSQL because user preferences, templates, and active notification metadata require ACID transactions, strong consistency, and flexible querying. These datasets are relatively small and highly relational. However, notification history grows rapidly and becomes a write-heavy, append-only workload. At very large scale, storing billions of notification records in PostgreSQL can create storage, indexing, and write-throughput bottlenecks. Therefore, I would move historical notification data to a horizontally scalable NoSQL database such as Cassandra or DynamoDB, which are optimized for high write throughput and large-scale storage while keeping critical transactional data in PostgreSQL.

## Retry Mechanism

Failures happen due to:

- Network issues
- Provider downtime
- Rate limits

Retry policy:

- 1 min
- 5 min
- 15 min
- 30 min

Exponential backoff:

2^n

Example:
```
Attempt 1 → Fail
Attempt 2 → 1 min
Attempt 3 → 2 min
Attempt 4 → 4 min
```
## Handling Duplicate Notifications

why duplicates happen first.

### Scenario: Worker Crashes

Suppose the queue contains:

```json
{
  "notificationId": "N1001",
  "userId": 101,
  "channel": "EMAIL"
}
```

Flow:

```text
Queue
  |
  v
Email Worker
```

Step 1

Worker reads message.

```text
Queue
  |
  v
Email Worker

Message is now being processed.
```

Step 2

Worker sends email successfully.

```text
Email Worker
      |
      v
Email Provider
```

User receives:

Your order has shipped

Step 3

Before acknowledging (ACK) the queue:

```text
Email Worker
      X
   CRASH
```

The worker dies.

Queue never receives:

```text
ACK
```

Step 4

Queue thinks:

"Nobody processed this message."

So it redelivers:

```text
Queue
  |
  v
Another Worker
```

Now the second worker processes it again.

Result:

- Email #1 Sent
- Email #2 Sent

User receives duplicate emails.

Why this happens

Most queues guarantee:

**At-Least-Once Delivery**

Meaning:

A message may be delivered more than once, but it won't be silently lost.

This is safer than losing notifications.

Therefore: Duplicates are possible and your application must handle them.

### Solution: Idempotency

Each notification gets a unique ID.

```json
{
  "notificationId": "N1001"
}
```

Before sending:

```sql
SELECT status
FROM notifications
WHERE notification_id = 'N1001';
```

First Processing

**Table:**

```text
notification_id | status
--------------------------
(empty)
```

**Worker checks.**

Not found

So it sends email.

**Then stores:**

```text
notification_id | status
--------------------------
N1001           | DELIVERED
```

Duplicate Processing

Another worker receives same message.

Checks:

```sql
SELECT status
FROM notifications
WHERE notification_id='N1001';
```

**Result:**

DELIVERED

Worker immediately stops. Skip Sending. No duplicate email.

### Better Approach: Unique Constraint

Instead of:

```
SELECT
then
INSERT
```

which can have race conditions,

use a unique key.

Table:

```sql
CREATE TABLE notifications (
    notification_id VARCHAR(50) PRIMARY KEY,
    status VARCHAR(20)
);
```

Worker tries:

```sql
INSERT INTO notifications
(notification_id,status)
VALUES ('N1001','PROCESSING');
```

**Worker 1**

Succeeds.

N1001 inserted

**Worker 2**

Attempts same insert.

**Database says:**

Duplicate Key Error

**Worker knows:**

- Already being processed
- or
- Already processed

and exits.