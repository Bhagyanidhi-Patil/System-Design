# WebSocket vs Polling

| Polling                                   | WebSocket                                    |
| ----------------------------------------- | -------------------------------------------- |
| Client repeatedly asks server for updates | Server and client keep persistent connection |
| Request-response every time               | Two-way real-time communication              |
| Wasteful                                  | Efficient                                    |
| Higher latency                            | Low latency                                  |

Suppose you are building:
```
- chat app
- stock market app
- live cricket score
- collaborative editor
- notifications
```

#### Question:

How will client get live updates?

Two common approaches:
```
- Polling
- WebSocket
```
---

## 1. Polling

#### Polling means:

Client repeatedly asks server:

***"Any new updates?"***

again and again.

### Flow

```text
Client ----> Server : Any update?
Server ----> Client : No

(after 5 sec)

Client ----> Server : Any update?
Server ----> Client : Yes
```

### Example

Frontend calls API every 5 seconds:

```js
setInterval(() => {
   fetch("/messages")
}, 5000)
```

### Problem with Polling

Suppose no new messages exist.

Still requests happen repeatedly.

This causes:
```
- unnecessary network traffic
- server load
- wasted resources
```
---

### Types of Polling

#### A) Short Polling

Regular interval requests.

Example:

- every 5 sec

Most common.

#### B) Long Polling

- Client sends request.
- Server HOLDS request until new data available.
- Then responds.
- Client immediately reconnects.

Better than short polling.

But still not true real-time persistent connection.

---

## 2. WebSocket

WebSocket creates:

- Persistent Full-Duplex Connection

**Persistent** Means,

***Connection stays open for a long time instead of closing after one request-response.***

Meaning:

- connection stays open
- both server and client can send data anytime

### Flow

```text
Client <=================> Server
      persistent connection
```

### WebSocket Analogy

Instead of repeatedly asking:

"Bus arrived?"

friend calls you immediately when bus arrives.

### Why WebSocket Is Fast

Because connection already exists.

No repeated:

- TCP handshake
- HTTP headers
- reconnection overhead

Latency becomes very low.

---

### Main Advantage of WebSocket

### Polling Flow (Live Score)

Client repeatedly asks:
```
- "Any score update?"
```
#### Example:
```
- Every 5 seconds:
  - Client → Server : score?
```
Even if score unchanged:
```
- requests still happen.
```
#### Polling Example:

```text
Client → score?
Server → 120/2

(after 5 sec)

Client → score?
Server → still 120/2
```

Unnecessary traffic.

### WebSocket Flow

With WebSocket:
```
- Connection already stays open.
```
Whenever score changes:
```
- Server itself pushes update immediately.
```
### Full Duplex

Very important interview term.

### HTTP Polling

Usually:

- Client → Request
- Server → Response

### WebSocket

- Client ↔ Server anytime

Both can initiate communication.

### When Polling Is Enough

Polling is okay for:

- weather refresh
- email refresh
- dashboard refresh every minute
- low-frequency updates

### When WebSocket Is Better

WebSocket is ideal for:

- chat applications
- multiplayer games
- trading systems
- live notifications
- collaborative editing
- ride tracking
- live sports updates

## Scaling Challenge with WebSockets

Important advanced point.

Since connections stay open:
```
1 million users = 1 million open connections
```

Server must maintain them.

This creates challenges:

- memory usage
- connection management
- load balancing

### Typical Architecture

For WebSockets:

```text
Client
   ↓
WebSocket Gateway
   ↓
Message Broker (Kafka/Redis)
   ↓
Application Servers
```

## Step-by-Step WebSocket Connection Flow

### STEP 1 — Client Wants Real-Time Communication

Example:

- chat app
- live score
- notifications

Client decides to open WebSocket connection.

### STEP 2 — TCP Connection Is Established First

Before WebSocket or HTTP:

- Client and server first establish TCP connection
- using TCP 3-way handshake.

#### TCP Handshake

```text
Client → SYN
Server → SYN-ACK
Client → ACK
```

Now TCP connection is ready.

**Important:**

Everything after this uses same TCP connection.

### STEP 3 — Client Sends HTTP Upgrade Request

Now client sends normal HTTP request:

```http
GET /chat HTTP/1.1
Host: example.com
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: abc123
Sec-WebSocket-Version: 13
```

Meaning:

"Please upgrade this HTTP connection to WebSocket."

This request travels over already established TCP connection.

### STEP 4 — Server Accepts Upgrade

Server replies:

```http
HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: xyz456
```

Meaning:

"Okay, protocol switched to WebSocket."

**Important:**

`101 Switching Protocols` is key response.

### STEP 5 — HTTP Ends

After successful upgrade:

- HTTP request-response cycle stops.
- Connection becomes Persistent WebSocket connection
- using SAME TCP connection.

### STEP 6 — Real-Time Bidirectional Communication Starts

Now:

```text
Client <=================> Server
```

Both can send messages anytime.

- Client Can Send: "hello"
- Server Can Push Anytime: "new notification"

without waiting for request.

### STEP 7 — Connection Remains Open

Unlike HTTP:
```
- Request → Response → Close
```

WebSocket keeps connection alive.

### STEP 8 — Connection Closes

Either side can close connection.

Example:

```js
socket.close()
```

Then TCP connection terminates.

## Full Flow Diagram

1. TCP Handshake
   - Client → SYN
   - Server → SYN-ACK
   - Client → ACK

2. HTTP Upgrade Request
   - Client → Upgrade:websocket

3. Server Accepts
   - Server → 101 Switching Protocols

4. WebSocket Established

```text
Client <=================> Server
      Real-time messages
```

## Why TCP Matters

WebSocket runs over TCP because TCP provides:

- reliable delivery
- ordered messages
- persistent connection

## How Server Tracks Connections

Server maintains open socket objects.

Example conceptually:

- User1 → SocketA
- User2 → SocketB
- User3 → SocketC

When server wants to push update:

- `SocketB.send("new message")`

```js
socket.close()
```

Then TCP connection terminates.

## Full Flow Diagram

1. TCP Handshake
   - Client → SYN
   - Server → SYN-ACK
   - Client → ACK

2. HTTP Upgrade Request
   - Client → Upgrade:websocket

3. Server Accepts
   - Server → 101 Switching Protocols

4. WebSocket Established

```text
Client <=================> Server
      Real-time messages
```

## Why TCP Matters

WebSocket runs over TCP because TCP provides:

- reliable delivery
- ordered messages
- persistent connection

## How Server Tracks Connections

Server maintains open socket objects.

Example conceptually:

- User1 → SocketA
- User2 → SocketB
- User3 → SocketC

When server wants to push update:

- `SocketB.send("new message")`

