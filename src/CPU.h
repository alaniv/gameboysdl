#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <array>

#include "MMU.h"
#include "CPUOPCODES.h"

#define CLOCK cycles += 4; deltaCycles += 4;

union cpu_register {
    struct regPair {
        uint8_t L, H;
    } pair;
    uint16_t HiLo;
    cpu_register(uint16_t hilo) : HiLo(hilo){};
};

class CPU {

  private:
    //Registers
	  cpu_register AF_reg; // F = ZNHC 0000 zero, sub, halfcarry, carry.
      uint16_t& AF; // alias 
      uint8_t& A; // alias 
      uint8_t& F; // alias 
	  cpu_register BC_reg;
      uint16_t& BC; // alias 
      uint8_t& B; // alias 
      uint8_t& C; // alias
	  cpu_register DE_reg;
      uint16_t& DE; // alias 
      uint8_t& D; // alias
      uint8_t& E; // alias
	  cpu_register HL_reg;
      uint16_t& HL; // alias 
      uint8_t& H; // alias
      uint8_t& L; // alias
      uint16_t SP;
	  uint16_t PC;
	  
	  bool IME;
	  bool HALTed;
	
  	std::array<uint8_t, 3> instruction;
    
    MMU& _mmu;
    
  	inline uint16_t to16bits(uint8_t & Hi, uint8_t & Lo){
        return ( static_cast<uint16_t>( Hi ) << 8 ) + ( static_cast<uint16_t>( Lo ) );
    }
	
    uint8_t memRead(uint16_t addr); // 4 cycles
    void memWrite(uint16_t& addr, uint8_t& data); // 4 cycles
    
    inline void op_INC(uint8_t& reg);
    inline void op_DEC(uint8_t& reg);
    inline void op_ADD(uint8_t& reg);
    inline void op_ADC(uint8_t& reg);       
    inline void op_SUB(uint8_t& reg);
    inline void op_SBC(uint8_t& reg);
    inline void op_AND(uint8_t& reg);
    inline void op_OR(uint8_t& reg);    
    inline void op_XOR(uint8_t& reg);
    inline void op_JR(uint8_t& val); // 4 cycles
    inline void op_JP(uint16_t& val); // 4 cycles
    inline void op_CP(uint8_t& value);
    inline void op_CALL(uint16_t& addr); // 12 cycles
    inline void op_RET(); // 12 cycles
    inline void op_RST(uint16_t rst_addr); // 16 cycles
    inline void op_POP(uint8_t& Hi, uint8_t& Lo); // 8 cycles
    inline void op_PUSH(uint8_t& Hi, uint8_t& Lo); // 12 cycles

    void op_PREFIXCB(); //not inline
    
    inline void op_CB_RLC(uint8_t& reg);
    inline void op_CB_RRC(uint8_t& reg);
    inline void op_CB_RL(uint8_t& reg);
    inline void op_CB_RR(uint8_t& reg);
    inline void op_CB_SLA(uint8_t& reg);
    inline void op_CB_SRA(uint8_t& reg);
    inline void op_CB_SWAP(uint8_t& reg);
    inline void op_CB_SRL(uint8_t& reg);
    inline void op_CB_BIT(uint8_t bit, uint8_t& reg);
    inline void op_CB_RES(uint8_t bit, uint8_t& reg);
    inline void op_CB_SET(uint8_t bit, uint8_t& reg);
	
	
  public:
    uint8_t deltaCycles;
    uint32_t cycles;

    CPU(MMU& mmu);
    void execute();
    bool handleInterrupt();
    uint16_t debugPrintRegs(); //returns PC
    void debugPrintInstruction();
};

inline void CPU::op_INC(uint8_t& reg){
  F &= (0x01 << 4); //CLEAR F except F.C 
  F |= (( ( (reg & 0x0F) + (0x01 & 0x0F) ) & 0x10) << 1); // SET F.H if HC
  reg++;
  F |= (reg == 0x00) << 7; // SET F.Z if zero
}

inline void CPU::op_DEC(uint8_t& reg){ //TEST
  F &= (0x01 << 4); //CLEAR F except F.C 
  F |= (( ( (reg & 0x0F) - (0x01 & 0x0F) ) & 0x10) << 1); // SET F.H if HC
  reg--;
  F |= (reg == 0x00) << 7; // SET F.Z if zero
  F |= ((0x01) << 6); // SET F.N
}

