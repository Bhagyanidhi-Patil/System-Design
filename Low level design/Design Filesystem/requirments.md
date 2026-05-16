# What is a File System?

A File System is a hierarchical structure used to:

store files  
store directories (folders)  
allow navigation using paths like:  
/a/b/c.txt  
/home/user/docs  

---

# What it should support

We design a basic in-memory file system.

## 📌 Requirements

## 1. Create Path
mkdir /a/b/c  
creates directories recursively  
parent must exist  

## 2. Add Content to File
addContentToFile("/a/b/c.txt", "hello")  
create file if not exists  
append content if exists  

## 3. Read File
readContentFromFile("/a/b/c.txt")  
returns full file contentDesign Idea  

We model file system as a tree (Trie-like structure):

Each node = directory or file

Node structure contains:  
children (subdirectories/files)  
isFile flag  
content string (for files)  

---

# 🏗️ Key Design

Root  
 ├── a  
 │    └── b  
 │         └── c.txt (file)  

---

# 🧠 What is a Trie?

A Trie is:

A tree where each node represents a character, and paths from root represent words or prefixes.

---

# 🌳 Example

If we insert these words:

cat  
car  
card  
dog  

Trie looks like:

(root)  
 ├── c  
 │    ├── a  
 │    │    ├── t (end)  
 │    │    └── r  
 │    │         └── d (end)  
 └── d  
      └── o  
           └── g (end)  

---

# Key idea

Instead of storing full strings repeatedly, we share prefixes.

So:

"car" and "card" share "car"  
"cat" shares "ca"