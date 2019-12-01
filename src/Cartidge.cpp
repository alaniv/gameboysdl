#include "Cartidge.h"

Cartidge_ROM_ONLY::Cartidge_ROM_ONLY()
  {
    CartType = 0x00;
  }



uint8_t Cartidge_ROM_ONLY::read(uint16_t address){
  if(address < 0x8000){ //BK 0 + Bk 1
    return fullData[address];
  } else {
    std::cerr << "ROM.ONLY: INVALID READ ADDRESS ERAM " << std::endl;
    exit(1);
  }
}

void Cartidge_ROM_ONLY::write(uint8_t addrBit, uint8_t data){
  std::cerr << "CART: not valid write to ROM-ONLY" << std::endl;
  return;
}  
  
void Cartidge_ROM_ONLY::writeERAM(uint16_t address, uint8_t data){return;} //ERROR
uint8_t Cartidge_ROM_ONLY::readERAM(uint16_t address){return 0xFF;} //ERROR
  
 ///
 /////  MBC1 (max 2MByte ROM and/or 32KByte RAM)
 ///
 
  
  
Cartidge_MBC1::Cartidge_MBC1() : 
  ramEnable(false), ramRomSelect(false), bankNumber(0x01)
  {
    CartType = 0x01;
  }
  
uint8_t Cartidge_MBC1::read(uint16_t address){
  if(address < 0x4000){
    return fullData[address]; //ROM BANK 0
  } else if(address < 0x8000){
    if(!ramRomSelect){
      return fullData[address - 0x4000 + bankNumber*0x4000]; //ROM BANK N
    } else {
      return fullData[address - 0x4000 + (bankNumber & 0x1F)*0x4000]; //ROM BANK N
    }
  } else {
    std::cerr << "ROM.ONLY: INVALID READ" << std::endl;
    return 0xFF;
  }
}
  
void Cartidge_MBC1::write(uint8_t addrBit, uint8_t data){
  switch(addrBit){
    case 0:
    case 1:
      if( (data & 0xF) == 0x0A){
        ramEnable = true;
      } else {
        ramEnable = false;
      }
      break;
    case 2:
    case 3:
      bankNumber &= 0xE0; //clear low 5 bit
      bankNumber |= (data & 0x1F);
      if( (bankNumber == 0x00) || (bankNumber == 0x20) || (bankNumber == 0x40)|| (bankNumber == 0x60)){
        bankNumber++;
      }
      break;
    case 4:
    case 5:
      bankNumber &= 0x9F; //clear bit 6-7
      bankNumber |= (data & 0x60);
      break;
    case 6:
    case 7:
     if( data == 0x00){
        ramRomSelect = false;
      } else {
        ramRomSelect = true;
      }
      break;
    default:
      std::cerr << "MBC1_ERROR" << std::endl;
      exit(1);
      break;
  } 
}

void Cartidge_MBC1::writeERAM(uint16_t address, uint8_t data){
  //std::cerr << "writeERAM" << std::endl;
  if(ramEnable && (address < ERAM.size()) ){
    if(ramRomSelect){
      ERAM[address + (bankNumber >> 5) * 0x2000] = data;
    } else {
      ERAM[address] = data;
    }
  }// else {
  //  std::cerr << "ERROR?" << std::endl;
  //}
  return;
}

uint8_t Cartidge_MBC1::readERAM(uint16_t address){
  if(ramEnable && (address < ERAM.size()) ){
    if(ramRomSelect){
      return ERAM[address + (bankNumber >> 5) * 0x2000];
    } else {
      return ERAM[address];
    }
  } else {
    //std::cerr << "ERROR??" << std::endl;
    return 0xFF; //ERROR?
  }
}

void Cartidge_MBC1::allocateRAM(uint32_t ramSize){
  std::cerr << "allocate ERAM" << std::endl;
  ERAM.resize(ramSize);
}

////
//   MBC3 (max 2MByte ROM and/or 32KByte RAM and Timer)
////

Cartidge_MBC3::Cartidge_MBC3() : 
  ramEnable(false),romBankNumber(0x01), ramBankNumber(0x00)
  {
    CartType = 0x13;
  }
  
uint8_t Cartidge_MBC3::read(uint16_t address){
  if(address < 0x4000){
    return fullData[address]; //ROM BANK 0
  } else if(address < 0x8000){
    return fullData[address - 0x4000 + romBankNumber*0x4000]; //ROM BANK N
  } else {
    std::cerr << "ROM.ONLY: INVALID READ" << std::endl;
    return 0xFF;
  }
}
  
void Cartidge_MBC3::write(uint8_t addrBit, uint8_t data){
  switch(addrBit){
    case 0:
    case 1:
      if( (data & 0xF) == 0x0A){
        ramEnable = true;
      } else {
        ramEnable = false;
      }
      break;
    case 2:
    case 3:
      romBankNumber = (data & 0x7F);
      if(romBankNumber == 0x00){ //20 40 60 supported
        romBankNumber++;
      }
      break;
    case 4:
    case 5:
      ramBankNumber = (data & 0x0F); //00-03 -> RAM ; 08-0C -> RTC
      break;
    case 6:
    case 7:
     if( data == 0x00){
       //RTC related;       
     }
     if( data == 0x01){
       //RTC related;       
     }
      break;
    default:
      std::cerr << "MBC1_ERROR" << std::endl;
      exit(1);
      break;
  } 
}

void Cartidge_MBC3::writeERAM(uint16_t address, uint8_t data){
  //std::cerr << "writeERAM" << std::endl;
  if(ramEnable && (ramBankNumber <= 0x03) && (address < ERAM.size()) ){
    ERAM[address + (ramBankNumber >> 5) * 0x2000] = data;
  }
  else if(ramEnable && (ramBankNumber >= 0x08) && (ramBankNumber <= 0x0C) && (address < ERAM.size()) ){
    //RTC related
    return;
  }
  return;
}

uint8_t Cartidge_MBC3::readERAM(uint16_t address){
  if(ramEnable && (address < ERAM.size()) ){
    if(ramBankNumber <= 0x03){
      return ERAM[address + (ramBankNumber >> 5) * 0x2000];
    }
    else if((ramBankNumber >= 0x08) && (ramBankNumber <= 0x0C) ){
      return 0x00; //RTC RELATED
    }
    else{
      //std::cerr << "ERROR??" << std::endl;
      return 0xFF; //ERROR?
    }
  } else {
    //std::cerr << "ERROR??" << std::endl;
    return 0xFF; //ERROR?
  }
}

void Cartidge_MBC3::allocateRAM(uint32_t ramSize){
  std::cerr << "allocate ERAM" << std::endl;
  ERAM.resize(ramSize);
}