void CPU::op_ADD(uint8_t& reg){
  ///z0hc
  F &= 0x00; //CLEAR F
  F |= (( ( (A & 0x0F) + (reg & 0x0F) ) & 0x10) << 1); // SET F.H if HC
  uint16_t addA = static_cast<uint16_t>(A) + static_cast<uint16_t>(reg);
  A = static_cast<uint8_t>(addA & 0x00FF);
  F |= static_cast<uint8_t>((addA & 0x0100) >> 4); // CARRY
  F |= (A == 0x00) << 7; // SET F.Z if zero
  F &= ~((0x01) << 6); // RESET F.N
}

inline void CPU::op_ADC(uint8_t& reg){
  ///z0hc
  uint8_t HC = (( ( (A & 0x0F) + (reg  & 0x0F) + ((F & 0x10) >> 4)  ) & 0x10) << 1); // SET F.H if HC
  uint16_t addA = static_cast<uint16_t>(A) + static_cast<uint16_t>(reg) + static_cast<uint16_t>((F & 0x10) >> 4) ;
  A = static_cast<uint8_t>(addA & 0x00FF);
  F &= 0x00; //CLEAR F
  F |= HC; //set fH if HC
  F |= static_cast<uint8_t>((addA & 0x0100) >> 4);
  F |= (A == 0x00) << 7; // SET F.Z if zero
  F &= ~((0x01) << 6); // RESET F.N
}

inline void CPU::op_SUB(uint8_t& reg){
  ///z1hc
  F &= 0x00; //CLEAR F
  F |= (( ( (A & 0x0F) - (reg & 0x0F) ) & 0x10) << 1); // SET F.H if HC
  uint16_t subA = static_cast<uint16_t>(A) - static_cast<uint16_t>(reg);
  A = static_cast<uint8_t>(subA & 0x00FF);
  F |= static_cast<uint8_t>((subA & 0x0100) >> 4);
  F |= (A == 0x00) << 7; // SET F.Z if zero
  F |= (0x01) << 6; // SET F.N
}

inline void CPU::op_SBC(uint8_t& reg){
  ///z1hc
  uint8_t HC = (( ( (A & 0x0F) - ( reg & 0x0F) - ( ((F & 0x10) >> 4) & 0x0F) ) & 0x10) << 1); // SET F.H if HC
  uint16_t subA = static_cast<uint16_t>(A) - static_cast<uint16_t>(reg) - static_cast<uint16_t>((F & 0x10) >> 4);
  A = static_cast<uint8_t>(subA & 0x00FF);
  F &= 0x00; //clear F
  F |= HC; //set f.H if HC
  F |= static_cast<uint8_t>((subA & 0x0100) >> 4);
  F |= (A == 0x00) << 7; // SET F.Z if zero
  F |= (0x01) << 6; // SET F.N
}

inline void CPU::op_AND(uint8_t& reg){
  A = A & reg;
  F = static_cast<uint8_t>((A == 0x00) << 7);
  F |= (0x01) << 5; // SET F.H
}

inline void CPU::op_OR(uint8_t& reg){
  A = A | reg;
  F = static_cast<uint8_t>((A == 0x00) << 7);
}

inline void CPU::op_XOR(uint8_t& reg){
  A = A ^ reg;
  F = static_cast<uint8_t>((A == 0x00) << 7);
}

inline void CPU::op_JR(uint8_t& val){ //OK
  PC += 0xFF00 * (val >> 7) + val; // sign extension
  CLOCK //when?
}

inline void CPU::op_JP(uint16_t& addr){
  PC = addr; // sign extension
  CLOCK //when?
}

inline void CPU::op_CP(uint8_t& value){
  // Z 1 H C
  F &= 0x00; //CLEAR F
  F |= (( ( (A & 0x0F) - (value & 0x0F) ) & 0x10) << 1); // SET F.H if HC
  uint16_t subA = static_cast<uint16_t>(A) - static_cast<uint16_t>(value);
  F |= static_cast<uint8_t>((subA & 0x0100) >> 4);
  F |= ((A - value) == 0x00) << 7; // SET F.Z if zero
  F |= (0x01) << 6; // SET F.N  
}

inline void CPU::op_CALL(uint16_t& addr){
  CLOCK // internal delay
  SP--;
  uint8_t hiPC = static_cast<uint8_t>( (PC & 0xFF00) >> 8);
  memWrite(SP, hiPC);
  uint8_t loPC = static_cast<uint8_t>(PC & 0x00FF);
  SP--;
  memWrite(SP, loPC);
  PC = addr;
}

