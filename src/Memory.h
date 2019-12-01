#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <string>
#include <fstream>

class Memory {
  public:
    std::vector<std::uint8_t> BIOS;                          //0000-00FF(256) (released later for ROM0)
    //std::array<uint8_t, 0x4000> ROM0;                      //0000-3FFF(16K)
    //std::array<uint8_t, 0x4000> ROM1;                      //4000-7FFF(16K)
    //std::array<uint8_t, 0x2000> VRAM;                      //8000-9FFF(8K)
    //std::array<uint8_t, 0x2000> ERAM;                      //A000-BFFF(8K)
    std::vector<std::uint8_t> WRAM;                          //C000-DFFF(8K)
    //std::array<uint8_t, 0x1E00> WRAM_SHADOW_minus_0.5;     //E000-FDFF(7.5K)
    //std::array<uint8_t, 0xA0> OAMRAM;                      //FE00-FE9F(160)
    //std::array<uint8_t, 0x60> NOTUSABLE;                   //FEA0-FEFF(96)
    //std::array<uint8_t, 0x80> IO;                          //FF00-FF7F(128)
    std::vector<std::uint8_t> ZRAM;                          //FF80-FFFF(128)     
    
    Memory();
    void loadBIOSfromFile(std::string romLocation);
};

#endif /* MEMORY_H */
