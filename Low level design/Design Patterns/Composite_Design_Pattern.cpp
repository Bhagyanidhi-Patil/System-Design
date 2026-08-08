/*
The Composite Design Pattern is a structural pattern that lets you treat individual objects and groups of objects uniformly. 
It’s especially useful when you’re working with tree-like structures (like file systems, UI elements, or organizational hierarchies).
----
Real-Life Example: File System 📁

Think about your computer.

Folder
├── File1.txt
├── File2.txt
└── Photos
      ├── img1.jpg
      └── img2.jpg

Here,

File → Leaf node
Folder → Composite node

When you click Delete on a folder,

It deletes all its files.
It deletes all its subfolders.

Notice that Delete works for both:

A single file
A folder containing hundreds of files

This is exactly what Composite provides.
----
Block Diagram
                     Component
                  +-------------+
                  | operation() |
                  +-------------+
                        ^
             -------------------------
             |                       |
             |                       |
         Leaf(File)            Composite(Folder)
         +----------+          +------------------+
         |operation()|         | vector<Component*>|
         +----------+          | add()            |
                               | remove()         |
                               | operation()      |
                               +------------------+
                                        |
                               -------------------
                               |        |        |
                              File     File    Folder



When to Use Composite?

Use it when you have a tree structure:

📁 File System
🌳 Organization Hierarchy
📂 Menu/Submenu
🛒 Categories/Subcategories
🎨 Graphics (Circle + Group of Shapes)
🌐 HTML/XML DOM Tree

*/

#include <iostream>
#include <vector>
using namespace std;

// Component
class FileSystem {
public:
    virtual void show() = 0;
    virtual void add(FileSystem* obj) {
        // Default: do nothing
    }
    virtual ~FileSystem() {}
};

// Leaf
class File : public FileSystem {
    string name;

public:
    File(string n) : name(n) {}

    void show() override {
        cout << "File: " << name << endl;
    }
};

// Composite
class Folder : public FileSystem {
    string name;
    vector<FileSystem*> children;

public:
    Folder(string n) : name(n) {}

    void add(FileSystem* obj) override{
        children.push_back(obj);
    }

    void show() override {
        cout << "Folder: " << name << endl;

        for (auto child : children)
            child->show();
    }
};

int main() {

    FileSystem* file1 = new File("resume.pdf");
    FileSystem* file2 = new File("photo.jpg");

    FileSystem* photos = new Folder("Photos");
    photos->add(file2);

    FileSystem* documents = new Folder("Documents");
    documents->add(file1);
    documents->add(photos);

    FileSystem* root = new Folder("Root");
    root->add(documents);

    root->show();

    delete root;   // (Assuming Folder destructor deletes its children)
}
//Because both individual objects (File) and groups of objects (Folder) implement the same FileSystem interface. 
//The client treats both uniformly through FileSystem*, and since a Folder can contain other FileSystem objects (files or folders), 
//it forms a tree structure.