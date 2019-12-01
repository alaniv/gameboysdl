#ifndef CARTREADER_H
#define CARTREADER_H

#include <fstream>
#include <vector> 
#include <string>
#include "Cartidge.h"

class CartReader {
  //TODO: this only works with 32K ROMS, like TETRIS.
  public:
    /*
    std::vector<std::uint8_t> ROM0; //0000-3FFF(16K)
    std::vector<std::uint8_t> ROM1; //4000-7FFF(16K)
    std::vector<std::uint8_t> ERAM; //A000-BFFF(8K)
    */
    Cartidge* cart;
    CartReader();
    void loadROMfromFile(std::string romLocation);

};

#endif /* ROMLOADER_H */
