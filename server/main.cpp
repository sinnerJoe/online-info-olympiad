#include <network/network.hpp>
#include <iostream>
#include <fstream>
using namespace std;

int main(){
    cout << "START MAIN LOOP\n";
    ofstream f{"logs.txt"};
    cout.rdbuf(f.rdbuf());
    mainLoop();
    
    cout << "END MAIN LOOP"<<std::endl;
}