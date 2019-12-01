#include "Gameboy.h"

Gameboy::Gameboy() :
  gpu(io),
  io(gpu),
  mmu(mem, io, gpu, cart),
  cpu(mmu)
{
}

void Gameboy::emuCycle(){
  //auto start = std::chrono::high_resolution_clock::now();
  if(!cpu.handleInterrupt()){
    cpu.execute();
  }
  gpu.update(cpu.deltaCycles);
  io.timer_step(cpu.deltaCycles);
  //auto finish = std::chrono::high_resolution_clock::now();
  //std::chrono::duration<double> elapsed = finish - start;       
  //std::cout << "Elapsed time: " << elapsed.count() << " s\n";

}

uint16_t Gameboy::emuTestCycle(){
  uint16_t cpu_pc = cpu.debugPrintRegs();
  cpu.execute();
  gpu.update(cpu.deltaCycles);
  cpu.debugPrintInstruction();
  std::cout << "--------------" << cpu.cycles << " " << (int)cpu.deltaCycles << std::endl;
  return cpu_pc;
}

void Gameboy::LoadRom(std::string filename){
  try{
    cart.loadROMfromFile(filename);
  }
  catch(...){
    std::cout << "Rom not found" << std::endl;
    exit(1);
  }
}

void Gameboy::LoadBootRom(std::string filename){
  try{
    mem.loadBIOSfromFile(filename);
  }
  catch(...){
    std::cout << "Boot rom not found" << std::endl;
    exit(1);
  }
}

bool Gameboy::availableImage(){
  return gpu.available_image;
}

std::vector<uint8_t> Gameboy::getBufferedImage(){
  return gpu.getBufferImage();
}

uint32_t Gameboy::cycles(){
  return cpu.cycles;
}

void Gameboy::input(enum GB_Key keyEvent){
  //if ( (io.JOYP & 0x20) == 0x00){ //Button
    switch(keyEvent){
      case GB_Key::A_Press:
      case GB_Key::B_Press:
      case GB_Key::SELECT_Press:
      case GB_Key::START_Press:
        //std::cout << "BUTTON PRESS" << std::endl;
        io.input_buffer &= static_cast<uint8_t>(keyEvent); // set 0
        break;
      case GB_Key::A_Release:
      case GB_Key::B_Release:
      case GB_Key::SELECT_Release:
      case GB_Key::START_Release:
        //std::cout << "BUTTON RELEASE" << std::endl;
        io.input_buffer |= static_cast<uint8_t>(keyEvent); // set 1
        break;

      case GB_Key::RIGHT_Press:
      case GB_Key::LEFT_Press:
      case GB_Key::UP_Press:
      case GB_Key::DOWN_Press:
        //std::cout << "DIRECTION PRESS" << std::endl;
        io.input_buffer &= static_cast<uint8_t>(keyEvent); // set 0
        break;
      case GB_Key::RIGHT_Release:
      case GB_Key::LEFT_Release:
      case GB_Key::UP_Release:
      case GB_Key::DOWN_Release:
        //std::cout << "DIRECTION RELEASE" << std::endl;
        io.input_buffer |= static_cast<uint8_t>(keyEvent); // set 1
        break;
      default:
        break;
    }
  return;
}
