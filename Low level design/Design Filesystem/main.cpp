#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>

using namespace std;

struct Node {

    bool isFile;
    bool isDirectory;

    string content;

    unordered_map<string, Node*> children;

    Node() {

        isFile = false;
        isDirectory = false;
    }
};

class FileSystem {

private:

    Node* root;

    // Split path into parts
    vector<string> split(const string& path) {

        vector<string> result;

        stringstream ss(path);

        string part;

        while (getline(ss, part, '/')) {

            if (!part.empty()) {
                result.push_back(part);
            }
        }

        return result;
    }

public:

    FileSystem() {

        root = new Node();

        root->isDirectory = true;
    }

    // Create directory path
    void mkdir(string path) {

        Node* curr = root;

        vector<string> parts = split(path);

        for (auto &dir : parts) {

            // Cannot go inside file
            if (curr->isFile) {

                cout << "Cannot create directory inside file\n";

                return;
            }

            // Create directory if not exists
            if (curr->children.count(dir) == 0) {

                curr->children[dir] = new Node();

                curr->children[dir]->isDirectory = true;
            }

            curr = curr->children[dir];
        }
    }

    // Add content to file
    void addContentToFile(string filepath,
                          string content) {

        Node* curr = root;

        vector<string> parts = split(filepath);

        for (int i = 0; i < parts.size(); i++) {

            string name = parts[i];

            // Cannot traverse inside file
            if (curr->isFile) {

                cout << "Cannot create inside file\n";

                return;
            }

            // Create node if not exists
            if (curr->children.count(name) == 0) {

                curr->children[name] = new Node();
            }

            curr = curr->children[name];
        }

        // Directory cannot become file
        if (curr->isDirectory) {

            cout << "Cannot add content to directory\n";

            return;
        }

        curr->isFile = true;

        curr->content += content;
    }

    // Read file content
    string readContentFromFile(string filepath) {

        Node* curr = root;

        vector<string> parts = split(filepath);

        for (auto &name : parts) {

            // File path does not exist
            if (curr->children.count(name) == 0) {

                return "File not found";
            }

            curr = curr->children[name];
        }

        // Not a file
        if (!curr->isFile) {

            return "Path is not a file";
        }

        return curr->content;
    }
};

int main() {

    FileSystem fs;

    fs.mkdir("/a/b/c");

    fs.addContentToFile("/a/b/c", "Hello ");
    fs.addContentToFile("/a/b/c.txt", "World");

    cout << fs.readContentFromFile("/a/b/c.txt") << endl;

    return 0;
}

/*
each node can represent either:
a directory
OR
a file

isFile == false
👉 node behaves like a directory

isFile == true
👉 node behaves like a file

Internally
Directory node (c)
    isFile = false
    children = {...}
    content = ""

File node (c.txt)
    isFile = true
    content = "Hello World"
    children = {}
----------------------------------------------------------------------------------------------------------------------------

func : vector<string>split(const string& path){}
Split a file path like: /a/b/c.txt into individual parts: ["a", "b", "c.txt"]

path = "/a/b/c.txt"

stringstream ss(path); 
ss contains: /a/b/c.txt , Now we can read it piece by piece.

getline(ss, part, '/')
Reads characters until delimiter '/'.

while(getline(ss, part, '/')) , the variable: part stores one token at a time from the stream ss, separated by '/'.

----------------------------------------------------------------------------------------------------------------------------

func : void mkdir(string path){}

It creates a directory path like: /a/b/c even if intermediate folders don’t exist.
So it ensures the structure: root → a → b → c

Node* curr = root; We always begin from the root of the file system.
vector<string> parts = split(path); /a/b/c becomes: ["a", "b", "c"]
Traverse each directory, We go one by one: "a", "b" ,"c"
Check if directory exists , If "dir" does NOT exist as a child → create it.
If current node has: root → a exists, then: skip creating "a" , move forward
curr->children[dir] = new Node(); This creates a new folder.
    So: curr → dir , is added to children map.
curr = curr->children[dir]; Move the pointer curr to the child node named dir

--------------------------------------------------------------------------------------------------------------------------------

fs.mkdir("/a/b/c");
fs.addContentToFile("/a/b/c.txt", "Hello ");

Are we turning c into a file?c is still a directory. But c.txt is a file inside c’s parent directory (b), not inside c.
mkdir /a/b/c 
Creates:
root
 └── a
      └── b
           └── c   (directory)

add file /a/b/c.txt

Final structure
root
 └── a
      └── b
           ├── c        (directory)
           └── c.txt    (file)
*/