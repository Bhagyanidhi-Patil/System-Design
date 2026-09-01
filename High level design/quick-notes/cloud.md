# ☁️ What is "Cloud"?

**Cloud means using someone else's computers over the internet instead of owning and managing the computers yourself.**

For example, companies like Amazon Web Services (AWS), Microsoft Azure, and Google Cloud own huge numbers of physical servers in data centers.

You can **rent their servers, storage, databases, networking, etc.** and use them through the internet.

---

## Without Cloud

Suppose you want to build an application.

You need to buy:

```text
Your Company
    │
    ├── 🖥️ Servers
    ├── 💾 Storage
    ├── 🌐 Network equipment
    ├── 🔌 Power
    ├── ❄️ Cooling
    └── 👨‍💻 People to maintain everything
```

This is expensive and difficult to manage.

---

## With Cloud

Instead:

```text
             Internet
                │
                ↓
       ☁️ Cloud Provider
       ┌─────────────────┐
       │ Servers         │
       │ Storage         │
       │ Databases       │
       │ Networking      │
       └─────────────────┘
                │
                ↓
          Your Application
```

You tell the cloud provider:

> "I need 10 servers."

They provide them.

Later:

> "I need 100 servers."

You can scale up.

And when you don't need them anymore:

> "I only need 5."

You can scale down.

You generally **pay for the resources you use**, rather than buying the entire physical infrastructure yourself.

---

# 🏢 What Does a Cloud Provider Actually Have?

Cloud isn't some magical thing floating in the sky 😄.

There are **real physical data centers**.

```text
                 Cloud Provider
                       │
                 ┌─────┴─────┐
                 ↓           ↓
             Data Center  Data Center
                 │
          ┌──────┼──────┐
          ↓      ↓      ↓
       Servers Servers Servers
          │
       Storage
          │
       Network
```

You access these physical resources through software and APIs.

---

# 🌎 What is a Region?

Cloud providers have data centers in different parts of the world.

For example:

```text
              CLOUD
                │
       ┌────────┼─────────┐
       ↓        ↓         ↓
    Mumbai    London     Tokyo
    Region    Region     Region
```

Each geographic area is called a **Region**.

---

# 🏢 What is an AZ?

Inside a region, there are multiple isolated infrastructure locations called **Availability Zones**.

```text
Mumbai Region
│
├── AZ-1
│    └── Data center(s)
│
├── AZ-2
│    └── Data center(s)
│
└── AZ-3
     └── Data center(s)
```

So now the hierarchy becomes:

```text
Cloud Provider
      ↓
   Region
      ↓
Availability Zones
      ↓
 Physical infrastructure
      ↓
 Servers / Storage / Network
```

---

# 🚀 Why Do Companies Use Cloud?

The biggest advantages are:

### 1. No need to buy all the hardware

You rent infrastructure.

### 2. Easy scaling

```text
10 servers → 100 servers → 1000 servers
```

when demand increases.

### 3. High availability

You can deploy across multiple AZs.

### 4. Global access

You can deploy applications close to users around the world.

### 5. Pay-as-you-go

You generally pay based on what resources you consume.
