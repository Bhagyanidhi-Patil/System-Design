### Stringstream 
`path = "/home/user/docs"`

**stringstream ss(path);**

Internally
```
"/home/user/docs"
 ^
```
The pointer starts at the beginning.

`getline(ss, temp, '/');`

**getline reads characters until it finds '/'.**

The first character is '/'.

So
```
temp = ""

Pointer moves

"/home/user/docs"
 ^
```
to
```
"/home/user/docs"
  ^
```
Since , if(!temp.empty()) -> is false, we ignore it.

#### Next Step

Again `getline(ss,temp,'/');`

Now it reads

home

until the next /.

So , **temp = "home"**

Pointer becomes
```
/home/user/docs
      ^
```
Similary, adds user and docs my moving the pointers.

Vector
```
home
user
docs
```
End of string reached.

**Final Result**
```
parts =
{
    "home",
    "user",
    "docs"
}
```

---

## Flow :

#### Step 1
`Every file and directory is represented by a Node.`

A node stores
```
name        -> Name of file/folder
isFile      -> true if file
content     -> Stores data (only for files)
children    -> Stores child files/folders (only for directories)
```

**Example**

`Directory`

docs
```
name = docs
isFile = false
content = ""
children = { }
```
`File`

file1.txt
```
name = file1.txt
isFile = true
content = "Hello"
children = { }      // Not used
```
---
#### Step 2
Constructor
```
FileSystem()
{
    root = new Node("/", false);
}
```
Initially
```
root

/

children = { }
```
Nothing exists.

---
#### Step 3

Suppose
```
mkdir("/home/user/docs");
```
The function `split(path)` does
```
"/home/user/docs"

↓

["home","user","docs"]
```
Implementation
```
stringstream ss(path);

while(getline(ss,temp,'/'))
```
Every word between '/' becomes one string.

---
#### Step 4 - mkdir

Suppose

`fs.mkdir("/home/user/docs");`

After split

parts
```
home
user
docs
```
Current starts from
```
curr

↓

/
```

##### Iteration 1
`dir = home`

Check
`curr->children.count("home")`
```
Current tree

/
```
No "home"

Create it
```
/

|
+---- home
```
Move current
```
curr

↓

home
```

##### Iteration 2

Now

`dir = user`

Inside home
```
home

children = { }
```

Create
```
/

|
+---- home

       |

       +---- user
```
Move
```
curr

↓

user
```

##### Iteration 3
`dir = docs`

Create
```
/

|
+---- home

       |

       +---- user

               |

               +---- docs
```
Done.

##### Final Tree
```
/

|
+---- home

       |

       +---- user

               |

               +---- docs
```

---
#### Step 5 - addFile()
`fs.addFile("/home/user/docs/file1.txt");`

Split
```
home
user
docs
file1.txt
```
Notice ,Last one is file.

Loop runs till
```
home

↓

user

↓

docs
```
Current becomes `docs`

Now

`string fileName = parts.back();`
```
fileName

↓

file1.txt
```
Create

`new Node(fileName,true)`

Tree becomes
```
docs

|

+---- file1.txt
```

---
#### Step 6 - writeFile()

`writeFile("/home/user/docs/file1.txt", "Hello World");`

Traverse
```
/

↓

home

↓

user

↓

docs
```
Then

`children["file1.txt"]`

Gets
```
Node

name=file1.txt

isFile=true

content=""
```
Now

`content = "Hello World"`

Node becomes
```
file1.txt

content="Hello World"
```

---
#### Step 7 - readFile()

Again

Traverse
```
/

↓

home

↓

user

↓

docs
```
Find

`file1.txt`

Return
```
content

↓

Hello World
```
Output

`Hello World`

---
#### Step 8 : ls()

Suppose `ls("/home/user/docs");`

Traverse
```
/

↓

home

↓

user

↓

docs
```
Current points to
```
docs

Now
```

`for(auto child:curr->children)`

Children contains
```
file1.txt

file2.txt
```

Print
```
File : file1.txt

File : file2.txt
```

---
#### Step 9 : deleteNode()

`deleteNode("/home/user/docs/file1.txt");`

Traverse
```
/

↓

home

↓

user

↓

docs
```
Current
```
docs
```
Now
```
children.erase(file1.txt)
```
Tree Before
```
docs

|

+ file1.txt

|

+ file2.txt
```
After
```
docs

|

+ file2.txt
```

---