#include<iostream>
#include"zip.hpp"
using namespace std;

int main(int argc, char* argv[]){
 
    if (argc != 3) {
        cout << "Failed to detect Files";
		exit(1);
	}
   
    zip objZip(argv[1], argv[2]);
    objZip.compress();

    cout << "Compressed successfully" << endl;
    
    return 0;
}
