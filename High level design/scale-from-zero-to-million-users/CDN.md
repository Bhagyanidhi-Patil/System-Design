# A CDN (Content Delivery Network) 

CDN is like a delivery helper for websites.Instead of sending request everytime to the one place (the main server), a CDN puts copies of your website's stuff (like images, videos, and code) on many computers around the world.  
So when someone visits, they get the content from the closest computer — which makes it load faster.

---

## ✅ CDN ≠ Cache, but they are related

- CDN uses caching to do its job better.  
- But caching can happen without a CDN — like in your browser or on a server.

---

## 🔹 What a CDN does:

1. Caches static files on servers around the world  
2. When a user visits your site:  
   - Instead of going to your main server…  
   - The CDN gives them the cached static content from the nearest server  

---

## ✅ Benefits:

- Faster load times  
- Less load on your main server  
- Better experience for users worldwide  

---

# 🌍 “CDNs are everywhere, but main data centers are placed strategically”

---

## 🌍 1. CDNs are Everywhere

CDNs (Content Delivery Networks) have servers in hundreds or even thousands of locations around the world — even in small cities or towns.

### Why?

- To serve static content (like images, videos, CSS, JS) from the closest possible location to the user  
- This makes websites load faster, no matter where the user is  
- CDN providers (like Cloudflare, Akamai, AWS CloudFront) have a global “edge network” designed just for this  

---

## 🧠 Think of CDNs as mini-caches sprinkled all around the planet.

---

## 🏢 2. Main Data Centers Are Placed Strategically

Main data centers host:

- Your core application logic  
- Your databases  
- Sensitive features like authentication, user data, and transactions  

These are:

- Expensive to run  
- Complex to manage  
- Need high-performance hardware, security, and redundancy  

---

### So instead of putting one everywhere, companies place them in key regions like:

- North America  
- Europe  
- Asia-Pacific  
- Middle East or Africa (if needed)  

---

### These centers are picked based on:

- Where most users are  
- Network speed  
- Cost  
- Legal requirements (like GDPR in Europe)  

---

## 🔁 Flow idea:

So whenever request comes:

1. First it checks CDN  
2. If not present in CDN, it checks next nearest CDN  
3. If still not present, it requests nearest data centre  

---

# 📦 How CDNs actually work

A CDN (Content Delivery Network) stores cached copies of data closer to users.

When data changes on the main server, the CDN is updated using one of these methods:

---

## 🔄 1. Cache Expiry (TTL - Time To Live)

Every file in CDN has an expiry time  
Example: 10 minutes, 1 hour, etc.

### How it works:

- CDN stores file (e.g., image.png)  
- User requests it → CDN serves cached version  
- After TTL expires → CDN fetches fresh version from origin server  

✔ Simple and automatic  
❌ Not instant updates  

---

## 🚀 2. Cache Invalidation (Manual or API trigger)

This is the most important real-world method.

When data changes:

👉 Main server (or dev) tells CDN:

“Delete old file and fetch new one”

### Example:

- You update image.png  
- You call CDN API:  
  “Invalidate image.png”  
- Next request → CDN pulls fresh file  

✔ Fast update control  
✔ Used in production systems  

---

## 🔁 3. Versioning (Best practice)

Instead of updating same file:

image-v1.png  
image-v2.png  

or

image.png?v=2  

### How it works:

- New version = new URL  
- CDN treats it as new file  
- No need to clear cache  

✔ Very reliable  
✔ Avoids sync issues  

---

## 🧠 So who “notifies” CDN?

👉 It is NOT automatic push notification

Instead:

- Either TTL expires  
- Or server tells CDN via API  
- Or new version URL is used  