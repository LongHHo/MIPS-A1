//GID:436
#include <iostream>
#include <iomanip>
#include "MemoryStore.h"
#include "RegisterInfo.h"
#include "EndianHelpers.h"
#include <fstream>
#include <stdlib.h>

using namespace std;

// function declaration to execute an instruction
int executeInstruction(uint32_t* pc, uint32_t* regs, MemoryStore *myMem);

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


// Extract bits from word inclusive from bit a to b
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

// Sign extend bits 15-31 with the 15th bit of the instruction
int32_t signExtendedImm(uint32_t instruction) {

    int32_t first16 = (0x0000FFFF & instruction);

    // check if 16th bit is 1, if so sign extend
    if (0x00008000 & first16) {
        return (0xFFFF0000 + first16);
    } else {
        return first16;
    }
}

// Given an instruction, returns the instruction type
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

// Dumps contents in array regs into RegisterInfo rerg
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

// Helper function to decode and execute the I type instruction
void iHelper(uint32_t instruction, uint32_t* pc, uint32_t* regs, MemoryStore *myMem) {

    uint32_t rt = createMask(16, 20, instruction);
    uint32_t rs = createMask(21, 25, instruction);

    uint32_t opcode = instruction >> 26;

    switch (opcode) {
        // addi
        case 0x08:
        {
            int32_t sourceNum = regs[rs]; 
            int32_t imm = signExtendedImm(instruction);

            int32_t result = sourceNum + imm;
            
            // check for overflow
            if ((sourceNum > 0 && imm > 0 && result < 0) || (sourceNum < 0 && imm < 0 && result > 0)) {
                RegisterInfo reg;
                dumpRegisterContents(&reg, regs);
                dumpRegisterState(reg); 
                dumpMemoryState(myMem);
                fprintf(stderr,"Arithmetic Overflow \n"); 
                exit(12);
            }
            regs[rt] = result;
            break;
        }
        // addiu
        case 0x09:
        {
            uint32_t sourceNum = regs[rs]; 
            int32_t imm = signExtendedImm(instruction);

            regs[rt] = sourceNum + imm;
            break;
        }
        // andi
        case 0x0c:
        {
            uint32_t num = regs[rs];
            // zero extended immediate 
            uint32_t immediate = (0x0000FFFF & instruction);
            regs[rt] = num & immediate;
            break;
        }
        // ori
        case 0x0d:
        {
            uint32_t num = regs[rs];
            // zero extended immediate 
            uint32_t immediate = (0x0000FFFF & instruction);
            regs[rt] = num | immediate;
            break;
        }
        // sw
        case 0x2b:
        {
            uint32_t value = regs[rt];
            uint32_t regAddr = regs[rs];
            int32_t offset = signExtendedImm(instruction);
            myMem->setMemValue((regAddr + offset), value, WORD_SIZE);
            break;
        }
        // lw
        case 0x23:
        {
            uint32_t regAddr = regs[rs];
            int32_t offset = signExtendedImm(instruction);
            myMem->getMemValue((regAddr + offset), regs[rt], WORD_SIZE);
            break;
        }
        // beq
        case 0x04:
        {
            if (regs[rt] == regs[rs]) {
                uint32_t tempPc = *pc;
                *pc = *pc + 4;
                executeInstruction(pc, regs, myMem);
                *pc = tempPc + (signExtendedImm(instruction) << 2);
            } 
            break;
        }
        // bne
        case 0x05:
        {
            if (regs[rt] != regs[rs]) {
                *pc = *pc + (signExtendedImm(instruction) << 2);
            } 
            break;
        }
        // slti
        case 0x0a:
        {
            int32_t immediate = signExtendedImm(instruction);
            int32_t num = regs[rs];
            regs[rt] = (num < immediate) ? 1 : 0;
            break;
        }
        // sltiu
        case 0x0b:
        {
            uint32_t immediate = signExtendedImm(instruction);
            uint32_t num = regs[rs];
            regs[rt] = (num < immediate) ? 1 : 0;
            break;
        }
        // lui
        case 0x0f:
        {   
            uint32_t immediate = (0x0000FFFF & instruction);
            immediate = (immediate << 16);
            regs[rt] = immediate;
            break;
        }
        // lbu
        case 0x24:
        {
            uint32_t regAddr = regs[rs];
            int32_t offset = signExtendedImm(instruction);
            myMem->getMemValue((regAddr + offset), regs[rt], BYTE_SIZE);
            break;
        }
        // lhu
        case 0x25:
        {
            uint32_t regAddr = regs[rs];
            int32_t offset = signExtendedImm(instruction);
            myMem->getMemValue((regAddr + offset), regs[rt], HALF_SIZE);
            break;
        }
        // sb
        case 0x28:
        {
            uint32_t value = createMask(0, 7, regs[rt]);
            uint32_t regAddr = regs[rs];
            int32_t offset = signExtendedImm(instruction);
            myMem->setMemValue((regAddr + offset), value, BYTE_SIZE);
            break;
        }
        // sh
        case 0x29:
        {
            uint32_t value = createMask(0, 15, regs[rt]);
            uint32_t regAddr = regs[rs];
            int32_t offset = signExtendedImm(instruction);
            myMem->setMemValue((regAddr + offset), value, HALF_SIZE);
            break;
        }
        // bgtz
        case 0x07:
        {
            if (regs[rs] > 0) {
                *pc = *pc + (signExtendedImm(instruction) << 2);
            } 
            break;
        }
        // blez
        case 0x06:
        {
            if (regs[rs] <= 0) {
                *pc = *pc + (signExtendedImm(instruction) << 2);
            } 
            break;
        }
        default:
            fprintf(stderr,"Illegal operation..."); 
            exit(127);
    }

    // go to next instruction
    *pc = *pc + 4;
}

