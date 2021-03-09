#include <iostream>
#include <iomanip>
#include "MemoryStore.h"
#include "RegisterInfo.h"
#include "EndianHelpers.h"
#include <fstream>


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


int main() {

    uint32_t instruction = 0xF0000080;
    uint32_t result = createMask(4, 7, instruction);
    cout << result << endl;



}


