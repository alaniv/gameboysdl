#include "IO.h"


IO::IO(GPU& gpu) : 
  _gpu(gpu), JOYP(0xFF), input_buffer(0xFF),
  DIV(0x0000), TIMA(0x0000), TMA(0x00), TAC(0x00),
  IE(0x00), IF(0x00)
{
}

void IO::timer_step(uint8_t deltaCycles){
    DIV += deltaCycles; //TODO: 64 M-cycles = 1 DIV ?
    if( (TAC & 0x4) == 0x0){
       TIMA = 0x0000; //TODO: value???
    } else {
       uint16_t preTIMA = TIMA;
       switch(TAC & 0x03){
            case 0x00: //256
                TIMA += deltaCycles/4;
                break; 
            case 0x01: //4
                TIMA += deltaCycles*16;
                break;
            case 0x02: //16
                TIMA += deltaCycles*4;
                break;
            case 0x03: //64
                TIMA += deltaCycles;
                break;
       }
       if( preTIMA > TIMA){ //overflow?
            IF |= 0x04; //TIMER IRQ
            TIMA = TMA;
       }
    }
}


uint8_t IO::read_addr(uint16_t address){
  switch(address){
    case 0x00:
      return JOYP;
      break;
      
    case 0x01:
      std::cerr << "IO READ EXCEPTION SB - Serial transfer data NOT IMPLEMENTED" << std::endl;
      //exit(1);
      return 0x00;
      break;
      
    case 0x02:
      std::cerr << "IO READ EXCEPTION SC - Serial Transfer Control NOT IMPLEMENTED" << std::endl;
      //exit(1);
      return 0x00;
      break;
      
    case 0x04:
      return static_cast<uint8_t>(DIV>>4);
      
    case 0x05:
      return static_cast<uint8_t>(TIMA>>4);
      
    case 0x06:
      return TMA;
      
    case 0x07:
      return TAC;
    
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x25:
    case 0x26:

    case 0x30:
    case 0x31:
    case 0x32:
    case 0x33:
    case 0x34:
    case 0x35:
    case 0x36:
    case 0x37:
    case 0x38:
    case 0x39:
    case 0x3A:
    case 0x3B:
    case 0x3C:
    case 0x3D:
    case 0x3E:
    case 0x3F:
      //std::cerr << "IO READ EXCEPTION AUDIO " << std::to_string(address) << std::endl;
      return 0x00;
      break;
     
     
    case 0x40: //LCDC - VIDEO
    case 0x41: //STAT - VIDEO
    case 0x42: //SCY - VIDEO
    case 0x43: //SCX - VIDEO
    case 0x44: //LY - VIDEO
    case 0x45: //LYC - VIDEO
    case 0x47: //BGP - VIDEO
    case 0x48: //OBP0 - VIDEO
    case 0x49: //OBP1 - VIDEO
    case 0x4A: //WY - VIDEO
    case 0x4B: //WX - VIDEO
      return _gpu.read_register(address);
      break;
    
    case 0x4D: // CGB ONLY SPEED SWITCH ????
      //std::cerr << "IO READ EXCEPTION WRONG ADDRESS " << std::to_string(address) << std::endl;
      return 0xFF;
      
    default:
      std::cerr << "IO READ EXCEPTION WRONG ADDRESS " << std::to_string(address) << std::endl;
      exit(1);
      return 0xFF;
      break;
  }
}

void IO::write_addr(uint16_t address, uint8_t data){
  switch(address){
    case 0x00:{
      JOYP &= 0xCF; //clear 5 4
      JOYP |= (data & 0x30); //set data to 5 4
      JOYP |= 0x0F; //clear low
      if( (JOYP & 0x30) == 0x10){
        JOYP &= input_buffer & 0x0F;
      } else if ( (JOYP & 0x30) == 0x20){
        JOYP &= ( input_buffer >> 4 ) & 0x0F;
      }
      break;    
    }
    case 0x01: //SB - Serial transfer data (R/W)
      break;
    case 0x02: //SC - SIO control (R/W)
      break;
      
    case 0x04:
      DIV = 0x00; //RESETS ON WRITE
      break;
    case 0x05:
      TIMA = (data<<4);
      break;
    case 0x06:
      TMA = data;
      break;
    case 0x07:
      TAC = data;
      break;
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x25:
    case 0x26:

    case 0x30:
    case 0x31:
    case 0x32:
    case 0x33:
    case 0x34:
    case 0x35:
    case 0x36:
    case 0x37:
    case 0x38:
    case 0x39:
    case 0x3A:
    case 0x3B:
    case 0x3C:
    case 0x3D:
    case 0x3E:
    case 0x3F:
      //std::cerr << "IO WRITE EXCEPTION AUDIO " << std::to_string(address) << std::endl;
      break;
      
    case 0x40: //LCDC - VIDEO
    case 0x41: //STAT - VIDEO
    case 0x42: //SCY - VIDEO
    case 0x43: //SCX - VIDEO
    case 0x44: //LY - VIDEO
    case 0x45: //LYC - VIDEO
    case 0x47: //BGP - VIDEO
    case 0x48: //OBP0 - VIDEO
    case 0x49: //OBP1 - VIDEO
    case 0x4A: //WY - VIDEO
    case 0x4B: //WX - VIDEO
      _gpu.write_register(address, data);
      break;
    case 0x7f:
      break;
      
    default:
      std::cerr << "IO WRITE EXCEPTION WRONG ADDRESS " << std::to_string(address) << " DATA " << std::to_string(data) << std::endl;
      exit(1);
      break;
  }
}
