#include "MMU.h"

MMU::MMU(Memory& memory, IO& io, GPU& gpu, CartReader& cartidge) : bios_on(true),
_memory(memory), _io(io), _gpu(gpu), _cartidge(cartidge)
{
}

uint8_t MMU::read_addr(uint16_t address){
  switch(address & 0xF000){
    case 0x0000:
      if(bios_on && address < 0x0100){
        return _memory.BIOS[address];
      } else {
        return (_cartidge.cart)->read(address); 
      }
    case 0x1000:
    case 0x2000:
    case 0x3000:
      return (_cartidge.cart)->read(address); 
      
    case 0x4000:
    case 0x5000:
    case 0x6000:
    case 0x7000:
      return (_cartidge.cart)->read(address); 

    case 0x8000:
    case 0x9000:
      return _gpu.VRAM[address - 0x8000]; 
      
    case 0xA000:
    case 0xB000:
      return (_cartidge.cart)->readERAM(address - 0xA000); 

    case 0xC000:
    case 0xD000:
      return _memory.WRAM[address - 0xC000]; 
      
    //WRAM_SHADOW_minus_0.5;     //E000-FDFF(7.5K)
    case 0xE000:
      return _memory.WRAM[address - 0xE000];
      
    case 0xF000:
      switch(address & 0x0F00){
        case 0x0000:
        case 0x0100:
        case 0x0200:
        case 0x0300:
        case 0x0400:
        case 0x0500:
        case 0x0600:
        case 0x0700:
        case 0x0800:
        case 0x0900:
        case 0x0A00:
        case 0x0B00:
        case 0x0C00:
        case 0x0D00:
          return _memory.WRAM[address - 0xE000];
        case 0x0E00:
          if(address < 0xFEA0){
            return _gpu.OAMRAM[address - 0xFE00];
          } else {
            // handle this case better
            std::cout << "MMU read NON ACCESSIBLE MEMORY ADDRESS 0xFEA0" << std::endl;
            return 0xFF;
          }
        case 0x0F00:
          if (address < 0xFF80){
            if (address == 0xFF0F){
              //std::cout << "MMU read IF" << std::endl;
              return _io.IF; // OK?
            } else {
              uint16_t ioAddr = address & 0x00FF;
              return _io.read_addr(ioAddr);
            }
          } else if (address == 0xFFFF){
            ///std::cout << "MMU read IE" << std::endl;
            return _io.IE; // OK ?
          } else { 
            return _memory.ZRAM[address - 0xFF80];
          }
          break;
        default:
          // handle this case better
          std::cout << "MMU read EXCEPTION 0xFF00" << std::endl;
          exit(1);
      }  
    default:
      // handle this case better
      std::cout << "MMU read EXCEPTION UNK" << std::endl;
      exit(1);
  }
}

void MMU::write_addr(uint16_t address, uint8_t data){
  switch(address & 0xF000){
    case 0x0000:
    case 0x1000:
    case 0x2000:
    case 0x3000:          
    case 0x4000:
    case 0x5000:
    case 0x6000:
    case 0x7000:
      // handle this case better
      (_cartidge.cart)->write((address & 0xF000) >> 12 , data);
      //exit(1);
      break;
      
    case 0x8000:
    case 0x9000:
      _gpu.VRAM[address - 0x8000] = data; 
      break;
            
    case 0xA000:
    case 0xB000:
      (_cartidge.cart)->writeERAM(address - 0xA000, data); 
      break;
      
    case 0xC000:
    case 0xD000:
      _memory.WRAM[address - 0xC000] = data; 
      break;
            
    //WRAM_SHADOW
    case 0xE000:
      _memory.WRAM[address - 0xE000] = data;
      break;
            
    case 0xF000:
      switch(address & 0x0F00){
        case 0x0000:
        case 0x0100:
        case 0x0200:
        case 0x0300:
        case 0x0400:
        case 0x0500:
        case 0x0600:
        case 0x0700:
        case 0x0800:
        case 0x0900:
        case 0x0A00:
        case 0x0B00:
        case 0x0C00:
        case 0x0D00:
          _memory.WRAM[address - 0xE000] = data;
          break;
          
        case 0x0E00:
          if(address < 0xFEA0){
            _gpu.OAMRAM[address - 0xFE00] = data;
          }
          break;
          
        case 0x0F00:
          if (address < 0xFF80){
            if (address == 0xFF0F){
              _io.IF = data;
            } else if (address == 0xFF50 && data == 0x01){ 
              // UNMAP BOOT ROM
              bios_on = false;
            } else if (address == 0xFF46){
              //std::cout << "OAM DMA" << std::endl;
              uint16_t offset = (data << 8);
              for(uint8_t i = 0x00; i < 0xA0; i++){
                _gpu.OAMRAM[i] = read_addr(offset);
                offset++;
              }
            } else {
              uint16_t ioAddr = address & 0x00FF;
              _io.write_addr(ioAddr, data);
            }
          } else if (address == 0xFFFF){
            _io.IE = data;
          } else { 
            _memory.ZRAM[address - 0xFF80] = data;
          }
          break;
        default:
          // handle this case better
          std::cout << "MMU write EXCEPTION UNK 0xFF00" << std::endl;
          exit(1);    
          break;    
      }
      break;
    default:
      // handle this case better
      std::cout << "MMU write EXCEPTION UNK" << std::endl;
      exit(1);
  }
}
