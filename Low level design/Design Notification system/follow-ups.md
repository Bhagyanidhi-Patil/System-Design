# Notification System Follow-Up Questions

## 1. How would you retry failed notifications?

Use a retry mechanism with exponential backoff.

For example, if an email fails because the email provider is temporarily unavailable, retry delivery a limited number of times instead of giving up immediately:

| Attempt | Wait before retry |
|--------:|------------------:|
| 1       | 1 second          |
| 2       | 2 seconds         |
| 3       | 4 seconds         |
| 4       | 8 seconds         |

This is exponential backoff. In production, add random jitter to avoid many jobs retrying simultaneously.

## 2. How would you make notification delivery asynchronous?

Use a message queue and background workers.

Sending a notification immediately when a method is called can make the caller wait for delivery. For example, if sending takes three seconds, the caller may have to wait for those three seconds.

Instead, enqueue a notification job and let a background worker send it:

1. An order is placed.
2. The application publishes a notification job to the queue.
3. The application continues without waiting for the email or SMS to be delivered.
4. A background worker consumes the job and sends the notification.
5. On failure, the worker retries according to the retry policy.

## 3. How would you prevent duplicate delivery?

Use an idempotency key and persist notification status.

Duplicates can happen when a queue redelivers a job, a worker crashes after sending, or a producer publishes the same event more than once. For example, the same order confirmation might be processed twice, causing two emails to be sent.

Create a unique notification identifier or idempotency key, such as:

```text
order-123:EMAIL:ORDER_CONFIRMATION
```

Store that key in a database with a unique constraint.

A simplified flow:

1. The worker receives a notification job.
2. It checks whether the notification has already been successfully processed.
3. If it has, the worker skips it.
4. Otherwise, it attempts delivery and records the result.
5. It updates the notification status and acknowledges the job.

Example notification statuses:

| Notification ID  | Status  |
|------------------|---------|
| `order-123-email` | `SENT`  |
| `order-123-sms`   | `PENDING` |
| `order-124-email` | `FAILED` |
