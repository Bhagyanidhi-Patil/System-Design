## File System Design

### Requirements

Support the following operations:

- Create Directory
- Create File
- Write to File
- Read File
- Delete File
- List Files/Directories (ls)

---

### Example:
```
mkdir("/home")

mkdir("/home/user")

addFile("/home/user/a.txt")

write("/home/user/a.txt","Hello")

read("/home/user/a.txt")

ls("/home/user")
```

---

### High Level Design

```
              FileSystem
                   |
                 Root
                   |
               Directory
             /      |      \
        docs     images    temp
         |
      File.txt

```
`Everything is represented as a Node.`


A Node can be
```
File
Directory
```
### Class Diagram

```
                    Node
           -------------------
           - name
           - isFile
           -------------------
                  ▲
         -------------------
         |                 |
      File            Directory
    ---------        -------------------
    content          children
                     map<string,Node*>

                 ▲
                 |
             FileSystem
                 |
               root

```

---

