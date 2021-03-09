#include <iostream>
#include <iomanip>
#include "MemoryStore.h"
#include "RegisterInfo.h"
#include "EndianHelpers.h"
#include <fstream>
#include <stdlib.h>

using namespace std;

const uint32_t NUMREGS = 32;
const uint32_t INSTR_SIZE = 4;

enum INSTR_TYPE
{  
    Halt, 
    Pad,
    R,
    I,
    J
};

// make own function later, from stack overflow
// inclusive from a to b
uint32_t createMask(uint8_t a, uint8_t b, uint32_t instruction)
{

  if (a > b) throw("Oh no");
  
  uint32_t r = 0;

  r = ((1 << (b + 1)) - 1) << a;

  return instruction & r;

}


INSTR_TYPE getType(uint32_t instruction) {

    if (instruction == 0xfeedfeed) {
        return Halt;
    }

    if (instruction == 0) {
        return Pad;
    }
    
    uint32_t opcode = instruction >> 26;


    switch(opcode) {
        case 0:
            return R;
            break;
        case 2:
            return J;
            break;
        case 3:
            return J;
            break;
        default:
            return I;
            break;
    }

}


void dumpRegisterContents(RegisterInfo *reg, uint32_t* regs) {

    reg->at = regs[1];


    for (uint32_t i = 0; i < V_REG_SIZE; i++) {
        reg->v[i] = regs[2 + i];
    }


    for (uint32_t i = 0; i < A_REG_SIZE; i++) {
        reg->a[i] = regs[4 + i];
    }


    for (uint32_t i = 0; i < T_REG_SIZE - 2; i++) {
        reg->t[i] = regs[8 + i];
    }
    reg->t[8] = regs[24];
    reg->t[9] = regs[25];


    for (uint32_t i = 0; i < S_REG_SIZE; i++) {
        reg->s[i] = regs[16 + i];
    }

    for (uint32_t i = 0; i < K_REG_SIZE; i++) {
        reg->k[i] = regs[26 + i];
    }

    reg->gp = regs[28];

    reg->sp = regs[29];

    reg->fp = regs[30];

    reg->ra = regs[31];

}


int main(int argc, char** argv) {   


    MemoryStore *myMem = createMemoryStore();
    if(argv[1] == NULL) { 
      cout << "No argument provided" << endl;
      exit(127);
      }
    string fileName = argv[1];

    // create bank of registers
    uint32_t regs[NUMREGS] = {0};

    // create program counter
    uint32_t pc = 0;

    // create variable to hold current instruction
    uint32_t instruction;

    // read binary file
    ifstream myFile(fileName.c_str(), ios::in | ios::binary);

    if (myFile.is_open()) {

        // get length of file:
        myFile.seekg (0, myFile.end);
        uint32_t byteLength = myFile.tellg();
        uint32_t wordLength = byteLength / 4;
        
        myFile.seekg (0, myFile.beg);

        // allocate memory:
        uint32_t * buffer = new uint32_t [wordLength];

        // read data as a block:
        myFile.read(reinterpret_cast<char*>(buffer), byteLength );


        // put data in memory
        for (int i = 0; i < wordLength; i++) {
            myMem->setMemValue(i*INSTR_SIZE, ConvertWordToBigEndian(buffer[i]), WORD_SIZE);
        }

        myFile.close();


    } else {
        cout << "Could not read\n " + fileName;
        return 0;
    }

    // test contents of memory
    uint32_t val = 0;
    myMem->getMemValue(0x08, val, WORD_SIZE);
    cout << "The 32-bit (word) value of address 0x08 is now 0x" << hex << setfill('0') << setw(8) << val << endl;
    myMem->getMemValue(0x0c, val, WORD_SIZE);
    cout << "The 32-bit (word) value of address 0x0c is now 0x" << hex << setfill('0') << setw(8) << val << endl;
    myMem->getMemValue(0x10, val, WORD_SIZE);
    cout << "The 32-bit (word) value of address 0x10 is now 0x" << hex << setfill('0') << setw(8) << val << endl;
    myMem->getMemValue(0x14, val, WORD_SIZE);
    cout << "The 32-bit (word) value of address 0x14 is now 0x" << hex << setfill('0') << setw(8) << val << endl;


    // main loop
    while (true) {
        myMem->getMemValue(pc, instruction, WORD_SIZE);
        char code = getType(instruction);
	uint32_t jAddress = createMask(0,26, instruction);

	// see which instruction type we retrieve
        switch (code) {
            case Halt:
                cout << "H" << endl;
                RegisterInfo reg;
                dumpRegisterContents(&reg, regs);
                dumpRegisterState(reg); 
                dumpMemoryState(myMem);
                return 0;
            case Pad:
                cout << "Pad" << endl;
                pc += INSTR_SIZE;
                return 0;
                break;
            case R:
                cout << "R" << endl;
                pc += INSTR_SIZE;
                break;
            case J:
	      {
                cout << "J" << endl;
		if (createMask(26,31,instruction) == 3) {
		    cout << "Jump and link" << endl;
                    regs[31] = pc + 8;
                }
		jAddress = jAddress << 2;
		uint32_t pcAddr = instruction & ~0xFFFFFFF;
		pc = pcAddr | jAddress;
                break;
	      }
            case I:
                cout << "I" << endl;
                pc += INSTR_SIZE;
                break;
            default:
                fprintf(stderr,"Illegal operation..."); 
                exit(127);
        }
        

    }






    return 0;
}
