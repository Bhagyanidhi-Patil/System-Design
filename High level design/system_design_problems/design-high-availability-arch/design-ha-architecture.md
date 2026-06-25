# Designing a High Availability (HA) Architecture

## What is High Availability?

High Availability (HA) means designing a system so that it remains available to users even when individual components fail.

The primary goal is to eliminate **Single Points of Failure (SPOF)** by introducing redundancy at every layer of the architecture.

For example:

* If one application server crashes, another server immediately serves incoming requests.
* If one database fails, another database instance takes over.
* If an entire data center becomes unavailable, traffic is redirected to another data center or region.

Organizations typically aim for high uptime targets such as:

* **99.9%** availability (approximately **8.76 hours** of downtime per year)
* **99.99%** availability (approximately **52 minutes** of downtime per year)
* **99.999%** availability (approximately **5 minutes** of downtime per year)

---

## Step 1: Remove Single Points of Failure

A system built with only one application server and one database creates multiple single points of failure. If either the application server or the database crashes, the entire application becomes unavailable.

To eliminate this risk, deploy multiple application servers behind a load balancer. If one server fails, the load balancer automatically routes requests to another healthy server, ensuring uninterrupted service.

---

## Step 2: Deploy Multiple Application Servers

Instead of running a single application instance, deploy multiple identical application servers behind a load balancer.

Benefits include:

* High availability
* Horizontal scalability
* Rolling deployments with zero downtime
* Better fault tolerance

All application servers should be **stateless**, meaning they should not store user-specific data locally.

---

### Why Should Application Servers Be Stateless?

Suppose user session information is stored inside Application Server A.

If Server A crashes, the user's session is lost, forcing the user to log in again.

Instead, store sessions in a centralized session store such as Redis. Since all application servers access the same session store, any server can handle any request without affecting the user experience.

Stateless services make scaling and failover significantly easier.

---

## Step 3: Use a Load Balancer

A load balancer sits in front of the application servers and distributes incoming requests across multiple instances.

Its responsibilities include:

* Distributing incoming traffic
* Performing health checks
* Detecting unhealthy servers
* Removing failed servers from the routing pool
* Sending traffic only to healthy instances

For example, the load balancer periodically checks each server's health. If one server fails to respond, it is automatically removed from service until it recovers.

### Common Load Balancing Algorithms

**Round Robin**

* Requests are distributed sequentially among servers.
* Suitable when all servers have similar capacity.

**Least Connections**

* Sends requests to the server with the fewest active connections.
* Ideal when request processing times vary.

**Weighted Round Robin**

* Servers with higher capacity receive a greater proportion of traffic.
* Useful when servers have different hardware specifications.

---

## Step 4: Database High Availability

A single database instance represents another Single Point of Failure.

To improve availability, use a **Primary-Replica (Master-Slave)** architecture.

* All write operations are directed to the Primary database.
* Read operations can be served by one or more Replica databases.

Advantages include:

* Improved read scalability
* Backup database copies
* Automatic failover capability

### Automatic Failover

If the Primary database fails, one of the Replicas is automatically promoted to become the new Primary. The application reconnects to the promoted database with minimal downtime.

---

## Step 5: Multi-Availability Zone (Multi-AZ) Deployment

Instead of deploying everything inside a single data center, distribute infrastructure across multiple Availability Zones (AZs) within the same cloud region.

Each Availability Zone is an independent data center with separate:

* Power supply
* Cooling systems
* Networking infrastructure

If one Availability Zone experiences an outage, workloads continue running in another Availability Zone, providing resilience against data center failures.


---

## Step 6: Multi-Region Architecture

For disaster recovery, deploy the application across multiple geographic regions.

Benefits include:

* Protection against regional outages
* Disaster recovery
* Lower latency for geographically distributed users

If one entire region becomes unavailable, traffic is automatically redirected to another healthy region.

---

### `Availability Zone` (AZ) vs `Region in High Availability`

An **Availability Zone (AZ)** is a physically separate data center (or a group of closely connected data centers) **within the same cloud region**.

