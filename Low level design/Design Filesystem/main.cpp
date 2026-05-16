#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>

using namespace std;

struct Node{
    bool isFile;
    string content;
    unordered_map<string,Node*>children;

    Node(){
        isFile = false;
    }
};

class FileSystem{
private:
    Node* root;

    //split path into parts
    vector<string>split(const string& path){
        vector<string>result;
        stringstream ss(path);
        string part;

        while(getline(ss,part,'/')){
            if(!part.empty()){
                result.push_back(part);
            }
        }
        return result;
    }

public:
    FileSystem(){
        root = new Node();
    }

    // create directory path
    void mkdir(string path){
        Node* curr = root;
        vector<string>parts = split(path);

        for(auto &dir:parts){
            if(curr->children.count(dir)==0){
                curr->children[dir] = new Node();
            }
            curr = curr->children[dir];
        }
    }

    // add content to file
    void addContentToFile(string filepath,string content){
        Node* curr =  root;
        vector<string>parts = split(filepath);

        for(int i=0;i<parts.size();i++){
            string name = parts[i];
            if(curr->children.count(name)==0){
                curr->children[name] = new Node();
            }
            curr = curr->children[name];
        }
        curr->isFile = true;
        curr->content+=content;
    }

    // read file
    string readContentFromFile(string filepath){
        Node* curr = root;
        vector<string>parts = split(filepath);

        for(auto &name : parts){
            curr = curr->children[name];
        }
        return curr->content;
    }
};

int main() {

    FileSystem fs;

    fs.mkdir("/a/b/c");

    fs.addContentToFile("/a/b/c.txt", "Hello ");
    fs.addContentToFile("/a/b/c.txt", "World");

    cout << fs.readContentFromFile("/a/b/c.txt") << endl;

    return 0;
}

/*
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