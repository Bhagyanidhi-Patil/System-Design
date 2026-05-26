one request
# REST vs gRPC

Think of both as ways for two applications to talk to each other.

Examples:

- A mobile app asking a server for user data
- One backend service calling another service

Both solve the same problem, but differently.

## 1. REST
REST (Representational State Transfer) is a way for applications to communicate over HTTP using standard methods like GET, POST, PUT, and DELETE.

It works by sending requests to URLs (endpoints) and usually exchanges data in JSON format.
REST works like a normal website request.

A client sends:

- a URL
- an HTTP method (GET, POST, etc.)
- usually JSON data

Example:

```http
GET /users/101
```

Server response:

```json
{
  "id": 101,
  "name": "Rahul"
}
```

### How REST Works

REST uses:

- HTTP
- URLs
- JSON

Each resource has a URL:

- /users
- /products
- /orders

### Operations (HTTP methods)

| Method | Meaning |
|--------|---------|
| GET    | Read data |
| POST   | Create data |
| PUT    | Update entire data |
| DELETE | Remove data |
| PATCH  | Update part of resource |

## 2. gRPC
gRPC (Google Remote Procedure Call) is a high-performance communication framework used for calling methods between services (in micro services) as if they were local functions.

It uses Protocol Buffers (binary format) and runs on HTTP/2.
gRPC works differently.

Instead of URLs and JSON, it uses:

- Protocol Buffers (protobuf)
- predefined service methods

It feels more like calling a function from another machine.

### Example

You define a service:

```proto
service UserService {
  rpc GetUser(UserRequest) returns (UserResponse);
}
```

Then call it like:

```python
user = client.GetUser(id=101)
```

This looks like a normal function call, but it actually communicates over the network.

### How gRPC Works

gRPC uses:

- HTTP/2
- binary data
- protobuf contracts

Data is converted into a compact binary format.

This makes communication:

- faster
- smaller
- more efficient

## REST vs gRPC — Main Difference

- REST: Client -> URL -> JSON -> Server
- gRPC: Client -> Method Call -> Binary -> Server

## Why gRPC Is Faster

gRPC gains speed from:

1. Binary Serialization

Binary is compact and uses less network bandwidth.

2. HTTP/2

HTTP/2 supports:

- multiplexing
- persistent connections
- streaming

REST usually uses HTTP/1.1.

3. Code Generation

gRPC generates client/server code automatically, reducing parsing overhead.

### REST

Usually:

- one request
- one response

### gRPC

Supports:

| Type | Meaning |
|------|---------|
| Unary | Single request/response |
| Server Streaming | Server sends continuous updates |
| Client Streaming | Client sends stream |
| Bidirectional | Both stream simultaneously |

