# Abstraction

`Abstraction` means `hiding the implementation details` and `showing only the essential functionality` to the user.

The user knows what to do, but doesn't need to know how it is done.

## WhatsApp Example

- Imagine you want to send a message on WhatsApp.

`You simply do`:
```
Open Chat

↓

Type Message

↓

Click Send
```
That's all you know.

### What happens internally?

`When you press Send, WhatsApp performs many operations behind the scenes`:
```
Check Internet Connection

↓

Encrypt the Message (End-to-End Encryption)

↓

Connect to WhatsApp Server

↓

Find Receiver's Device

↓

Transfer the Encrypted Message

↓

Decrypt on Receiver's Device

↓

Show Double Tick / Blue Tick
```
- As a user, you don't know or care about these implementation details.

- You simply press Send and expect the message to be delivered.

- This is `abstraction`.

## Abstraction vs Abstract Class

| Abstraction | Abstract Class |
|-------------|----------------|
| An OOP concept/principle | A C++ language feature used to implement abstraction |
| Hides implementation details | Contains one or more pure virtual functions |
| Tells what should be done | Helps define how derived classes must implement it |
| Can be achieved using abstract classes, interfaces, etc. | One of the ways to achieve abstraction in C++ |