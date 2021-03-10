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

  if (a > b) throw("a must be less than b\n");
  
  uint32_t r = 0;

  uint32_t aMask = ((1 << a) - 1);
  uint32_t bMask = ((1 << b + 1) - 1);

  if (b == 31) {
    bMask = 0xFFFFFFFF;
  }

  r = bMask ^ aMask;

  return (instruction & r) >> a;

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

void rHelper(uint32_t instruction, uint32_t* pc, uint32_t* regs) {


    // get funct code
  uint32_t funct = createMask(0, 5, instruction);

  cout << funct << " is the funct code" << endl;

    switch(funct){
      // add signed
        case 0x20:
	  {
	  uint32_t rd = createMask(11, 15, instruction);
	  uint32_t rt = createMask(16, 20, instruction);
	  uint32_t rs = createMask(21, 25, instruction);

	  int op1 = regs[rs];
	  int op2 = regs[rt];
	  
	  regs[rd] = op1 + op2;
	 
	  *pc = *pc + 4;
	  break;
	  }
       // add unsigned
       case 0x21:
        {
	uint32_t rd = createMask(11, 15, instruction);
	uint32_t rt = createMask(16, 20, instruction);
	uint32_t rs = createMask(21, 25, instruction);

	uint32_t op1 = regs[rs];
	uint32_t op2 = regs[rt];
	  
        regs[rd] = op1 + op2;
	
	*pc = *pc + 4;
	break;
      }
      // and
    case 0x24:
      {
	uint32_t rd = createMask(11, 15, instruction);
	uint32_t rt = createMask(16, 20, instruction);
	uint32_t rs = createMask(21, 25, instruction);

	uint32_t op1 = regs[rs];
	uint32_t op2 = regs[rt];
	  
        regs[rd] = op1 & op2;
	
	*pc = *pc + 4;
	break;
      }
      // jr
    case 0x08:
      {
	uint32_t rs = createMask(21,25,instruction);
	*pc = regs[rs];

	break;
      }
      // nor
    case 0x27:
      {
	uint32_t rd = createMask(11, 15, instruction);
	uint32_t rt = createMask(16, 20, instruction);
	uint32_t rs = createMask(21, 25, instruction);

	uint32_t op1 = regs[rs];
	uint32_t op2 = regs[rt];
	  
        regs[rd] = ~(op1 | op2);
	*pc = *pc + 4;

	break;
      }
       // or
    case 0x25:
      {
	uint32_t rd = createMask(11, 15, instruction);
	uint32_t rt = createMask(16, 20, instruction);
	uint32_t rs = createMask(21, 25, instruction);

	uint32_t op1 = regs[rs];
	uint32_t op2 = regs[rt];
	  
        regs[rd] = (op1 | op2);
	*pc = *pc + 4;

	break;
      }
      // slt
      case 0x2a:
      {
	uint32_t rd = createMask(11, 15, instruction);
	uint32_t rt = createMask(16, 20, instruction);
	uint32_t rs = createMask(21, 25, instruction);

	int32_t op1 = regs[rs];
	int32_t op2 = regs[rt];
	  
        regs[rd] = (op1<op2)?1:0;
	*pc = *pc + 4;

	break;
      }
      // sltu
      case 0x2b:
      {
	uint32_t rd = createMask(11, 15, instruction);
	uint32_t rt = createMask(16, 20, instruction);
	uint32_t rs = createMask(21, 25, instruction);

	uint32_t op1 = regs[rs];
	uint32_t op2 = regs[rt];
	  
        regs[rd] = (op1<op2)?1:0;
	*pc = *pc + 4;

	break;
      }
      // sll
    case 0x00:
      {
	uint32_t rd = createMask(11, 15, instruction);
	uint32_t rt = createMask(16, 20, instruction);
	uint32_t shamt = createMask(6,10,instruction);

	uint32_t op1 = regs[rt];
	  
        regs[rd] = op1<<shamt;
	*pc = *pc + 4;

	break;
      }
      // srl
    case 0x02:
      {
	uint32_t rd = createMask(11, 15, instruction);
	uint32_t rt = createMask(16, 20, instruction);
	uint32_t shamt = createMask(6,10,instruction);

	uint32_t op1 = regs[rt];
	  
        regs[rd] = op1>>shamt;
	*pc = *pc + 4;

	break;
      }
      
      

      
        default:
	  exit(127);
    }

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
    regs[9] = 1;

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


    // main loop
    while (true) {
        myMem->getMemValue(pc, instruction, WORD_SIZE);
        char code = getType(instruction);
	uint32_t jAddress = createMask(0,25, instruction);

	// see which instruction type we retrieve
        switch (code) {
            case Halt:
                cout << "Halt" << endl;
                RegisterInfo reg;
                dumpRegisterContents(&reg, regs);
                dumpRegisterState(reg); 
                dumpMemoryState(myMem);
                return 0;
            case Pad:
                cout << "Pad" << endl;
                pc += INSTR_SIZE;
                break;
            case R:
                cout << "R" << endl;
		rHelper(instruction, &pc, regs);
                break;
            case J:
	      {
                cout << "J" << endl;
                if (createMask(26,31,instruction) == 3) {
                    cout << "Jump and link" << endl;
                            regs[31] = pc + 8;
                        }
                jAddress = jAddress << 2;
                uint32_t pcAddr = pc & ~0xFFFFFFF;
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
