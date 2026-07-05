#include<iostream>
#include<vector>
#include<unordered_map>
#include<sstream>

using namespace std;

class Node{
public:
    string name;
    bool isFile;
    string content;
    unordered_map<string,Node*>children;

    Node(string name,bool isFile){
        this->name = name;
        this->isFile = isFile;
    }
    virtual ~Node(){}
};

class FileSystem{
private:
    Node* root;

    vector<string>split(string path){
        vector<string>parts;
        string temp;
        stringstream ss(path);

        while(getline(ss,temp,'/')){
            if(!temp.empty()){
                parts.push_back(temp);
            }
        }
        return parts;
    }
public:
    FileSystem(){
        root = new Node("/",false);
    }

    void mkdir(string path){
        vector<string>parts = split(path);
        Node* curr = root;
        for(string dir:parts){
            if(curr->children.count(dir)==0){
                curr->children[dir] = new Node(dir,false);
            }
            curr = curr->children[dir];
        }
    }

    void addFile(string path){
        vector<string>parts = split(path);
        Node* curr = root;

        for(int i=0;i<parts.size()-1;i++){
            if(curr->children.count(parts[i])==0){
                curr->children[parts[i]] = new Node(parts[i],false);
            }
            curr = curr->children[parts[i]];
        }
        string fileName = parts.back();
        curr->children[fileName] = new Node(fileName,true);
    }

    void writeFile(string path,string content){
        vector<string>parts = split(path);
        Node* curr = root;
        for(int i=0;i<parts.size()-1;i++){
            curr = curr->children[parts[i]];
        }
        curr->children[parts.back()]->content = content;
    }

    string readFile(string path){
        vector<string>parts = split(path);
        Node* curr = root;
        for(int i=0;i<parts.size()-1;i++){
            curr = curr->children[parts[i]];
        }
        return curr->children[parts.back()]->content;
    }

    void ls(string path){
        vector<string>parts = split(path);
        Node* curr = root;

        for(string dir:parts){
            curr = curr->children[dir];
        }
        cout<<"\n Contents of "<<path<<":\n";

        for(auto child:curr->children){
            if(child.second->isFile){
                cout<<"File : ";
            }
            else{
                cout<<"Dir : ";
            }
            cout<<child.first<<endl;
        }
    }

    void deleteNode(string path){
        vector<string>parts = split(path);
        Node* curr = root;
        for(int i=0;i<parts.size()-1;i++){
            curr = curr->children[parts[i]];
        }
        string name = parts.back();
        if(curr->children.count(name)){
            delete curr->children[name];
            curr->children.erase(name);
        }
    }
};

int main()
{
    FileSystem fs;

    // Create Directories
    fs.mkdir("/home");
    fs.mkdir("/home/user");
    fs.mkdir("/home/user/docs");

    // Create Files
    fs.addFile("/home/user/docs/file1.txt");
    fs.addFile("/home/user/docs/file2.txt");

    // Write Data
    fs.writeFile("/home/user/docs/file1.txt", "Hello World!");
    fs.writeFile("/home/user/docs/file2.txt", "Low Level Design");

    // Read Data
    cout << "Reading file1.txt : "
         << fs.readFile("/home/user/docs/file1.txt") << endl;

    cout << "Reading file2.txt : "
         << fs.readFile("/home/user/docs/file2.txt") << endl;

    // List Contents
    fs.ls("/home");
    fs.ls("/home/user");
    fs.ls("/home/user/docs");

    // Delete File
    fs.deleteNode("/home/user/docs/file1.txt");

    cout << "\nAfter deleting file1.txt\n";

    fs.ls("/home/user/docs");

    return 0;
}