#include<iostream>
#include"zip.hpp"
using namespace std;


int main(){
    string inputFileName, outputFileName;
    cin >> inputFileName;
    cin >> outputFileName;
    
    zip objZip(inputFileName, outputFileName);
    objZip.compress();

    cout << "Compressed successfully" << endl;
    
    return 0;
}
