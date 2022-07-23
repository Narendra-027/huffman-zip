#include "zip.hpp"
#include<iostream>

// function for creating array of nodes.
// each node contain char, its frequency in file and encoded code. 
void zip :: createNodeArr(){
    //there are 128 characters in ASCII
    for(int i = 0; i < 128; i++){
        nodeArr.push_back(new Node());
        nodeArr[i]->chr = i;
        nodeArr[i]->freq = 0;
    }
}

// function for traversing tree which will generate encoded value of charecters
void zip :: traverseTree(Node *root, string code){
    if(root->left == NULL && root->left == NULL){
        root->code = code;
    }

    traverseTree(root->left, code + '0');
    traverseTree(root->right, code + '1');
}


//Function to convert binary string to its equivalent decimal value
int zip :: binToDec(string binStr){
    int res = 0;
    for(int i = 0; i < binStr.size(); i++){
        res = res*2 + binStr[i] - '0';
    }
    return res;
}

//Function to convert a decimal number to its equivalent binary string
string zip :: decToBin(int decNum){
    string temp = "";
    string binary = "";
    while(decNum > 0){
        temp.push_back(decNum%2 + '0');
        decNum /= 2;
    }
    //appending zeros to make 8 bit binary
    binary.append(8 - temp.length(), '0');
    for(int i = temp.length()-1; i >= 0; i--){
        binary.push_back(temp[i]);
    }
    return binary;
}


void zip :: createMinHeap(){
    char chr;
    std :: cout << "opened" << endl;
    inFile.open(inFileName, ios :: in);
    inFile.get(chr);

    // counting the frequency of characters in text file.
    while(!inFile.eof()){
        nodeArr[chr]->freq++;
        inFile.get(chr);
    }
    inFile.close();

    //creating the min heap of the nodes of characters which appeared in text file.
    for(int i = 0; i < 128; i++){
        if(nodeArr[i]->freq > 0){
            minHeap.push(nodeArr[i]);
        }
    }
}

// creating huffman tree from minHeap created above.
void zip :: createTree(){
    Node *leftNode, *rightNode;
    priority_queue <Node*, vector<Node*>, Compare> tempHeap(minHeap); // temporary min heap
    while(tempHeap.size() != 1){
        leftNode = tempHeap.top();
        tempHeap.pop();

        rightNode = tempHeap.top();
        tempHeap.pop();

        root = new Node();
        root->freq = leftNode->freq + rightNode->freq;
        root->left = leftNode;
        root->right = rightNode;

        tempHeap.push(root);
    }
}

void zip :: buildTree(char chr, string &binCode){
    Node *currNode = root;
    for(int i = 0; i < binCode.length(); i++){
        if(binCode[i] == '0'){
            if(currNode->left == NULL){
                currNode->left = new Node();
            }
            currNode = currNode->left;
        }
        else if(binCode[i] == '1'){
            if(currNode->right == NULL){
                currNode->right = new Node();
            }
            currNode = currNode->right;
        }
    }
    currNode->chr = chr;
}

void zip :: createCodes(){
    traverseTree(root, "");
}

void zip :: createZip(){
    inFile.open(inFileName, ios::in);
    outFile.open(outFileName, ios::out | ios::binary);
    string codeMessage = "";
    string s = "";
    char chr;

    //first store the size of the minHeap created from text file.
    codeMessage += (char)minHeap.size();
    priority_queue <Node*, vector<Node*>, Compare> tempHeap(minHeap);
    Node *currNode;
    int codeLength;
    // now we will store (char + char's 128 bit huffman code)*minHeapSize
    // 128 bit is devided into 16 decimal numbers of 8 bit binary.
    // why 16 numbers? because less than 15 numbers will lead to loss in information
    while(!tempHeap.empty()){
        currNode = tempHeap.top();
        codeMessage += currNode->chr;

        codeLength = currNode->code.length();
        s.assign(127 - codeLength, '0');
        s += '1'; // this '1' is like a flag which will tell us during decoding that next to the '1' there is original code
        s += currNode->code;

        codeMessage += (char)binToDec(s.substr(0,8));
        for(int i = 0; i < 15; i++){
            s = s.substr(8);
            codeMessage += (char)binToDec(s.substr(0,8));
        }
        tempHeap.pop();
    }
    s.clear();

    inFile.get(chr);
    while(!inFile.eof()){
        s += nodeArr[chr]->code;
        while(s.length() > 8){
            codeMessage += (char)binToDec(s.substr(0,8));
            s = s.substr(8);
        }
        inFile.get(chr);
    }

    // if remaining bits are less than 8, append the required number of '0' to make it 8 bit
    int count = 8 - s.length();
    if(s.length() < 8){
        s.append(count, '0');
    }

    codeMessage += (char)binToDec(s);
    codeMessage += (char)count;

    outFile.write(codeMessage.c_str(), codeMessage.size());
    inFile.close();
    outFile.close();
}

void zip :: extractZip(){
    inFile.open(inFileName, ios::in | ios::binary);
    outFile.open(outFileName, ios::out);
    // reading the size of minHeap at the begining of file
    unsigned char size;
    inFile.read(reinterpret_cast<char*>(&size), 1);

    inFile.seekg(-1, ios::end);
    char count0;
    inFile.read(&count0, 1);

    //Ignoring the meta data (huffman tree) (1 + 17 * size) and reading remaining file
    inFile.seekg(1 + 17 * size, ios::beg);

    vector<unsigned char> text;
    unsigned char textSeg;
    inFile.read(reinterpret_cast<char*>(&textSeg), 1);
    while(!inFile.eof()){
        text.push_back(textSeg);
        inFile.read(reinterpret_cast<char*>(&textSeg), 1);
    }

    Node *currNode = root;
    string path;
    for(int i = 0; i < text.size()-1; i++){
        path = decToBin(text[i]);
        if(i == text.size()-2){
            path = path.substr(0, 8-count0);
        }

        for(int j = 0; j < path.size(); j++){
            if(path[j] == '0'){
                currNode = currNode->left;
            }
            else{
                currNode = currNode->right;
            }

            if(currNode->left == NULL && currNode->right == NULL){
                outFile.put(currNode->chr);
                currNode = root;
            }
        }
    }
    inFile.close();
    outFile.close();
}


void zip :: makeTree(){
    inFile.open(inFileName, ios::in | ios::binary);
    //Reading size of MinHeap
    unsigned char size;
    inFile.read(reinterpret_cast<char*>(&size), 1);
    root = new Node();

    for(int i = 0; i < size; i++){
        char chr;
        unsigned char decCode[16];
        inFile.read(&chr, 1);
        inFile.read(reinterpret_cast<char*>(decCode), 16);

        string binCode = "";
        for(int j = 0; j < 16; j++){
            binCode += decToBin(decCode[j]);
        }

        int k = 0;
        while(binCode[k] == 0){
            k++;
        }

        binCode = binCode.substr(k+1);
        buildTree(chr, binCode);
    }
    inFile.close();
}

void zip :: compress(){
    std :: cout << "done1" << endl;
    createMinHeap();
    createTree();
    std :: cout << "done2" << endl;
    createCodes();
    std :: cout << "done3" << endl;
    createZip();
    std :: cout << "done4" << endl;
}

void zip :: extract(){
    makeTree();
    extractZip();
}
