#include <iostream>
#include <iomanip>
#include "MemoryStore.h"
#include "RegisterInfo.h"
#include <fstream>

using namespace std;

const uint32_t NUMREGS = 32;
const uint32_t INSTR_SIZE = 4;

int main(int argc, char** argv) {   

    cout << "helllooo";
    MemoryStore *myMem = createMemoryStore();
    string fileName = argv[1];



    // create bank of registers
    uint32_t regs[NUMREGS] = {0};



    ifstream myFile(fileName, ios::in | ios::binary);

    
    
    if (myFile.is_open()) {

        // set pointer to beginning of file
        // myFile.seekg(0);
        // uint32_t currInstr = 0;
        // uint32_t currAddress = 0;
        // // read files into memory
        // while (!myFile.eof()) {
        //     myFile.read(reinterpret_cast<char*>(&currInstr), INSTR_SIZE);

        //     myMem->setMemValue(currAddress, currInstr, WORD_SIZE);

        //     // move pointer in file
        //     myFile.seekg(INSTR_SIZE, ios::cur);
        //     // increment address in myMem
        //     currAddress += INSTR_SIZE;
        // }

        // get length of file:
        myFile.seekg (0, myFile.end);
        int length = myFile.tellg();
        myFile.seekg (0, myFile.beg);

        // allocate memory:
        char * buffer = new char [length];

        // read data as a block:
        myFile.read (buffer,length);




        // put data in memory
        for (int i = 0; i < length; i ++) {
            
            myMem->setMemValue(i, *(buffer + i), BYTE_SIZE);

        }



        myFile.close();


    } else {
        cout << "Could not read " + fileName;
    }


    uint32_t value = 0;
    myMem->getMemValue(0x00, value, WORD_SIZE);
    cout << "The 32-bit (word) value of address 0x00 is now 0x" << hex << setfill('0') << setw(8) << value << endl;





    return 0;
}
