### Problem Statement

`Design a Read-Write Lock` where

- Multiple readers can read simultaneously.
- Only one writer can write.
- Writer must get exclusive access.
- No reader can read while a writer is writing.

---

#### Example
```
Reader1  -----> Reading
Reader2  -----> Reading
Reader3  -----> Reading

(All three allowed together)

----------------------------

Writer1 -----> Writing

(No reader or writer allowed)

----------------------------

Reader1
Writer1

Reader waits OR Writer waits depending on policy.
```

---

#### Real World Examples

`Database`
```
100 users reading product details

Only 1 admin updates product price.
```
`Operating System`
```
Many threads reading cache.

One thread updates cache.
```
`File System`
```
Many applications reading file.

One process writing file.
```

---