inline void CPU::op_RET(){
  CLOCK // when?
  //uint8_t hiPC = static_cast<uint8_t>( (PC & 0xFF00) >> 8);
  uint8_t LoPC = memRead(SP);
  SP++;
  //uint8_t loPC = static_cast<uint8_t>(PC & 0x00FF);
  uint8_t HiPC = memRead(SP);
  SP++;
  PC = to16bits(HiPC, LoPC);
}

inline void CPU::op_RST(uint16_t rst_addr){
  CLOCK // internal delay
  SP--;
  uint8_t hiPC = static_cast<uint8_t>( (PC & 0xFF00) >> 8);
  memWrite(SP, hiPC);
  uint8_t loPC = static_cast<uint8_t>(PC & 0x00FF);
  SP--;
  memWrite(SP, loPC);
  PC = rst_addr;
}

inline void CPU::op_POP(uint8_t& Hi, uint8_t& Lo){
  Lo = memRead(SP);
  SP++;
  Hi = memRead(SP);
  SP++;
}

inline void CPU::op_PUSH(uint8_t& Hi, uint8_t& Lo){
  SP--;
  memWrite(SP, Hi);
  SP--;
  memWrite(SP, Lo);
  CLOCK // when?
}

inline void CPU::op_CB_RLC(uint8_t& reg){
  //Z 0 0 C
  uint8_t highBit = ((reg & 0x80) >> 7);
  reg = ( reg << 1) | highBit;
  F &= 0x00; // clear F
  F |= (highBit << 4);
  F |= ((reg == 0x00) << 7);
}

inline void CPU::op_CB_RRC(uint8_t& reg){
  //Z 0 0 C
  uint8_t lowBit = (reg & 0x01);
  reg = ( reg >> 1) | (lowBit << 7);
  F &= 0x00; // clear F
  F |= (lowBit << 4);
  F |= ((reg == 0x00) << 7);
}

inline void CPU::op_CB_SWAP(uint8_t& reg){
  reg = ( ((reg & 0x0F) << 4) | ((reg & 0xF0) >> 4) );
  F = 0x00; //CLEAR F
  F |= (( reg == 0x00 ) << 7); // SET F.Z if reg zero
}

inline void CPU::op_CB_SRL(uint8_t& reg){ 
  //Z 0 0 C
  uint8_t lowBit = (reg & 0x01);
  reg = (reg>>1);
  F = 0x00; //CLEAR F
  F |= (( reg == 0x00 ) << 7); // SET F.Z if reg zero
  F |= (lowBit << 4);
}

inline void CPU::op_CB_RL(uint8_t& reg){ //OK
  //Z 0 0 C
  uint8_t highBit = ((reg & 0x80) >> 7);
  reg = ( ( reg << 1) | ((F & 0x10) >> 4) ) & 0xFF;
  F &= 0x00; // clear F
  F |= (highBit << 4);
  F |= ((reg == 0x00) << 7);
}

inline void CPU::op_CB_RR(uint8_t& reg){
  //Z 0 0 C
  uint8_t lowBit = (reg & 0x01);
  reg = ( ( reg >> 1) | ((F & 0x10) << 3) ) & 0xFF;
  F &= 0x00; // clear F
  F |= (lowBit << 4);
  F |= ((reg == 0x00) << 7);
}

inline void CPU::op_CB_SLA(uint8_t& reg){
  //Z 0 0 C
  uint8_t highBit = ((reg & 0x80) >> 7);
  reg = ( ( reg << 1) );
  F &= 0x00; // clear F
  F |= (highBit << 4);
  F |= ((reg == 0x00) << 7);
}

inline void CPU::op_CB_SRA(uint8_t& reg){
  uint8_t highBit = (reg & 0x80);
  uint8_t lowBit = (reg & 0x01);
  reg = ( ( reg >> 1) | highBit ) & 0xFF;
  F &= 0x00; // clear F
  F |= (lowBit << 4);
  F |= ((reg == 0x00) << 7);
}

inline void CPU::op_CB_BIT(uint8_t bit, uint8_t& reg){ //OK
  F &= ~(0x01 << 7); //CLEAR F.Z
  F |= ( (((reg >> bit) & 0x01 ) == 0x00) << 7); // SET F.Z if bit set
  F &= ~(0x01 << 6); //CLEAR F.N
  F |= (0x01 << 5); //SET F.H
}

inline void CPU::op_CB_RES(uint8_t bit, uint8_t& reg){
  reg &= ~(0x01 << bit); //CLEAR BIT N
}

inline void CPU::op_CB_SET(uint8_t bit, uint8_t& reg){
  reg |= (0x01 << bit); //SET BIT N
}

#endif /* CPU_H */