For example, in AWS:

```text
Region: ap-south-1 (Mumbai)

      +-------------------------------+
      |       Mumbai Region           |
      |                               |
      |  AZ-1a     AZ-1b     AZ-1c     |
      | (DC-1)    (DC-2)    (DC-3)     |
      +-------------------------------+
```

* **Region** is a geographical location where cloud infrastructure is deployed, such as Mumbai, Singapore, or Virginia.

* **Availability Zone (AZ)** is an independent data center (or group of data centers) within the same region.

Each Availability Zone has:

* Independent power supply
* Independent cooling systems
* Independent networking infrastructure
* High-speed private network connections to other AZs within the same region

This design ensures that if one Availability Zone experiences a failure, the other Availability Zones continue operating without interruption.

---

## Example: Multi-AZ Architecture

```text
               Mumbai Region (ap-south-1)

          +-------------------------------+
          |        Load Balancer          |
          +-------------------------------+
                /                    \
           AZ-1a                  AZ-1b
       +------------+         +------------+
       | App Server |         | App Server |
       | Primary DB |<------->| Replica DB |
       +------------+         +------------+
```

If **AZ-1a** experiences a power outage or infrastructure failure:

* The Load Balancer automatically routes incoming requests to the application server running in **AZ-1b**.
* The database replica in **AZ-1b** can be promoted to become the new Primary database.
* The application continues serving users with little or no downtime.

This is known as **Multi-Availability Zone (Multi-AZ) Deployment**, which protects against failures of an individual data center.

---

### What Happens if the Entire Region Fails?

A **Multi-AZ** architecture protects only against the failure of a single Availability Zone. It **cannot** protect against an outage affecting the entire region.

For regional disaster recovery, a **Multi-Region Architecture** is required.

```text
              Global Load Balancer / DNS
                        |
              -------------------------
              |                       |
       Mumbai Region          Singapore Region
        (ap-south-1)         (ap-southeast-1)
             |                       |
       Multiple AZs           Multiple AZs
```

If the **Mumbai Region** suffers a large-scale outage:

* DNS or a Global Load Balancer automatically redirects user traffic to the **Singapore Region**.
* The backup region continues serving requests, ensuring business continuity.


---

## Step 7: DNS Failover

- **DNS Failover** is a mechanism that automatically redirects user traffic to a healthy server or region when the primary server or region becomes unavailable.

