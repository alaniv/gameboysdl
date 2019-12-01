#include "GPU.h"

GPU::GPU(IO& io) :
  clock(0), mode(0), 
  COLOR{
    0xE0, 0xF8, 0xD0, //verde claro
    0x88, 0xC0, 0x70, //verde medio
    0x34, 0x68, 0x56, //verde oscuro
    0x08, 0x24, 0x32 //verde negro
  },
  LCDC(0), STAT(0), SCX(0), SCY(0), LY(0), LYC(0), DMA(0),
  BGP(0), OBP0(0), OBP1(0), WX(0), WY(0),
  _io(io),
  bufferImage(160*144*4, 0), VRAM(0x2000, 0), OAMRAM(0xA0, 0), available_image(false)
{
  for(int i=3; i< 160*144*4; i += 4){
    bufferImage[i] = 0xFF;
  }

}

uint8_t GPU::read_register(uint8_t address){
  switch(address){
    case 0x40: //LCDC - VIDEO
      return LCDC;
    case 0x41: //STAT - VIDEO
      return STAT;
    case 0x42: //SCY - VIDEO
      return SCY;
    case 0x43: //SCX - VIDEO
      return SCX;
    case 0x44: //LY - VIDEO
      return LY;
    case 0x45: //LYC - VIDEO
      return LYC;
    case 0x47: //BGP - VIDEO
      return BGP; 
    case 0x48: //OBP0 - VIDEO
      return OBP0; 
    case 0x49: //OBP1 - VIDEO
      return OBP1;
    case 0x4A: //WY - VIDEO
      return WY;
    case 0x4B: //WX - VIDEO
      return WX;
    default:
      std::cerr << "GPU READ EXCEPTION WRONG ADDRESS " << std::to_string(address) << std::endl;
      exit(1);
      break;
  }
}

void GPU::write_register(uint16_t address, uint8_t data){
  switch(address){    
    case 0x40: //LCDC - VIDEO
      LCDC = data;
      break;
    case 0x41: //STAT - VIDEO
      STAT = data;
      break;
    case 0x42: //SCY - VIDEO
      SCY = data;
      break;
    case 0x43: //SCX - VIDEO
      SCX = data;
      break;
    case 0x45: //LYC - VIDEO
      LYC = data;
      break;
    case 0x46: //DMA - VIDEO
      std::cerr << "GPU WRITE : NOT IMPLEMENTED VIDEO : DMA" << std::endl;
      exit(1);
      break;
    case 0x47: //BGP - VIDEO
      BGP = data;
      break;
    case 0x48: //OBP0 - VIDEO
      OBP0 = data;
      break;
    case 0x49: //OBP1 - VIDEO
      OBP1 = data;
      break;
    case 0x4A: //WY - VIDEO
      WY = data;
      break;
    case 0x4B: //WX - VIDEO
      WX = data;
      break;
    default:
      std::cerr << "GPU WRITE EXCEPTION WRONG ADDRESS " << std::to_string(address) << " DATA " << std::to_string(data) << std::endl;
      exit(1);
      break;
  }
}

std::vector<std::uint8_t> GPU::getBufferImage(){
  if(!available_image){
    std::cout << "GPU ERROR: NOT AVAILABLE IMAGE" << std::endl;
    exit(1);
  }
  available_image = false;
  return bufferImage;
}


void GPU::update(uint8_t deltaCycles){
  clock += deltaCycles;
  if(LY == LYC){
    STAT |= 0x40;
    _io.IF |= 0x02; // SET IRQ for STAT
  }
  switch(STAT & 0x03){
    case 0x00:{ //H-Blank
      if( clock >= 204){
        clock -= 204;
        LY++;
        if(LY > 143){
          STAT &= 0xFC; STAT |= 0x01; // mode 1
          available_image = true;
          _io.IF |= 0x01; // SET IRQ for VBLANK
        } else {
          STAT &= 0xFC; STAT |= 0x02; // mode 2
        }
      }
      break;
    }
    case 0x01:{ //V-Blank
      if( clock >= 456){
        clock -= 456;
        LY++;
        if(LY > 153){
          STAT &= 0xFC; STAT |= 0x02; // mode 2
          LY = 0;
        }
      }
      break;
    }
    case 0x02:{ //Searching OAM-RAM //TODO blocks OAM
      if( clock >= 80){
        clock -= 80;
        STAT |= 0x03; // mode 3
      }
      break;
    }
    case 0x03:{ //Transfering Data to LCD Driver //TODO blocks OAM and VRAM
      if( clock >= 172){
        clock -= 172;
        STAT &= 0xFC; // mode 0
        draw_line(); //TODO
      }
      break;
    }
    default:{ 
      std::cout << "GPU: STAT ERROR" << std::endl;
      exit(1);
      break;
    }
  }
}

