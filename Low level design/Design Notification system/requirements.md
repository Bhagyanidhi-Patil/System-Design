### Problem Statement

`Design a Notification System that can send notifications through multiple channels.`

**Initially support:**

- Email
- SMS
- Push Notification

**Future:**

- WhatsApp
- Slack
- Teams
- Telegram
- Discord

`The interviewer usually asks:`

"Tomorrow if I ask you to add WhatsApp notifications, how much code will change?"

`Your answer should be:`

"Only create one new class. Existing code remains unchanged."
That is the goal.

---

### Requirements

`Functional`

- Send Email
- Send SMS
- Send Push Notification

`Non Functional`

- Easy to add new notification channels
- Low coupling
- High cohesion
- Reusable
- Thread safe
- SOLID principles

---

### Design Patterns Used
```
| Pattern               | Why                                             |
| --------------------- | ----------------------------------------------- |
| Strategy Pattern      | Every notification has different implementation |
| Factory Pattern       | Creates correct notifier                        |
| Open Closed Principle | Add new notifier without changing code          |
| Dependency Inversion  | Client depends on interface                     |
```

---

### Class diagram
```
                    +----------------+
                    | INotification  |
                    +----------------+
                           ^
                           |
       ----------------------------------------
       |                 |                  |
       |                 |                  |
+-------------+   +-------------+   +---------------+
| Email       |   | SMS         |   | Push          |
+-------------+   +-------------+   +---------------+

                     ^
                     |
             NotificationFactory

                     ^
                     |
              NotificationService

                     ^
                     |
                   Client
```
---
