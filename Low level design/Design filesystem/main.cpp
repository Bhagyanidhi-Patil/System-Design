#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

using namespace std;

enum NodeType{
    FILE_NODE,
    DIR_NODE
};

class Node{
public:
    string name;
    NodeType type;

    //only for files
    string content;
    //only for dir
    unordered_map<string,Node*>children;

    Node(string name,NodeType type)
        :name(name),type(type){}
    
    ~Node(){
        for(auto &child:children){
            delete child.second;
        }
    }
};

class FileSystem{
private:
    Node* root;
    
    vector<string>split(const string& path){
        stringstream ss(path);
        vector<string>parts;
        string temp;

        while(getline(ss,temp,'/')){
            if(!temp.empty())
                parts.push_back(temp);
        }
        return parts;
    }
public:
    FileSystem(){
        root = new Node("/",NodeType::DIR_NODE);
    }

    ~FileSystem(){
        delete root;
    }

    void mkdir(string path){
        vector<string>parts = split(path);
        Node* curr = root;

        for(string dir:parts){
            if(curr->children.count(dir)==0){
                curr->children[dir] = new Node(dir,NodeType::DIR_NODE);
            }
            curr = curr->children[dir];
        }
    }

    bool addFile(string path){
        vector<string>parts = split(path);

        if(parts.empty()){
            return false;
        }
        Node* curr = root;
        for(int i=0;i<parts.size()-1;i++){
            if(curr->children.count(parts[i])==0)
                return false;
            curr = curr->children[parts[i]];
            // Parent must be dir_node
            if(curr->type==NodeType::FILE_NODE)
                return false;
        }
        string filename = parts.back();
        // FILE_NODE already exists
        if(curr->children.count(filename)!=0){
            return false;
        }
        curr->children[filename] = new Node(filename,NodeType::FILE_NODE);
        return true;
    }

    bool writeFile(string path,string content){
        vector<string>parts = split(path);
        if(parts.empty()){
            return false;
        }
        Node* curr = root;
        for(int i=0;i<parts.size()-1;i++){
            if(curr->children.count(parts[i])==0){
                return false;
            }
            curr = curr->children[parts[i]];
            if(curr->type==NodeType::FILE_NODE)
                return false;
        }
        string filename = parts.back();
        if(curr->children.count(filename)==0)return false;

        Node* file_node = curr->children[filename];
        if(file_node->type == NodeType::DIR_NODE){
            return false;
        }
        file_node->content = content;
        return true;
    }

    string readFile(string path){
        vector<string>parts = split(path);
        if(parts.empty())return "";

        Node* curr = root;
        for(int i=0;i<parts.size()-1;i++){
            if(curr->children.count(parts[i])==0)
                return "";
            curr = curr->children[parts[i]];
            if(curr->type==NodeType::FILE_NODE)
                return "";
        }
        string filename = parts.back();
        if(curr->children.count(filename)==0)
            return "";
        Node* file_node = curr->children[filename];
        if(file_node->type==NodeType::DIR_NODE){
            return "";
        }
        return file_node->content;
    }

    void ls(string path){
        vector<string>parts = split(path);
        Node* curr = root;
        for(string dir:parts){
            if(curr->children.count(dir)==0){
                cout<<"Path doesn't exists"<<endl;
                return;
            }
            curr = curr->children[dir];
        }
        if(curr->type == NodeType::FILE_NODE){
            cout<<"Path is a file_node"<<endl;
            return ;
        }

        cout<<"\n Contents of : "<<curr->name<<endl;
        for(auto& child:curr->children){
            if(child.second->type==NodeType::FILE_NODE){
                cout<<"FILE_NODE : ";
            }
            else{
                cout<<"Dir : ";
            }
            cout<<child.first<<endl;
        }
    }

    bool deleteNode(string path){
        vector<string>parts = split(path);
        if(parts.empty()){
            return false;
        }
        Node* curr = root;
        for(int i=0;i<parts.size()-1;i++){
            if(curr->children.count(parts[i])==0){
                return false;
            }
            curr = curr->children[parts[i]];
        }
        string name = parts.back();
        if(curr->children.count(name)==0){
            return false;
        }
        delete curr->children[name];   // delete node
        curr->children.erase(name);    // delete entry from map
        return true;
    }
};

int main() {

    FileSystem fs;

    // Create directories
    fs.mkdir("/home");
    fs.mkdir("/home/user");
    fs.mkdir("/home/user/docs");

    // Create files
    fs.addFile("/home/user/docs/file1.txt");
    fs.addFile("/home/user/docs/file2.txt");

    // Write data
    fs.writeFile(
        "/home/user/docs/file1.txt",
        "Hello World!"
    );

    fs.writeFile(
        "/home/user/docs/file2.txt",
        "Low Level Design"
    );

    // Read data
    cout << "Reading file1.txt : "
         << fs.readFile("/home/user/docs/file1.txt")
         << endl;

    cout << "Reading file2.txt : "
         << fs.readFile("/home/user/docs/file2.txt")
         << endl;

    // List contents
    fs.ls("/home");
    fs.ls("/home/user");
    fs.ls("/home/user/docs");

    // Delete file_node
    fs.deleteNode("/home/user/docs/file1.txt");

    cout << "\nAfter deleting file1.txt\n";

    fs.ls("/home/user/docs");

    return 0;
}