void GPU::draw_line(){
  if ((LCDC & 0x80) == 0x00){ //OFF DISPLAY
    for(int m = 0; m<160; m++){ // for each pìxel in line
      bufferImage[4*(LY*160 + m) + 2] = 0xE0; //clear line
      bufferImage[4*(LY*160 + m) + 1] = 0xF8;
      bufferImage[4*(LY*160 + m) + 0] = 0xD0;
    }
    return;
  }

  //auto start = std::chrono::high_resolution_clock::now();
  const std::vector<uint8_t> backgroundPalette = {
    static_cast<uint8_t>(BGP & 0x03),
    static_cast<uint8_t>((BGP & 0x0C)>>2),
    static_cast<uint8_t>((BGP & 0x30)>>4),
    static_cast<uint8_t>((BGP & 0xC0)>>6)
  };
  const std::vector<uint8_t> ObjectPalette0 = {
    static_cast<uint8_t>(OBP0 & 0x03),
    static_cast<uint8_t>((OBP0 & 0x0C)>>2),
    static_cast<uint8_t>((OBP0 & 0x30)>>4),
    static_cast<uint8_t>((OBP0 & 0xC0)>>6)
  };
  const std::vector<uint8_t> ObjectPalette1 = {
    static_cast<uint8_t>(OBP1 & 0x03),
    static_cast<uint8_t>((OBP1 & 0x0C)>>2),
    static_cast<uint8_t>((OBP1 & 0x30)>>4),
    static_cast<uint8_t>((OBP1 & 0xC0)>>6)
  };
      
  uint16_t tile_data_addr = ((LCDC & 0x10) == 0) ? 0x8800 : 0x8000; //BIT4 tile data select
  uint16_t tile_map_addr = ((LCDC & 0x08) == 0) ? 0x9800 : 0x9C00; //BIT3 tile map select
  
  for(int m = 0; m<160; m++){ // for each pìxel in line
    //WHICH TILE
    uint8_t tile_X = ( ( (m + SCX ) % 256 ) / 8 );
    uint8_t tile_Y = ( ( (LY + SCY ) % 256 ) / 8 );
    //GET TILE ID
    uint8_t tile_ID = VRAM[(tile_map_addr + 32*tile_Y + tile_X - 0x8000)]; //OK 0x01
    if((LCDC & 0x10) == 0){ //ADJUST signed case
      tile_ID += 0x80;
    }
    // A TILE IS 16 BYTES long. There are 256 -> 0x1000 BYTES.
    
    // WHICH PIXEL
    uint8_t data_X = ( ( m + SCX )  % 8 ); // % 256 % 8 == % 8
    uint8_t data_Y = ( (LY + SCY ) % 8 );
    uint8_t x_bit_mask = (0x1 << (7 - data_X)); //mask for data_x'th bit
    //GET PIXEL
    /*
    76543210 
    HHHHHHHH byte 0
    LLLLLLLL byte 1 
    HL = color code
    */
    uint16_t data_addr = tile_data_addr + (tile_ID << 4 ) + (data_Y << 1) - 0x8000;
    uint8_t codeL = ( ( VRAM[data_addr] & x_bit_mask ) >> (7-data_X) );
    uint8_t codeH = ( ( VRAM[data_addr + 0x1] & x_bit_mask ) >> (7-data_X) ); //OK
    uint8_t color = backgroundPalette[((codeH << 1) + codeL)];
    
    //STORE PIXEL BG
    if((LCDC & 0x01) != 0){
      bufferImage[4*(LY*160 + m) + 2] = COLOR[3*color];
      bufferImage[4*(LY*160 + m) + 1] = COLOR[3*color+1];
      bufferImage[4*(LY*160 + m) + 0] = COLOR[3*color+2];
    }

    if((LCDC & 0x20) != 0){ //WINDOW DISPLAY ENABLED
      uint16_t wd_map_addr = ((LCDC & 0x40) == 0) ? 0x9800 : 0x9C00; //BIT6 tile data select
      if( ( 0 <= (m - WX + 7) ) && ( (m - WX + 7) < 160 ) ){
        if( ( 0 <= (LY - WY) ) && ( (LY - WY) < 144 ) ){
              //WHICH TILE
              uint8_t wd_tile_X = ( (m - WX + 7) / 8 );
              uint8_t wd_tile_Y = ( (LY - WY ) / 8 );
              //GET TILE ID
              uint8_t wd_tile_ID = VRAM[(wd_map_addr + 32*wd_tile_Y + wd_tile_X - 0x8000)]; //OK 0x01
              if((LCDC & 0x10) == 0){ //ADJUST signed case
                wd_tile_ID += 0x80;
              }
              // A TILE IS 16 BYTES long. There are 256 -> 0x1000 BYTES.
              
              // WHICH PIXEL
              uint8_t wd_data_X = ( (m - WX + 7)  % 8 ); // % 256 % 8 == % 8
              uint8_t wd_data_Y = ( (LY - WY ) % 8 );
              uint8_t wd_x_bit_mask = (0x1 << (7 - wd_data_X)); //mask for data_x'th bit
              //GET PIXEL
              /*
              76543210 
              HHHHHHHH byte 0
              LLLLLLLL byte 1 
              HL = color code
              */
              uint16_t wd_data_addr = tile_data_addr + (wd_tile_ID << 4 ) + (wd_data_Y << 1) - 0x8000;
              uint8_t wd_codeL = ( ( VRAM[wd_data_addr] & wd_x_bit_mask ) >> (7-wd_data_X) );
              uint8_t wd_codeH = ( ( VRAM[wd_data_addr + 0x1] & wd_x_bit_mask ) >> (7-wd_data_X) ); //OK
              uint8_t wd_color = backgroundPalette[((wd_codeH << 1) + wd_codeL)];
              
              //STORE PIXEL WD
              bufferImage[4*(LY*160 + m) + 2] = COLOR[3*wd_color];
              bufferImage[4*(LY*160 + m) + 1] = COLOR[3*wd_color+1];
              bufferImage[4*(LY*160 + m) + 0] = COLOR[3*wd_color+2];
              
        }
      }
    }
    
    /// SPRITES ///
    if((LCDC & 0x02) != 0){
      uint8_t spriteSize = ((LCDC & 0x04) == 0) ? 8 : 16; // sprite size
    //<-
      for(int spr = 0; spr < 40; spr++){ //SEARCH WHICH SPRITES MUST BE DRAWN
        if( ( OAMRAM[4*spr] - 16 <= LY ) && ( LY < OAMRAM[4*spr] - 16 + spriteSize) ){
          if( (OAMRAM[4*spr + 1] - 8 <= m ) && ( m < OAMRAM[4*spr + 1] )){
            
            uint8_t sprite_ID = OAMRAM[4*spr + 2];
            uint8_t sprite_Y = LY - OAMRAM[4*spr] + 16;
            uint8_t sprite_X = m - OAMRAM[4*spr + 1] + 8;
            uint8_t flip_X = (OAMRAM[4*spr+3] & 0x20) ? 0x1 : 0x0;
            uint8_t flip_Y = (OAMRAM[4*spr+3] & 0x40) ? 0x1 : 0x0;
            
            uint16_t sprite_addr = (sprite_ID << 4 ) + (flip_Y ? ( (spriteSize - 1 - sprite_Y) << 1) : (sprite_Y << 1) );

            uint8_t spr_X_bit = flip_X ? ( sprite_X) : (7 - sprite_X);
            uint8_t sCodeL = ( ( VRAM[sprite_addr] & (0x1 << spr_X_bit) ) >> spr_X_bit );
            uint8_t sCodeH = ( ( VRAM[sprite_addr + 0x1] & (0x1 << spr_X_bit) ) >> spr_X_bit ); //OK
            
            uint8_t sColor = ((OAMRAM[4*spr+3] & 0x10) == 0x00) ? ObjectPalette0[((sCodeH << 1) + sCodeL)] : ObjectPalette1[((sCodeH << 1) + sCodeL)];
   
            //STORE PIXEL if Priority condition:
            if((OAMRAM[4*spr+3] & 0x80) == 0x80){ //BG priority
              if(((codeH << 1) + codeL) == 0){
                if(((sCodeH << 1) + sCodeL) != 0){
                  bufferImage[4*(LY*160 + m) + 2] = COLOR[3*sColor];
                  bufferImage[4*(LY*160 + m) + 1] = COLOR[3*sColor+1];
                  bufferImage[4*(LY*160 + m) + 0] = COLOR[3*sColor+2];
                }
              }
            }
            if((OAMRAM[4*spr+3] & 0x80) == 0x00){ //SPR priority
              if(((sCodeH << 1) + sCodeL) != 0){
                bufferImage[4*(LY*160 + m) + 2] = COLOR[3*sColor];
                bufferImage[4*(LY*160 + m) + 1] = COLOR[3*sColor+1];
                bufferImage[4*(LY*160 + m) + 0] = COLOR[3*sColor+2];
              }
            }
            
          }
        }
      }
    }
    


  } //end for m
}
