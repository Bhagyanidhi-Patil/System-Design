# ACID Property

ACID property is four important rules to keep database transactions safe and reliable. These properties make sure that even if something goes wrong (crash, power failure, etc.), your data stays correct.

## What is ACID?

ACID stands for:

- **A** → Atomicity
- **C** → Consistency
- **I** → Isolation
- **D** → Durability

## 1. Atomicity (All or Nothing)

A transaction either completes fully or doesn't happen at all. If any part fails → the whole transaction is rolled back.

**👉 Think of it like:** Either all changes happen or none happen.

**Example:** Transferring ₹100 from Account A to B:
- Money A से कट गया लेकिन B में नहीं गया ❌ → rollback  
- Either both happen ✔️ or neither happens ✔️
## 2. Consistency (Valid State Only)

The database always moves from one valid state to another valid state. The database must stay correct before and after the transaction.

**👉 Think of it like:** Data follows all the rules at all times.

**Example:** If a column requires unique values, duplicates will never be stored.
## 3. Isolation (No Interference)

Multiple transactions can run at the same time without affecting each other. Each transaction happens as if it's the only one. It's like each transaction runs independently.

**👉 Think of it like:** Transactions are isolated from each other.

**Example:** Two people buying the last item at the same time — only one should get it.
## 4. Durability (Permanent Changes)

Once a transaction is committed, the data is permanently stored, even if the system crashes. The result won't disappear — even if the system crashes.

**👉 Think of it like:** Once saved, data is saved forever.

**Example:** After payment success, the record stays saved — even after power failure.

## Real-World Example: Transferring Money Between Two Accounts
Let’s say:
- Alice wants to send $100 to Bob
- Alice's account has $500
- Bob's account has $300

**Step-by-Step:**
1. Subtract $100 from Alice's account
2. Add $100 to Bob's account

### Applying ACID:

**Atomicity (All or Nothing)**
- Both steps must happen together
- If step 1 works but step 2 fails (e.g., system crashes), Alice loses money and Bob gets nothing — **not allowed!**
- The system will cancel the whole transaction if any part fails

**Consistency (Follow the Rules)**

The system checks rules:
- No negative balances
- Transaction limits
- Valid account numbers

If anything breaks a rule, the transaction won't happen.

**Isolation (No Mix-Ups)**

Suppose two people are sending money at the same time:
- One to Bob, one to Alice

Each transaction is handled separately so they don't interfere with each other.

**Durability (Stays Saved)**

After the transaction finishes:
- Alice has $400
- Bob has $400

Even if the server crashes right after, this change is saved permanently.