- Normally, when a user enters a website such as **[www.example.com](http://www.example.com)**, the user's device first queries the **DNS (Domain Name System)** to obtain the IP address of the server hosting the application.

For example:

```text
User
   |
   |  www.example.com
   |
 DNS Server
   |
   |----> 13.10.20.30 (Mumbai Region)
```

The user is then connected to the Mumbai region.

---

### What Happens if the Mumbai Region Fails?

Suppose the entire Mumbai region becomes unavailable due to a major outage.

Without DNS Failover:

```text
User
   |
DNS
   |
Mumbai Region ❌
```

The DNS continues returning the IP address of the failed region, so users cannot access the application until the issue is manually fixed.

---

### With DNS Failover

A health monitoring service continuously checks whether each region is healthy.

For example:

* Mumbai → Healthy ✅
* Singapore → Healthy ✅

Initially, DNS returns the IP address of the Mumbai region.

```text
User
   |
DNS
   |
Mumbai Region
```

If the Mumbai region becomes unavailable:

* The health monitor detects the failure.
* DNS updates its records.
* New users are automatically redirected to the Singapore region.

```text
User
   |
DNS
   |
Singapore Region
```

Users continue accessing the application with minimal downtime.

---

### How Does DNS Know a Region Has Failed?

DNS itself does not perform health checks.

Instead, a monitoring service continuously checks the application's health by sending periodic requests to each region.

For example:

```text
Health Monitor

    |

Mumbai Region  ----> Failed ❌

Singapore Region ----> Healthy ✅
```

Once the monitor detects that Mumbai is unavailable, it instructs the DNS service to stop returning Mumbai's IP address and instead return Singapore's IP address.

---

### Real-World Example

Suppose Amazon deploys its application in:

* Mumbai (Primary Region)
* Singapore (Backup Region)

Normally:

```text
amazon.com

↓

Mumbai Region
```

If Mumbai experiences a complete outage:

```text
amazon.com

↓

Singapore Region
```

Users continue using the application without needing to change the URL.

---

## Step 8: Distributed Cache

Frequently accessed data should be stored in a distributed cache such as Redis.

Advantages include:

* Reduced database load
* Lower response time
* Improved application performance

The application should always be capable of reading directly from the database if the cache becomes unavailable. A cache should improve performance but should never become a dependency for system availability.

---

## Step 9: Message Queue

Use a message queue such as Kafka to decouple services.

Instead of synchronous communication:

* The producer publishes messages to Kafka.
* Consumer services process messages asynchronously.

If a downstream service crashes, messages remain safely stored inside Kafka until the service recovers, preventing data loss.

---

## Step 10: Monitoring and Health Checks

Continuously monitor system health using metrics such as:

* CPU utilization
* Memory usage
* Disk utilization
* Response time
* Error rates
* Database replication lag

Monitoring platforms include:

* Prometheus
* Grafana
* CloudWatch

Alerts should trigger whenever critical thresholds are exceeded, enabling rapid detection and response to failures.

---

## Step 11: Auto Scaling

Traffic varies throughout the day.

Auto Scaling automatically:

* Adds application servers during traffic spikes.
* Removes unused servers during low traffic.

This ensures:

* High availability
* Better resource utilization
* Cost optimization

---

## Step 12: Graceful Failover

When an application server fails, existing requests are allowed to complete if possible, while new requests are immediately redirected to healthy servers.

This minimizes service disruption and ensures users experience little or no downtime.

---

## Complete High Availability Architecture

```text
                          Internet
                              |
                        DNS Failover
                              |
                   Global Load Balancer
                    /                 \
             Region A               Region B
                |                      |
         +---------------+      +---------------+
         | Load Balancer |      | Load Balancer |
         +---------------+      +---------------+
           /    |    \             /    |    \
       App1   App2  App3       App4   App5  App6
          \     |     /           \     |     /
             Redis Cache             Redis Cache
                    \                 /
                     \               /
                  Message Queue (Kafka)
                           |
                   Primary Database
                     /            \
                Replica 1      Replica 2
```

### Architecture Explanation

1. **Internet**
   Users send requests from anywhere in the world.

2. **DNS Failover**
   Continuously monitors the health of each region. If one region becomes unavailable, DNS automatically redirects users to another healthy region.

3. **Global Load Balancer**
   Routes user requests to the nearest or healthiest region based on latency, health, or routing policies.

4. **Region A and Region B**
   The application is deployed in multiple geographic regions to provide disaster recovery and protect against complete regional outages.

5. **Regional Load Balancer**
   Each region has its own load balancer that distributes incoming traffic among multiple application servers and removes unhealthy instances using health checks.

6. **Application Servers (App1–App6)**
   Multiple stateless application servers process user requests. Since they are stateless, any server can handle any request, making failover and horizontal scaling simple.

7. **Redis Cache**
   Frequently accessed data is stored in Redis to reduce database load and improve response time. If Redis becomes unavailable, the application falls back to the database.

8. **Message Queue (Kafka)**
   Kafka enables asynchronous communication between services. If a downstream service fails, messages remain safely stored in Kafka until the service recovers, preventing data loss.

9. **Primary Database**
   All write operations are directed to the Primary database.

10. **Replica Databases**
    Replicas receive data from the Primary and primarily serve read requests. If the Primary fails, one of the Replicas is promoted to become the new Primary through automatic failover.

### Key Benefits

* No Single Point of Failure (SPOF)
* Automatic failover for servers and databases
* High availability across multiple Availability Zones and Regions
* Horizontal scalability
* Better performance through caching
* Reliable asynchronous communication using Kafka
* Disaster recovery through Multi-Region deployment
* Continuous monitoring and health checks ensure uninterrupted service
