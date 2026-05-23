# Proxy

A proxy server sits between your device and the internet.

Your traffic goes:  
**You → Proxy → Website**

Websites see the proxy’s IP instead of yours.  
Usually works for specific apps or browsers.

Often used for:
- bypassing geo-restrictions
- hiding IPs
- filtering traffic
- web scraping

---

## Common Types

- HTTP proxy
- SOCKS5 proxy

A proxy usually does not encrypt all your traffic.

---

# Forward Proxy

A forward proxy stands between the client (user) and the internet.

## Flow:
**You → Forward Proxy → Website**

The website sees the proxy, not you.

## Used for:
- hiding client IPs
- bypassing restrictions
- filtering employee/student internet access
- caching websites

## Example:
- A school blocks social media using a proxy.
- You use a SOCKS5 proxy for browsing.

## Popular software:
- Squid
- HAProxy

---

# Reverse Proxy

A reverse proxy stands in front of servers.

## Flow:
**User → Reverse Proxy → Backend Server**

The user thinks they are talking directly to the website, but the reverse proxy handles the request first.

## Used for:
- load balancing
- SSL/TLS termination
- DDoS protection
- caching
- hiding backend servers

Very common in websites and cloud systems.

## Popular reverse proxies:
- NGINX
- Apache HTTP Server
- Cloudflare services

---

## Combined Flow Idea:
**Client → Forward Proxy → Internet → Reverse Proxy → Server**

| Component      | Role                            |
|----------------|---------------------------------|
| Forward proxy  | Represents/protects the client  |
| Reverse proxy  | Represents/protects the server  |

---

# Case 1 — Company Controls the Proxy

**You → Company Proxy → YouTube**

When you try to open YouTube:
- request goes to company proxy first
- proxy checks rules

Proxy says:
> "YouTube blocked"

So you cannot access it.

Here the proxy acts like a security guard.

---

# Case 2 — You Use Your Own Proxy

**You → Your Proxy → YouTube**

Now company network only sees:
**You → Your Proxy**

It may not know you are visiting YouTube behind that proxy.

So the proxy helps bypass restrictions.

---

## Without your own proxy:
**You → YouTube**

Company can clearly see:
- you
- YouTube

So they block it.

---

## With your own proxy:
**You → Proxy → YouTube**

Company now sees:
- you
- connecting to a proxy server

It may NOT clearly see:
- YouTube behind it

So sometimes the block is bypassed.

---

<p align="center">
  <img src="image.png" alt="alt text" width="800"/>
</p>

<p align="center">
  <img src="image-1.png" alt="alt text" width="800"/>
</p>

<p align="center">
  <img src="image-2.png" alt="alt text" width="800"/>
</p>

---

# 🔐 Firewall vs Proxy Server – Key Differences

A **firewall** mainly controls:

> “Can this connection happen or not?”

It filters network traffic.

## Firewall examples:
- block suspicious IPs
- block ports
- allow only company apps
- stop malware traffic

## Flow:
**Device ↔ Firewall ↔ Internet**

Firewall focuses on:
- connections
- packets
- ports
- protocols
- security rules

### Examples:
- Palo Alto Networks firewalls
- Fortinet firewalls

---

A **proxy** mainly controls:

> “What website/content is being accessed?”

## Proxy examples:
- block YouTube
- allow only educational sites
- cache webpages
- hide IPs

## Flow:
**User ↔ Proxy ↔ Website**

Proxy focuses on:
- websites
- web requests
- content access
- identity masking

---

## Combined View:
Employee
↓
Firewall
↓
Proxy
↓
Internet


---

# 🔍 Proxy vs VPN – Key Differences

Both hide your internet traffic in some way, but a VPN is broader and more secure.

| Feature | Proxy | VPN (Virtual Private Network) |
|--------|------|-------------------------------|
| Main Function | Routes specific app or browser traffic | Encrypts and routes all internet traffic |
| Encryption | ❌ Usually no encryption | ✅ End-to-end encryption |
| IP Masking | ✅ Selected apps only | ✅ System-wide |
| Security Level | Basic | Strong |
| Scope | App/browser-specific | All apps/services |
| Speed | Often faster | May be slower |
| Use Case | Basic anonymity, bypass restrictions | Secure browsing, privacy protection |
| Cost | Often free/cheap | Usually paid |

## 🎯 Simple Analogy:
- Proxy = middleman forwarding specific messages
- VPN = secure tunnel for all activity

---

# 🔀 Proxy vs Load Balancer – Key Differences

| Feature | Proxy Server | Load Balancer |
|--------|-------------|---------------|
| Primary Role | Intermediary for requests | Distributes traffic across servers |
| Direction | Client → Proxy → Server | Client → LB → multiple servers |
| Use Case | Filtering, masking, caching | Scalability, reliability |
| Focus | Control & privacy | Performance & availability |
| Layer | L7 / sometimes L3-4 | L4 and L7 |
| Caching | ✅ Yes | ❌ Usually no |

## 💡 Simple Analogy:
- Proxy = gatekeeper (controls request flow)
- Load balancer = traffic cop (distributes load)