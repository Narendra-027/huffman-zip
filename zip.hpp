#ifndef ZIP_HPP
#define ZIP_HPP
#include <string>
#include <vector>
#include <queue>
#include <fstream>

using namespace std;

// huffMan tree Node;

struct Node
{
    char chr;
    unsigned freq;
    string code;
    Node *left, *right;

    Node() { left = right = NULL; }
};

class zip{
private:
    vector<Node *> nodeArr;
    fstream inFile, outFile;
    string inFileName, outFileName;

    Node *root;

    class Compare{
    public:
        bool operator()(Node *left, Node *right){
            return left->freq > right->freq;
        }
    };

    //priority queue for storing characters in their decreasing frequency in text file
    priority_queue <Node*, vector<Node*>, Compare> minHeap;

    void createNodeArr();


    void traverseTree(Node* , string);

    string decToBin(int);

    int binToDec(string);

    void createMinHeap();

    // constructing the tree for encoding.
    void createTree();

    void buildTree(char, string&);

    void createCodes();

    void createZip();

    void extractZip();

    void makeTree();

public:
    //constructor
    zip(string inFileName, string outFileName){
        this->inFileName = inFileName;
        this->outFileName = outFileName;
        createNodeArr();
    }

    void compress();

    void extract();
};
#endif
