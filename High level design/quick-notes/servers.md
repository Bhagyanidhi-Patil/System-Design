# 🖥️ What is a Server?

A **server is a computer (super-computer) that provides some service to other computers (clients) over a network.**

That's the simplest definition.

```text
Client  ───────request──────→  Server
Client  ←──────response──────  Server
```

The "server" is not necessarily a special kind of computer. **It is usually a computer running software that is providing a service.**

---

# 1. Simple Example

When you open YouTube:

```text
Your Laptop
    │
    │ "Give me this video"
    ↓
YouTube Server
    │
    │ "Here is the video"
    ↓
Your Laptop
```

Your laptop is the **client**.

The computer handling your request is the **server**.

---

# 2. What is Actually Inside a Server?

A physical server is basically a powerful computer:

```text
┌──────────────────────────────┐
│       Physical Server        │
│                              │
│  CPU                         │
│  RAM                         │
│  Storage (SSD/HDD)           │
│  Network Interface           │
│  Motherboard                 │
│                              │
└──────────────────────────────┘
```

It is not fundamentally different from your laptop.

The difference is that servers are generally designed for:

* Running continuously
* Handling many requests
* Having lots of CPU/RAM/storage
* Reliability
* High-speed networking

---

# 3. Server ≠ Server Software

This is very important.

A **server can mean the machine**, but "server" can also refer to the **software running on that machine**.

For example:

```text
Physical Machine
       ↓
    Linux OS
       ↓
┌──────┼───────────┐
↓      ↓           ↓
Web    Database    Backend
Server Server      Server
```

Here:

* Physical machine = computer
* Web server = software
* Database server = software
* Backend server = software

That's why the word **server** can sometimes be confusing.

---

# 4. Example: Your Backend Application

Suppose you build a Java Spring Boot application.

You can run it on a server:

```text
┌─────────────────────────────┐
│        Physical Server      │
│                             │
│       Linux OS              │
│          ↓                  │
│     Java + Spring Boot      │
│          ↓                  │
│     Your Backend App        │
└─────────────────────────────┘
```

Your users send requests:

```text
User
 ↓
Internet
 ↓
Your Server
 ↓
Spring Boot Application
 ↓
Response
```

So when someone says:

> "Deploy the application on a server."

They basically mean:

> **Run your application software on a computer that is reachable over the network and can handle requests.**

---

# 5. Database Server

Now connect this with your previous question.

Suppose you have MySQL:

```text
┌─────────────────────────────┐
│        Server Machine       │
│                             │
│       Linux OS              │
│          ↓                  │
│        MySQL                │
│          ↓                  │
│       Database              │
│                             │
│ users / orders / products   │
└─────────────────────────────┘
```

So:

**Server** = computer

**MySQL** = database software

**Database** = your organized data

---

# 6. You Can Have Multiple Servers

A large application doesn't usually run on just one server.

For example:

```text
                    Load Balancer
                         │
              ┌──────────┼──────────┐
              ↓          ↓          ↓
           Server 1   Server 2   Server 3
              │          │          │
              └──────────┼──────────┘
                         ↓
                    Database
```

Why?

Because if Server 1 gets too many requests:

```text
Server 1 → 1 million requests 😵
```

you can distribute them:

```text
Server 1 → 333K
Server 2 → 333K
Server 3 → 334K
```

This is where your earlier question about **load balancers** comes in.

---

# 7. Where Does Cloud Fit?

Now everything connects:

```text
                  CLOUD PROVIDER
                       │
                 Physical Servers
                       │
          ┌────────────┼────────────┐
          ↓            ↓            ↓
        Server       Server       Server
          │            │            │
       Backend      Backend      Backend
```

Instead of buying these physical servers yourself, you can **rent computing resources from a cloud provider**.

---

# 8. Where Does AZ Fit?

Now add AZ:

```text
              CLOUD REGION
                   │
        ┌──────────┼──────────┐
        ↓          ↓          ↓
      AZ-1       AZ-2       AZ-3
        │          │          │
     Servers    Servers    Servers
        │          │          │
     Backend    Backend    Backend
```

So:

**Cloud** → You use infrastructure provided by a cloud provider.

**Region** → Geographic area containing cloud infrastructure.

**AZ** → Isolated infrastructure location inside a region.

**Server** → Computer running your application/software.

**Database** → Data managed by database software running on a server.

**Load Balancer** → Component that distributes requests across servers.

**CDN** → Distributed servers that cache content closer to users.

---

# 🧠 Final Mental Model

```text
                         USERS
                           │
                           ↓
                          CDN
                           │
                           ↓
                    Load Balancer
                           │
              ┌────────────┼────────────┐
              ↓            ↓            ↓
            AZ-1          AZ-2         AZ-3
              │            │            │
           Server        Server       Server
              │            │            │
        Backend App   Backend App   Backend App
              │            │            │
              └────────────┼────────────┘
                           ↓
                     Database Server
                           │
                         MySQL
                           │
                         Data
```

### One sentence to remember:

> **A server is a computer that runs software and provides a service to other computers over a network.**
