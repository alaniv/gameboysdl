#ifndef MMU_H
#define MMU_H

#include "IO.h"
#include "Memory.h"
#include "CartReader.h"
#include "GPU.h"

class MMU {

  private:
    bool bios_on; // BIOS MAPPED. associated with FF50

    
    Memory& _memory;
    IO& _io;
    GPU& _gpu;
    CartReader& _cartidge;
    

  public:
    MMU(Memory& memory, IO& io, GPU& gpu, CartReader& cartidge);
    uint8_t read_addr(uint16_t address);
    void write_addr(uint16_t address, uint8_t data);
};

#endif /* MMU_H */
