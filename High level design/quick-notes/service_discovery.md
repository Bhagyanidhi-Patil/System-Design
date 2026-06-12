# Service Discovery

Service Discovery is a mechanism in distributed systems and microservices architectures that allows services to automatically find and communicate with each other without hardcoding network locations (IP addresses and ports).

## Why Service Discovery is Needed

In traditional applications, servers often have fixed IP addresses and locations.

### Example:

```text
Frontend → 192.168.1.10:8080
Backend  → 192.168.1.20:9000
Database → 192.168.1.30:3306
```

This works when servers rarely change.

However, in cloud and microservices environments:

* Services are created and destroyed dynamically.
* Containers restart frequently.
* IP addresses change.
* Multiple instances of the same service run for scalability.

### Example:

```text
User Service:
Instance 1 → 10.0.1.5
Instance 2 → 10.0.1.8
Instance 3 → 10.0.1.12
```

Hardcoding addresses becomes impossible.

Service Discovery solves this problem.

Service Discovery is the process by which one service dynamically locates another service on a network.

Instead of knowing the exact IP address, a service asks a registry:

```text
"Where is User Service?"
```

The registry replies:

```text
User Service:
10.0.1.5:8080
10.0.1.8:8080
10.0.1.12:8080
```

# Components of Service Discovery

## 1. Service Provider

A service that offers functionality.

### Example:

* User Service
* Payment Service
* Order Service

```text
User Service
IP: 10.0.1.5
Port: 8080
```

## 2. Service Registry

A central database that stores service locations.

### Examples:

* Eureka
* Consul
* Apache ZooKeeper
* etcd

Registry stores:

```text
User Service → 10.0.1.5:8080
Order Service → 10.0.1.8:9090
Payment Service → 10.0.1.10:7070
```

## 3. Service Consumer

A service that needs another service.

### Example:

```text
Order Service
```

needs

```text
Payment Service
```

Instead of knowing the address:

```text
10.0.1.10:7070
```

it queries the registry.

# In Software Systems

Suppose you have an e-commerce application with these services:

```text
Order Service
Payment Service
User Service
```

## Service Provider

A service that provides functionality to other services.

### Example:

```text
Payment Service
```

It provides:

* Payment processing
* Transaction validation
* Payment status

So Payment Service = Service Provider.

## Service Consumer

A service that uses functionality provided by another service.

### Example:

```text
Order Service
```

When a customer places an order:

```text
Order Service
      |
      v
Payment Service
```

Order Service asks Payment Service:

```text
"Please process payment for ₹1000."
```

Here:

* Order Service = Service Consumer
* Payment Service = Service Provider

The same service can be both a Service Provider and a Service Consumer at the same time.

This is very common in microservices architectures.

### Order Service as a Consumer

When Order Service needs user details:

```text
Order Service ---> User Service
```

* Order Service = Consumer
* User Service = Provider

### Order Service as a Provider

Now suppose the frontend application wants to create an order:

```text
Frontend ---> Order Service
```

* Frontend = Consumer
* Order Service = Provider

## How Service Discovery Works

## Step 1: Service Starts

Payment Service starts.

```text
Payment Service
10.0.1.10:7070
```

## Step 2: Registration

Payment Service registers itself.

```text
Registry

Payment Service
→ 10.0.1.10:7070
```

## Step 3: Consumer Requests

Order Service wants Payment Service.

```text
Registry:
Where is Payment Service?
```

## Step 4: Registry Responds

```text
Payment Service:
10.0.1.10:7070
```

## Step 5: Communication

Order Service communicates directly.

```text
Order Service
      ↓
Payment Service
```

## Service Discovery in Kubernetes

In Kubernetes, service discovery is built-in.

Suppose:

```text
pod1 → User Service
pod2 → User Service
pod3 → User Service
```

Pods get dynamic IPs:

```text
10.1.1.2
10.1.1.5
10.1.1.8
```

Instead of using IPs:

```text
10.1.1.2
```

applications use:

```text
user-service
```

Kubernetes DNS resolves:

```text
user-service.default.svc.cluster.local
```

to the correct pod.

This is automatic service discovery.

## Service discovery with load balancer
## Without Load Balancing

Suppose there are 3 Payment Service instances:

```text
Payment Service A → 10.0.1.10
Payment Service B → 10.0.1.11
Payment Service C → 10.0.1.12
```

Now imagine 1,000 customers are making payments.

If every request goes to only:

```text
10.0.1.10
```

then:

```text
10.0.1.10 → Overloaded 😫
10.0.1.11 → Idle 😴
10.0.1.12 → Idle 😴
```

This is inefficient.

## With Load Balancing

The load balancer spreads requests across all available instances.

```text
Customer 1 → 10.0.1.10
Customer 2 → 10.0.1.11
Customer 3 → 10.0.1.12
Customer 4 → 10.0.1.10
Customer 5 → 10.0.1.11
Customer 6 → 10.0.1.12
```

Now the work is shared:

```text
10.0.1.10 → Busy
10.0.1.11 → Busy
10.0.1.12 → Busy
```

No single server gets overloaded.

## Where Service Discovery Comes In

The load balancer first needs to know:

```text
"What Payment Service instances are available?"
```

It asks the Service Registry.

Registry replies:

```text
Payment Service:

10.0.1.10
10.0.1.11
10.0.1.12
```

Now the load balancer knows all available instances and can distribute traffic among them.

## Health Checks

Good service discovery systems continuously monitor service health.

### Example:

```text
Payment Service A → Healthy
Payment Service B → Down
Payment Service C → Healthy
```

Registry only returns:

```text
A
C
```

and excludes:

```text
B
```

This prevents failed requests.
