# How Do You Design for Fault Tolerance?

## Definition

- Fault tolerance is the ability of a system to continue operating correctly even when some components fail.

- In distributed systems, failures are expected rather than exceptional. Servers can crash, networks can partition, databases can become unavailable, and services can time out. A well-designed system should remain available and recover gracefully without causing a complete outage.

---

## 1. Identify Failure Points First

Before designing fault tolerance, identify where failures can occur:

`Client → Load Balancer → Application Servers → Cache → Database → Message Queue`

Possible failures include:

* Load balancer failure
* Application server crash
* Cache outage
* Database failure
* Network partition
* Message broker failure
* Datacenter outage

A good system assumes every component can fail.

---

## 2. Remove Single Points of Failure (SPOF)

### Bad Design

`Users → Load Balancer → Single App Server → Single Database`

If the application server crashes, the entire system becomes unavailable.

### Better Design

`Users → Load Balancer → Multiple Application Servers → Database Cluster`

Benefits:

* Load balancer redirects traffic to healthy servers.
* Users remain unaffected by individual server failures.

**Principle:** Every critical component should have redundancy.

---

## 3. Replication

Replication means maintaining multiple copies of data or services.

### Database Replication
```
Primary Database

↓            ↓

Replica 1 Replica 2
```
Benefits:

* Higher availability
* Faster read operations
* Disaster recovery

If the primary database fails, a replica can be promoted to become the new primary, allowing the system to continue operating.

Examples:

* MySQL Replication
* PostgreSQL Streaming Replication
* MongoDB Replica Sets

---

## 4. Load Balancing

Load balancers distribute traffic across multiple servers.
```
Load Balancer

↙ ↓ ↘

App1 App2 App3
```
Benefits:

* Prevents server overload
* Enables automatic failover
* Improves availability

If App2 crashes, traffic is automatically routed to App1 and App3.

Examples:

* NGINX
* HAProxy
* AWS Application Load Balancer (ALB)

---

## 5. Health Checks

Load balancers continuously verify server health through health check endpoints.

Example:
```
LB → /health

LB → /health

LB → /health
```
Healthy Response:

`200 OK`

Unhealthy Response:

`500 Error`

Timeout

`Unhealthy instances are removed from the load balancer rotation, preventing user traffic from reaching failed servers.`

---

## 6. Failover Mechanisms

Failover automatically switches traffic to backup resources when failures occur.

### Database Failover
```
Primary Database

↓

Standby Database
```
If the primary database fails, the standby database is promoted to primary.

### Multi-Region Failover
```
Region A (Active)

Region B (Standby)
```
If Region A becomes unavailable, traffic is redirected to Region B.

Used by large-scale companies such as Netflix, Amazon, and Google.

---

## 7. Timeouts

Never wait indefinitely for a dependency.

Bad Design:

`Service A → Service B (hangs forever)`

Result:

* Threads become blocked
* Resources are exhausted

Good Design:

Timeout = 2 seconds

After timeout:

* Fail fast
* Return fallback response
* Retry later

Benefits:

* Prevents thread exhaustion
* Improves responsiveness

---

## 8. Retries with Exponential Backoff

Many failures are temporary and recover quickly.

Example:
```
Attempt 1 → Fail → Wait 1 second

Attempt 2 → Fail → Wait 2 seconds

Attempt 3 → Fail → Wait 4 seconds
```
Formula:

Delay = Base × 2ⁿ

Benefits:

* Reduces pressure on recovering services
* Prevents retry storms

---

## 9. Circuit Breaker Pattern

When a service repeatedly fails, temporarily stop sending requests.

Normal State:

`Service A → Service B`

Repeated Failures:

`Service B Down`

Circuit Open:

`Service A ✖ Service B`

Requests fail immediately instead of wasting resources.

After a recovery period:

`Half-Open State`

A few test requests are sent.

If successful:

`Circuit Closed`

Benefits:

* Prevents cascading failures
* Protects downstream services

Popular Libraries:

* Resilience4j
* Hystrix (legacy)

---

## 10. Graceful Degradation

Not every feature is equally important.

### Critical Features

* Login
* Search
* Checkout

### Non-Critical Features

* Recommendations
* Reviews
* Analytics

If the recommendation service fails:

* Hide recommendations
* Continue checkout flow

Users can still complete purchases.

This approach is called graceful degradation.

---

## 11. Caching for Fault Tolerance

Architecture:

Client → Cache → Database

If the database becomes slow or temporarily unavailable:

* Serve cached data

Benefits:

* Lower latency
* Better availability

Examples:

* Redis
* Memcached

---

## 12. Message Queues

Use asynchronous communication to decouple services.

### Without Queue

Order Service → Payment Service

If Payment Service fails, order processing stops.

### With Queue

Order Service → Kafka → Payment Service

If Payment Service is down:

* Messages remain in the queue
* Processing resumes later

Benefits:

* Decoupling
* Reliability
* Backpressure handling

Examples:

* Kafka
* RabbitMQ
* Amazon SQS

---

## 13. Data Backup and Disaster Recovery

Always plan for catastrophic failures.

### Backups

* Daily full backups
* Hourly incremental backups

Store backups in:

* Different servers
* Different regions
* Different cloud storage systems

### Recovery Metrics

#### RPO (Recovery Point Objective)

Maximum acceptable data loss.

Example:

RPO = 5 minutes

At most 5 minutes of data can be lost.

#### RTO (Recovery Time Objective)

Maximum acceptable downtime.

Example:

RTO = 15 minutes

The system must recover within 15 minutes.

---

## 14. Multi-AZ Deployment

Deploy services across multiple Availability Zones.

Load Balancer

↙ ↘

AZ-1 AZ-2

App1 App2 App3 App4

↓

Database Cluster

If AZ-1 fails, traffic is automatically routed to AZ-2.

Benefits:

* High availability
* Better fault isolation

---

## 15. Multi-Region Architecture

For mission-critical applications.

Global DNS

↙ ↘

Region A Region B

Advantages:

* Disaster recovery
* Lower latency
* High availability

Trade-offs:

* Increased complexity
* Data consistency challenges

---

## 16. Idempotency

Retries can create duplicate operations.

Example:

POST /pay

If a timeout occurs, the client may retry the request.

Without idempotency:

* Payment charged twice

With Idempotency Key:

Request-ID: abc123

The server recognizes the duplicate request and returns the original result instead of processing it again.

Important for:

* Payments
* Order creation
* Money transfers

---

## 17. Monitoring and Alerting

Failures must be detected quickly.

Monitor:

* CPU utilization
* Memory usage
* Error rates
* Latency
* Throughput
* Queue depth

Tools:

* Prometheus
* Grafana
* Datadog

Example Alerts:

* Error Rate > 5%
* Latency > 500 ms
* CPU Usage > 90%

Alerts enable engineers to respond immediately before users are affected.

---

## Interview Answer (2-Minute Summary)

To design a fault-tolerant system, I first identify all potential failure points and eliminate single points of failure through redundancy. I deploy multiple application instances behind a load balancer, use replicated databases with automatic failover, and perform health checks to remove unhealthy nodes. For service-to-service communication, I implement timeouts, retries with exponential backoff, and circuit breakers to prevent cascading failures. I use message queues such as Kafka for asynchronous processing and graceful degradation so that non-critical features can fail without affecting core functionality. For disaster recovery, I maintain backups, define RPO and RTO targets, and deploy across multiple availability zones or regions. Finally, I use monitoring, alerting, and idempotency mechanisms to ensure the system remains reliable and recoverable even during failures.
