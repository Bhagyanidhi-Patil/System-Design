# CDN (Content Delivery Network)

A CDN is a globally distributed network of servers that delivers content to users from the nearest location instead of the main server.

It helps websites/apps become:
- faster
- more scalable
- more reliable

---

# Simple Analogy

Imagine:

- Your main server is in the US
- A user opens your app from India

## Without CDN

```text
India User → US Server → Response
```

This is slow because data travels very far.

## With CDN

```text
India User → India CDN Edge Server → Response
```

Much faster.

---

# What Does CDN Store?

Usually:

- images
- videos
- CSS
- JavaScript
- PDFs
- static assets

Modern CDNs can also cache:

- API responses
- HTML pages
- dynamic content

---

# How CDN Works

## Step 1

User requests an image:

```text
example.com/logo.png
```

---

## Step 2

Request goes to nearest CDN edge server.

Example:

- Bangalore edge server
- Singapore edge server
- London edge server

---

## Step 3

If CDN already has the file:
→ return immediately.

This is called:
Cache Hit

Fast response.

---

## Step 4

If CDN does not have the file:
→ fetch from origin server.

This is called:
Cache Miss

Then CDN stores it for future users.

---

# Architecture

```text
                Origin Server
                      │
         ┌────────────┼────────────┐
         │            │            │
     CDN India    CDN Europe   CDN US
         │            │            │
      Users        Users        Users
```

---

# Common CDN Providers

Popular companies:

- Cloudflare
- Akamai Technologies
- Amazon Web Services
- Fastly
- Google Cloud

---

# Example in Real Life

When you open:

- Netflix
- YouTube
- Instagram

The videos/images are usually delivered from nearby CDN servers instead of one central server.

That’s why content loads quickly worldwide.

---

# Is CDN Replica of Application Server?

A CDN is not a replica of the application server.

> “A CDN stores cached copies of static or frequently accessed content (HTML, CSS, JS, images, videos, cached API responses) closer to users. The content may originate from the application server, but the CDN is not a full replica of the application server because it does not contain the complete business logic or application processing capabilities.”

---

# What Application Server Does

Application server handles:

- business logic
- authentication
- payments
- DB queries
- API processing

Example:

```js
login()
checkout()
createOrder()
```

Examples:

- Node.js server
- Spring Boot app
- Django backend

---

# What CDN Does

CDN mainly serves:

- cached images
- videos
- JS/CSS
- static files
- cached API responses

It tries to avoid hitting the app server repeatedly.

---

# Simple Analogy

## Application Server

= Restaurant Kitchen

Actually cooks food.

---

## CDN

= Nearby food delivery pickup center

Stores popular prepared items for quick delivery.

It does NOT cook food.

---

# How CDNs Handle Freshness

There are several mechanisms.

---

# 1. TTL (Time To Live)

Most common.

Example:

```text
Cache this image for 5 minutes
```

After 5 minutes:

- CDN discards old copy
- fetches latest from origin

---

# 2. Cache Invalidation / Purge

When data changes:
you tell CDN:

```text
Delete old cached version
```

Then next request fetches latest version.

Used heavily in:

- e-commerce
- news websites
- social media

---

# 3. Versioned Assets

Very common trick.

Instead of:

```text
logo.png
```

Use:

```text
logo_v2.png
```

or:

```text
app.js?v=123
```

New filename = CDN treats it as new content.

---

# 4. Revalidation

CDN asks:

```text
“Has this file changed?”
```

Origin responds:

- yes → send new version
- no → keep cached version

Using:

- ETag
- Last-Modified headers

---

# What is Replica Then?

A replica usually means:
Another copy of the SAME system/service

Example:

```text
App Server 1
App Server 2
App Server 3
```

These are replicas of the application server.

Or:

```text
Primary DB
Read Replica DB
```

Replica means:
same functionality duplicated.