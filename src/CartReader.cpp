#include "CartReader.h"

CartReader::CartReader()
//: ROM0(0x4000, 0), ROM1(0x4000, 0), ERAM(0x2000,0)
{}

void CartReader::loadROMfromFile(std::string romLocation){
	std::ifstream inFile;
	long size = 0;

	inFile.open( romLocation, std::ios::binary );
	inFile.seekg(0, std::ios::end); // set the pointer to the end
	size = inFile.tellg() ; // get the length of the file
	inFile.seekg(0, std::ios::beg); // set the pointer to the beginning

	char* buffer = new char[size];
	inFile.read( buffer, size );
  
  uint8_t cartType = static_cast<uint8_t>(buffer[0x147]);
  uint8_t romSize = static_cast<uint8_t>(buffer[0x148]);
  uint8_t ramSize = static_cast<uint8_t>(buffer[0x149]);

  std::cout << "Name data:";
  for(int i=0x134; i<0x144; i++){
    std::cout << buffer[i];
	}
  std::cout << std::endl;
  
  std::cout << "Cartidge type: " << std::to_string(cartType) << std::endl;
  std::cout << "File Size: " << (size / 1024) << "K" << std::endl;
  if(romSize < 8){
    std::cout << "Rom Size: " << ( 32 << romSize ) << std::endl;
  } else {
    std::cout << "Rom Size: " << "other: 1.1 1.2 1.5 M" << std::endl;
  }
  
  uint32_t ramBytesSize = 0;
  switch(ramSize){
    case 0:
      std::cout << "Ram Size: NONE" << std::endl;
      ramBytesSize = 0x0;
      break;
    case 1:
      std::cout << "Ram Size: 2K" << std::endl;
      ramBytesSize = 0x800;
      break;
    case 2:
      std::cout << "Ram Size: 8K" << std::endl;
      ramBytesSize = 0x2000;
      break;
    case 3:
      std::cout << "Ram Size: 32K" << std::endl;
      ramBytesSize = 0x2000 * 4;
      break;
    case 4:
      std::cout << "Ram Size: 128K" << std::endl;
      ramBytesSize = 0x2000 * 16;
      break;
    case 5:
      std::cout << "Ram Size: 64K" << std::endl;
      ramBytesSize = 0x2000 * 8;
      break;
    default:
      std::cout << "Ram Size: UNK" << std::endl;
      break;
  }
  
  switch(cartType){
    case 0x00:
      cart = new Cartidge_ROM_ONLY();
      break;
    case 0x01:
      cart = new Cartidge_MBC1();
      cart->allocateRAM(ramBytesSize);
      break;
    case 0x03: //TODO: RAM BATTERY
      cart = new Cartidge_MBC1();
      cart->allocateRAM(ramBytesSize);
      break;
    case 0x13: //TODO: RAM BATTERY, REAL TIME CLOCK
      cart = new Cartidge_MBC3();
      cart->allocateRAM(ramBytesSize);
      break;
    default:
      std::cout << "NOT IMPLEMENTED MBC" << std::endl;
      exit(1);
      break;
  }
  
  (cart->fullData).assign(size,0);
	for(int i=0; i<size; i++){
	    (cart->fullData)[i] = static_cast<uint8_t>(buffer[i]);
	}
	
	delete[] buffer;
	inFile.close();
}