// Helper function to decode and execute the R type instruction
void rHelper(uint32_t instruction, uint32_t* pc, uint32_t* regs, MemoryStore *myMem) {

   uint32_t rd = createMask(11, 15, instruction);
   uint32_t rt = createMask(16, 20, instruction);
   uint32_t rs = createMask(21, 25, instruction);


    // get funct code
  uint32_t funct = createMask(0, 5, instruction);

  cout << funct << " is the funct code" << endl;

    switch(funct){
      // add signed
        case 0x20:
	  {
	  int32_t op1 = regs[rs];
	  int32_t op2 = regs[rt];

	  int32_t result = op1 + op2;
 
      cout << "Add op1 is " << op1 << endl;
      cout << "And op2 is " << op2 << endl;

      cout << "Result is" << result <<endl;


      if ((op1 > 0 && op2 > 0 && result < 0) || (op1 < 0 && op2 < 0 && result > 0)) {
        RegisterInfo reg;
        dumpRegisterContents(&reg, regs);
        dumpRegisterState(reg); 
        dumpMemoryState(myMem);
        fprintf(stderr,"Arithmetic Overflow \n"); 
        exit(12);
      }

      regs[rd] = result;
	 
	  *pc = *pc + 4;
	  break;
	  }
       // add unsigned
       case 0x21:
        {
	uint32_t op1 = regs[rs];
	uint32_t op2 = regs[rt];
	  
        regs[rd] = op1 + op2;
	
	*pc = *pc + 4;
	break;
      }
      // and
    case 0x24:
      {
	uint32_t op1 = regs[rs];
	uint32_t op2 = regs[rt];
	  
        regs[rd] = op1 & op2;
	
	*pc = *pc + 4;
	break;
      }
      // jr
    case 0x08:
      {
	*pc = regs[rs];

	break;
      }
      // nor
    case 0x27:
      {
	uint32_t op1 = regs[rs];
	uint32_t op2 = regs[rt];
	  
    regs[rd] = ~(op1 | op2);
	*pc = *pc + 4;

	break;
      }
       // or
    case 0x25:
      {
	uint32_t op1 = regs[rs];
	uint32_t op2 = regs[rt];
	  
        regs[rd] = (op1 | op2);
	*pc = *pc + 4;

	break;
      }
      // slt
      case 0x2a:
      {
	int32_t op1 = regs[rs];
	int32_t op2 = regs[rt];
	  
        regs[rd] = (op1<op2)?1:0;
	*pc = *pc + 4;

	break;
      }
      // sltu
      case 0x2b:
      {
	uint32_t op1 = regs[rs];
	uint32_t op2 = regs[rt];
	  
        regs[rd] = (op1<op2)?1:0;
	*pc = *pc + 4;

	break;
      }
      // sll
    case 0x00:
      {
	uint32_t shamt = createMask(6,10,instruction);

	uint32_t op1 = regs[rt];
	  
        regs[rd] = op1<<shamt;
	*pc = *pc + 4;

	break;
      }
      // srl
    case 0x02:
      {
	uint32_t shamt = createMask(6,10,instruction);

	uint32_t op1 = regs[rt];
	  
        regs[rd] = op1>>shamt;
	*pc = *pc + 4;

	break;
      }
      // sub signed
        case 0x22:
	  {
	  int32_t op1 = regs[rs];
	  int32_t op2 = regs[rt];


      int result = op1 - op2;

        // check for overflow
        if ((op1 < 0 && op2 > 0 && result > 0) || (op1 > 0 && op2 < 0 && result < 0)) {
            RegisterInfo reg;
            dumpRegisterContents(&reg, regs);
            dumpRegisterState(reg); 
            dumpMemoryState(myMem);
            fprintf(stderr,"Arithmetic Overflow \n"); 
            exit(12);
            
        }

	  
	  regs[rd] = result;
	 
	  *pc = *pc + 4;
	  break;
	  }
        // sub unsigned
        case 0x23:
        {
        uint32_t op1 = regs[rs];
        uint32_t op2 = regs[rt];
        
        
        regs[rd] = op1 - op2;
        
        *pc = *pc + 4;
        break;
      }      
    default:
        fprintf(stderr,"Illegal operation..."); 
        exit(127);
    }

}

// Decode instruction at pc and execute it
int executeInstruction(uint32_t* pc, uint32_t* regs, MemoryStore *myMem) {

        // create variable to hold current instruction
        uint32_t instruction;

        myMem->getMemValue(*pc, instruction, WORD_SIZE);
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
                return 1;
            case Pad:
                cout << "Pad" << endl;
                *pc += INSTR_SIZE;
                break;
            case R:
                cout << "R" << endl;
		        rHelper(instruction, pc, regs, myMem);
                break;
            case J:
	      {
                cout << "J" << endl;
                if (createMask(26,31,instruction) == 3) {
                    cout << "Jump and link" << endl;
                            regs[31] = *pc + 8;
                }
                jAddress = jAddress << 2;
                uint32_t pcAddr = *pc & ~0xFFFFFFF;
                *pc = pcAddr | jAddress;
                break;
	      }
            case I:
                cout << "I" << endl;
                iHelper(instruction, pc, regs, myMem);
                break;
            default:
                fprintf(stderr,"Illegal operation..."); 
                exit(127);
        }
	regs[0] = 0;
    return 0;

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
        if (executeInstruction(&pc, regs, myMem)) {
            return 0;
        }
    }


    return 0;
}
