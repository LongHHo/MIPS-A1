#include <iostream>
#include <iomanip>
#include "MemoryStore.h"
#include "RegisterInfo.h"
#include "EndianHelpers.h"
#include <fstream>
#include <bitset>

using namespace std;



// make own function later, from stack overflow
uint32_t createMask(uint8_t a, uint8_t b, uint32_t instruction)
{

  if (a > b) throw("Oh no");
  
  uint32_t r = 0;

  uint32_t aMask = ((1 << a) - 1);
  uint32_t bMask = ((1 << b + 1) - 1);

  if (b == 31) {
    bMask = 0xFFFFFFFF;
  }


  r = bMask ^ aMask;

  cout << r << endl;

  return (instruction & r) >> a;

}


int32_t signExtendedImm(uint32_t instruction) {

    int32_t first16 = (0x0000FFFF & instruction);

    // check if 16th bit is 1, if so sign extend
    if (0x00008000 & first16) {
        return (0xFFFF0000 + first16);
    } else {
        return first16;
    }
}


int main() {

	int op1 = 0b11100100000000000000000000000011;
	int op2 = 0b01111111111111111111111111111110;
  int op3 = 0b11100100000000001000000000000000;


  int32_t res1 = signExtendedImm(op1);
  int32_t res2 = signExtendedImm(op3);

  bitset<32> y(res1);
  bitset<32> x(res2);
  cout << y << endl;
  cout << x << endl;



}


