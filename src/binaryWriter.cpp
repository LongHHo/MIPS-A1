#include <fstream>

#include <iostream>
#include <iomanip>
#include "MemoryStore.h"
#include "RegisterInfo.h"

using namespace std;
    


int main() {

    char buffer[4] = {'a', 'b', 'c', 'd'};
    ofstream myFile ("data.bin", ios::out | ios::binary);
    myFile.write (buffer, 4);

}