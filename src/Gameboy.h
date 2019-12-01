#ifndef GAMEBOY_H
#define GAMEBOY_H

class GPU;
class CPU;

#include <chrono>
#include "GPU.h"
#include "CPU.h"
#include "IO.h"
#include "Memory.h"
#include "MMU.h"
#include "CartReader.h"

enum class GB_Key : uint8_t {
  A_Press = 0xFE,
  B_Press = 0xFD,
  SELECT_Press = 0xFB,
  START_Press = 0xF7,
  
  A_Release = 0x01,
  B_Release = 0x02,
  SELECT_Release = 0x04,
  START_Release = 0x08,
  
  RIGHT_Release = 0x10,
  LEFT_Release = 0x20,
  UP_Release = 0x40,
  DOWN_Release = 0x80,
  
  RIGHT_Press = 0xEF,
  LEFT_Press = 0xDF,
  UP_Press = 0xBF,
  DOWN_Press = 0x7F
};

class Gameboy {
  private:
    Memory mem;
    CartReader cart;
    GPU gpu;
    IO io;
    MMU mmu;
    CPU cpu;
    uint8_t input_buffer;
    //Audio audio;

  
  public:
    Gameboy();
    void LoadRom(std::string filename);
    void LoadBootRom(std::string filename);
    void emuCycle();
    uint16_t emuTestCycle();
    std::vector<std::uint8_t> getBufferedImage(); // 160 * 144 * 4
    bool availableImage();
    uint32_t cycles();
    
    void input(enum GB_Key keyEvent);
};

#endif /* GAMEBOY_H */
