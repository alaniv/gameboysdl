#ifndef IO_H
#define IO_H

#include <iostream>
#include "GPU.h"

class GPU; //fd

class IO {   
  public:
    GPU & _gpu;
    IO(GPU& gpu);
    uint8_t read_addr(uint16_t address);
    void write_addr(uint16_t address, uint8_t data);
    uint8_t JOYP; // FF00 JOYPAD
    uint8_t input_buffer;
    uint16_t DIV; // FF04 DIV - Divider Register (R/W) // 16 = DIV + internal
    uint16_t TIMA; // FF05 TIMA - Timer counter (R/W) // 16 = TIMA + internal
    uint8_t TMA; // FF06 Timer Modulo (R/W)
    uint8_t TAC; // FF07 Timer Control (R/W)
    uint8_t IE; // INTERRUPTION ENABLE
    uint8_t IF; // INTERRUPTION FLAG
    
    void timer_step(uint8_t deltaCycles);
};

#endif /* IO_H */
