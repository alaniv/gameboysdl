#ifndef GPU_H
#define GPU_H

#include <vector>
#include <chrono>
#include <iostream>
#include "IO.h"

class IO; //fd

class GPU {
  private:
    uint32_t clock;
    uint8_t mode; // use STAT...
    const std::vector<uint8_t> COLOR;
    
    uint8_t LCDC; // 40
    uint8_t STAT; // 41
    uint8_t SCX; // 42
    uint8_t SCY; // 43
    uint8_t LY; //44 R
    uint8_t LYC; //45
    uint8_t DMA; //46
    uint8_t BGP; // 47
    uint8_t OBP0; // 48
    uint8_t OBP1; // 49
    uint8_t WX; // 4A
    uint8_t WY; // 4B
    
    IO& _io;
    void draw_line();
    std::vector<std::uint8_t> bufferImage;


  public:
    std::vector<std::uint8_t> VRAM; //8000-9FFF(8K)
    std::vector<std::uint8_t> OAMRAM; //FE00-FE9F(160)
    bool available_image;
    
    GPU(IO& _io);
    
    std::vector<std::uint8_t> getBufferImage();
    uint8_t read_register(uint8_t address);
    void write_register(uint16_t address, uint8_t data);
    void update(uint8_t deltaCycles);

};

#endif /* GPU_H */
