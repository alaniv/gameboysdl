#include "Memory.h"

Memory::Memory() : 
  BIOS(0x0100, 0), WRAM(0x2000, 0), ZRAM(0x80,0)
{}

void Memory::loadBIOSfromFile(std::string romLocation){
	std::ifstream inFile;
	long size = 0;

	inFile.open( romLocation, std::ios::binary );
	inFile.seekg(0, std::ios::end);
	size = inFile.tellg();
	inFile.seekg(0, std::ios::beg);

	char* buffer = new char[size];
	inFile.read( buffer, size );
	
	for(int i=0; i<0x0100; i++){
	    BIOS[i] = static_cast<uint8_t>(buffer[i]);
	}
	
	delete[] buffer;
	inFile.close();
}
