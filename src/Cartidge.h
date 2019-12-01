#ifndef CARTIDGE_H
#define CARTIDGE_H

#include <fstream>
#include <vector>
#include <string>
#include <iostream>

class Cartidge {
  public:
    std::vector<std::uint8_t> fullData; //whole ROM
    uint8_t CartType;
    virtual void write(uint8_t addrBit, uint8_t data) = 0;
    virtual uint8_t read(uint16_t address) = 0;
    virtual void writeERAM(uint16_t address, uint8_t data) = 0;
    virtual uint8_t readERAM(uint16_t address) = 0;
    virtual void allocateRAM(uint32_t ramSize) = 0;
};

class Cartidge_ROM_ONLY : public Cartidge{
  public:
    Cartidge_ROM_ONLY();
    void write(uint8_t addrBit, uint8_t data);
    uint8_t read(uint16_t address);
    void writeERAM(uint16_t address, uint8_t data);
    uint8_t readERAM(uint16_t address);
    void allocateRAM(uint32_t ramSize){};
};


class Cartidge_MBC1 : public Cartidge{
  public:
    bool ramEnable;
    bool ramRomSelect; //false = ROM
    std::vector<uint8_t> ERAM;
    Cartidge_MBC1();
    uint8_t bankNumber;
    void write(uint8_t addrBit, uint8_t data);
    uint8_t read(uint16_t address);
    void writeERAM(uint16_t address, uint8_t data);
    uint8_t readERAM(uint16_t address);
    void allocateRAM(uint32_t ramSize);
};

class Cartidge_MBC3 : public Cartidge{
  public:
    bool ramEnable;
    std::vector<uint8_t> ERAM;
    Cartidge_MBC3();
    uint8_t romBankNumber;
    uint8_t ramBankNumber;
    void write(uint8_t addrBit, uint8_t data);
    uint8_t read(uint16_t address);
    void writeERAM(uint16_t address, uint8_t data);
    uint8_t readERAM(uint16_t address);
    void allocateRAM(uint32_t ramSize);
};

#endif /* CARTIDGE_H */
