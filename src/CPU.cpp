#include "CPU.h"

CPU::CPU(MMU& mmu) : 
  AF_reg(0), AF(AF_reg.HiLo), A(AF_reg.pair.H), F(AF_reg.pair.L),
  BC_reg(0), BC(BC_reg.HiLo), B(BC_reg.pair.H), C(BC_reg.pair.L),
  DE_reg(0), DE(DE_reg.HiLo), D(DE_reg.pair.H), E(DE_reg.pair.L),
  HL_reg(0), HL(HL_reg.HiLo), H(HL_reg.pair.H), L(HL_reg.pair.L),
  SP(0), PC(0), 
  IME(true), HALTed(false),
  instruction({{0,0,0}}), 
  _mmu(mmu), deltaCycles(0), cycles(0)
{
}



uint8_t CPU::memRead(uint16_t addr){
  CLOCK
  return _mmu.read_addr(addr);
}

void CPU::memWrite(uint16_t& addr, uint8_t& data){
  CLOCK
  _mmu.write_addr(addr, data);
}

uint16_t CPU::debugPrintRegs(){
  std::cout << "B: " << std::to_string(B);
  std::cout << "\tC: " << std::to_string(C);
  std::cout << "\tD: " << std::to_string(D);
  std::cout << "\tE: " << std::to_string(E);
  std::cout << "\tH: " << std::to_string(H);
  std::cout << "\tL: " << std::to_string(L);
  std::cout << "\tA: " << std::to_string(A);
  std::cout << "\nPC: " << std::to_string(PC);
  std::cout << "\tSP: " << std::to_string(SP);
  std::cout << "\tF: " << std::to_string(F);
  std::cout << std::endl;
  return PC;
}

void CPU::debugPrintInstruction(){
  switch(OPCODE_SIZE[instruction[0]]){
    case 2:{
      std::string base = OPCODE[instruction[0]];
      base = base.replace( base.find("{1}"), 3, std::to_string(instruction[1]) );
      std::cout << base << std::endl;
      break;
    }
    case 3:{
      std::string base = OPCODE[instruction[0]];
      base = base.replace( base.find("{1}"), 3, std::to_string(instruction[1]) );
      base = base.replace( base.find("{2}"), 3, std::to_string(instruction[2]) );
      std::cout << base << std::endl;
      break;
    }
    default: //case 1:
      std::cout << OPCODE[instruction[0]] << std::endl;
  }
}



void CPU::execute(){
  if(HALTed)
    return;
  deltaCycles = 0;
  instruction[0] = memRead(PC); PC++;
  
  //THE BIG SWITCH
  switch(instruction[0]){
    case 0x00:{ //NOP
 	    break;
  	}
    case 0x01:{ //LD BC,d16
      instruction[2] = memRead(PC); PC++;
      instruction[1] = memRead(PC); PC++;
      B = instruction[1]; C = instruction[2];
   	  break;
	  }
    case 0x02:{ //LD (BC),A
      memWrite(BC, A);
 	  break;
  	}
    case 0x03:{ //INC BC
      CLOCK //when?
      BC++;
      break;
    }
    case 0x04:{ //INC B
      op_INC(B);
   	  break;
	  }
    case 0x05:{ //DEC B
      op_DEC(B);
   	  break;
	  }
    case 0x06:{ //LD B,d8
      instruction[1] = memRead(PC); PC++;
      B = instruction[1];
   	  break;
	  }
    case 0x07:{ //RLCA
      uint8_t highBit = (A & 0x80) >> 7;
      A = ((A << 1) | highBit );
      F &= 0x00; // clear F
      F |= highBit << 4;
   	  break;
	  }
    case 0x08:{ //LD (a16),SP 
      instruction[2] = memRead(PC); PC++;
      instruction[1] = memRead(PC); PC++;
      uint16_t addr = to16bits(instruction[1], instruction[2]);
      uint8_t hiSP = static_cast<uint8_t>( (SP & 0xFF00) >> 8);
      uint8_t loSP = static_cast<uint8_t>(SP & 0x00FF);
      memWrite(addr, loSP);
      addr++;
      memWrite(addr, hiSP);
   	  break;
	  }
    case 0x09:{ //ADD HL,BC 
      CLOCK //when?
      F &= (0x01 << 7); //CLEAR F except F.Z
      uint32_t sum = static_cast<uint32_t>(HL) + static_cast<uint32_t>(BC);
      uint16_t hcSum = (HL & 0x0FFF) + (BC & 0x0FFF);
      F |= ((sum & 0xFFFF0000) != 0x0) << 4; // SET F.C if Carry 15
      F |= ((hcSum & 0xF000) != 0x0) << 5; // SET F.HC if HC
      HL = sum & 0xFFFF;
   	  break;
	  }
    case 0x0A:{ //LD A,(BC)
      A = memRead(BC);
   	  break;
	  }
    case 0x0B:{ //DEC BC
      CLOCK //when?
      BC--;
      break;
    }
    case 0x0C:{ //INC C
      op_INC(C);
   	  break;
	  }
    case 0x0D:{ //DEC C
      op_DEC(C);
   	  break;
	  }
    case 0x0E:{ //LD C,d8
      instruction[1] = memRead(PC); PC++;
      C = instruction[1];
   	  break;
	  }
    case 0x0F:{ //RRCA
      uint8_t lowBit = (A & 0x01);
      A = ( ( A >> 1) | lowBit << 7 );
      F &= 0x00; // clear F
      F |= lowBit << 4;
   	  break;
	  }
    case 0x10:{ //STOP 
	    std::cout << "Not implemented opcode: 0x10 STOP" << std::endl;
      HALTed = true;
   	  break;
	  }
    case 0x11:{ //LD DE,d16
      instruction[2] = memRead(PC); PC++;
      instruction[1] = memRead(PC); PC++;
      D = instruction[1]; E = instruction[2];
   	  break;
	  }
    case 0x12:{ //LD (DE),A
      memWrite(DE, A);
 	    break;
  	}
    case 0x13:{ //INC DE
      CLOCK //when?
      DE++;
      break;
    }
    case 0x14:{ //INC D
      op_INC(D);
   	  break;
	  }
    case 0x15:{ //DEC D
      op_DEC(D);
   	  break;
	  }
    case 0x16:{ //LD D,d8
      instruction[1] = memRead(PC); PC++;
      D = instruction[1];
   	  break;
	  }
    case 0x17:{ //RLA OK
      uint8_t highBit = (A & 0x80) >> 7;
      A = ( ( A << 1) | ((F & 0x10) >> 4) ) & 0xFF;
      F &= 0x00; // clear F
      F |= highBit << 4;
   	  break;
	  }
    case 0x18:{ //JR r8
      instruction[1] = memRead(PC); PC++;
      op_JR(instruction[1]);
   	  break;
	  }
    case 0x19:{ //ADD HL,DE
      CLOCK
      F &= (0x01 << 7); //CLEAR F except F.Z
      uint32_t sum = static_cast<uint32_t>(HL) + static_cast<uint32_t>(DE);
      uint16_t hcSum = (HL & 0x0FFF) + (DE & 0x0FFF);
      F |= ((sum & 0xFFFF0000) != 0x0) << 4; // SET F.C if Carry 15
      F |= ((hcSum & 0xF000) != 0x0) << 5; // SET F.HC if HC
      HL = sum & 0xFFFF;
   	  break;
	  }
    case 0x1A:{ //LD A,(DE)
      A = memRead(DE);
   	  break;
	  }
    case 0x1B:{ //DEC DE
      CLOCK //when?
      DE--;
      break;
    }
    case 0x1C:{ //INC E
      op_INC(E);
   	  break;
	  }
    case 0x1D:{ //DEC E
      op_DEC(E);
   	  break;
	  }
    case 0x1E:{ //LD E,d8
      instruction[1] = memRead(PC); PC++;
      E = instruction[1];
   	  break;
	  }
    case 0x1F:{ //RRA
      uint8_t lowBit = (A & 0x01);
      A = ( ( A >> 1) | ((F & 0x10) << 3) );
      F &= 0x00; // clear F
      F |= lowBit << 4;
   	  break;
	  }
    case 0x20:{ //JR NZ,r8
      instruction[1] = memRead(PC); PC++;
      if (!(F & 0x80)){ // NOT ZERO
        op_JR(instruction[1]);
      }
   	  break;
	  }
    case 0x21:{ //LD HL,d16
      instruction[2] = memRead(PC); PC++;
      instruction[1] = memRead(PC); PC++;
      H = instruction[1]; L = instruction[2];
   	  break;
	  }
    case 0x22:{ //LD (HL+),A
      memWrite(HL, A);
      HL++;
 	  break;
  	}
    case 0x23:{ //INC HL
      CLOCK //when?
      HL++;
      break;
    }
    case 0x24:{ //INC H
      op_INC(H);
   	  break;
	  }
    case 0x25:{ //DEC H
      op_DEC(H);
   	  break;
	  }
    case 0x26:{ //LD H,d8
      instruction[1] = memRead(PC); PC++;
      H = instruction[1];
   	  break;
	  }
    case 0x27:{ //DAA
      uint8_t setFlagC = 0x00;
      uint8_t correction = 0x00;
      if(((F & 0x20) != 0x00) || (((F & 0x40) == 0x00) && ((A & 0x0F) > 0x09))){
        correction |= 0x06;
      }
      if(((F & 0x10) != 0x00) || (((F & 0x40) == 0x00) && ((A & 0xFF) > 0x99))){
        correction |= 0x60;
        setFlagC = 0x10;
      }
      
      if ((F & 0x40) != 0x00) {
        A -= correction;
      } else {
        A += correction;
      }
      F &= 0x40; //clear all except N
      
      F |= (A == 0x00) << 7; // SET F.Z if zero
      F |= setFlagC;
   	  break;
	  }
    case 0x28:{ //JR Z,r8
      instruction[1] = memRead(PC); PC++;
      if (F & 0x80){ // ZERO
        op_JR(instruction[1]);
      }
   	  break;
	  }
    case 0x29:{ //ADD HL,HL
      CLOCK
      F &= (0x01 << 7); //CLEAR F except F.Z
      uint32_t sum = static_cast<uint32_t>(HL) + static_cast<uint32_t>(HL);
      uint16_t hcSum = (HL & 0x0FFF) + (HL & 0x0FFF);
      F |= ((sum & 0xFFFF0000) != 0x0) << 4; // SET F.C if Carry 15
      F |= ((hcSum & 0xF000) != 0x0) << 5; // SET F.HC if HC
      HL = sum & 0xFFFF;
   	  break;
	  }
    case 0x2A:{ //LD A,(HL+)      
      A = memRead(HL);
      HL++;
   	  break;
  	}
    case 0x2B:{ //DEC HL
      CLOCK
      HL--;
      break;
    }
    case 0x2C:{ //INC L
      op_INC(L);
   	  break;
	  }
    case 0x2D:{ //DEC L
      op_DEC(L);
   	  break;
	  }
    case 0x2E:{ //LD L,d8
      instruction[1] = memRead(PC); PC++;
      L = instruction[1];
   	  break;
	  }
    case 0x2F:{ //CPL
      A = A ^ 0xFF;
      F |= (0x01) << 6; // SET F.N
      F |= (0x01) << 5; // SET F.H
   	  break;
	  }
    case 0x30:{ //JR NC,r8
      instruction[1] = memRead(PC); PC++;
      if (!(F & 0x10)){ // NOT CARRY
        op_JR(instruction[1]);
      }
   	  break;
	  }
    case 0x31:{ //LD SP,d16
      instruction[2] = memRead(PC); PC++;
      instruction[1] = memRead(PC); PC++;
      SP = to16bits(instruction[1], instruction[2]);
   	  break;
	  }
    case 0x32:{ //LD (HL-),A
      memWrite(HL, A);
      HL--;
 	  break;
  	}
    case 0x33:{ //INC SP
      CLOCK //when?
      SP++;
      break;
    }
    case 0x34:{ //INC (HL)
      uint8_t val = memRead(HL);
      op_INC(val);
      memWrite(HL, val);
   	  break;
	  }
    case 0x35:{ //DEC (HL)
      uint8_t val = memRead(HL);
      op_DEC(val);
      memWrite(HL, val);
   	  break;
	  }
    case 0x36:{ //LD (HL),d8
      instruction[1] = memRead(PC); PC++;
      memWrite(HL, instruction[1]);
 	    break;
  	}
    case 0x37:{ //SCF 
      F &= ~((0x01) << 6); // RESET F.N
      F &= ~((0x01) << 5); // RESET F.H
      F |= (0x01 << 4 ); // SET F.C
   	  break;
	  }
    case 0x38:{ //JR C,r8
      instruction[1] = memRead(PC); PC++;
      if (F & 0x10){ // CARRY
        op_JR(instruction[1]);
      }
   	  break;
	  }
    case 0x39:{ //ADD HL,SP
      CLOCK
      F &= (0x01 << 7); //CLEAR F except F.Z
      uint32_t sum = static_cast<uint32_t>(HL) + static_cast<uint32_t>(SP);
      uint16_t hcSum = (HL & 0x0FFF) + (SP & 0x0FFF);
      F |= ((sum & 0xFFFF0000) != 0x0) << 4; // SET F.C if Carry 15
      F |= ((hcSum & 0xF000) != 0x0) << 5; // SET F.HC if HC
      HL = sum & 0xFFFF;
   	  break;
	  }
    case 0x3A:{ //LD A,(HL-)      
        A = memRead(HL);
        HL--;
        break;
  	}
    case 0x3B:{ //DEC SP
      CLOCK //when?
      SP--;
      break;
    }
    case 0x3C:{ //INC A
      op_INC(A);
   	  break;
	  }
    case 0x3D:{ //DEC A
      op_DEC(A);
   	  break;
	  }
    case 0x3E:{ //LD A,d8
      instruction[1] = memRead(PC); PC++;
      A = instruction[1];
   	  break;
	  }
    case 0x3F:{ //CCF 
      F &= ~((0x01) << 6); // RESET F.N
      F &= ~((0x01) << 5); // RESET F.H
      if((F & 0x10) != 0x0){
        F &= ~((0x01) << 4); // RESET F.C
      } else {
        F |= (0x01 << 4);
      }
   	  break;
	  }
    case 0x40:{ //LD B,B
      B = B;
   	  break;
	  }
    case 0x41:{ //LD B,C
      B = C;
   	  break;
	  }
    case 0x42:{ //LD B,D
      B = D;
   	  break;
	  }
    case 0x43:{ //LD B,E
      B = E;
   	  break;
	  }
    case 0x44:{ //LD B,H
      B = H;
   	  break;
	  }
    case 0x45:{ //LD B,L
      B = L;
   	  break;
	  }
    case 0x46:{ //LD B,(HL)
      B = memRead(HL);
   	  break;
	  }
    case 0x47:{ //LD B,A
      B = A;
   	  break;
	  }
    case 0x48:{ //LD C,B
      C = B;
   	  break;
	  }
    case 0x49:{ //LD C,C
      C = C;
   	  break;
	  }
    case 0x4A:{ //LD C,D
      C = D;
   	  break;
	  }
    case 0x4B:{ //LD C,E
      C = E;
   	  break;
	  }
    case 0x4C:{ //LD C,H
      C = H;
   	  break;
	  }
    case 0x4D:{ //LD C,L
      C = L;
   	  break;
	  }
    case 0x4E:{ //LD C,(HL)
      C = memRead(HL);
   	  break;
	  }
    case 0x4F:{ //LD C,A
      C = A;
   	  break;
	  }
    case 0x50:{ //LD D,B
      D = B;
   	  break;
	  }
    case 0x51:{ //LD D,C
      D = C;
   	  break;
	  }
    case 0x52:{ //LD D,D
      D = D;
   	  break;
	  }
    case 0x53:{ //LD D,E
      D = E;
   	  break;
	  }
    case 0x54:{ //LD D,H
      D = H;
   	  break;
	  }
    case 0x55:{ //LD D,L
      D = L;
   	  break;
	  }
    case 0x56:{ //LD D,(HL)
      D = memRead(HL);
   	  break;
	  }
    case 0x57:{ //LD D,A
      D = A;
   	  break;
	  }
    case 0x58:{ //LD E,B
      E = B;
   	  break;
	  }
    case 0x59:{ //LD E,C
      E = C;
   	  break;
	  }
    case 0x5A:{ //LD E,D
      E = D;
   	  break;
	  }
    case 0x5B:{ //LD E,E
      E = E;
   	  break;
	  }
    case 0x5C:{ //LD E,H
      E = H;
   	  break;
	  }
    case 0x5D:{ //LD E,L
      E = L;
   	  break;
	  }
    case 0x5E:{ //LD E,(HL)
      E = memRead(HL);
   	  break;
	  }
    case 0x5F:{ //LD E,A
      E = A;
   	  break;
	  }
    case 0x60:{ //LD H,B
      H = B;
   	  break;
	  }
    case 0x61:{ //LD H,C
      H = C;
   	  break;
	  }
    case 0x62:{ //LD H,D
      H = D;
   	  break;
	  }
    case 0x63:{ //LD H,E
      H = E;
   	  break;
	  }
    case 0x64:{ //LD H,H
      H = H;
   	  break;
	  }
    case 0x65:{ //LD H,L
      H = L;
   	  break;
	  }
    case 0x66:{ //LD H,(HL)
      H = memRead(HL);
   	  break;
	  }
    case 0x67:{ //LD H,A
      H = A;
   	  break;
	  }
    case 0x68:{ //LD L,B
      L = B;
   	  break;
	  }
    case 0x69:{ //LD L,C
      L = C;
   	  break;
	  }
    case 0x6A:{ //LD L,D
      L = D;
   	  break;
	  }
    case 0x6B:{ //LD L,E
      L = E;
   	  break;
	  }
    case 0x6C:{ //LD L,H
      L = H;
   	  break;
	  }
    case 0x6D:{ //LD L,L
      L = L;
   	  break;
	  }
    case 0x6E:{ //LD L,(HL)
      L = memRead(HL);
   	  break;
	  }
    case 0x6F:{ //LD L,A
      L = A;
   	  break;
	  }
    case 0x70:{ //LD (HL),B
      memWrite(HL, B);
 	    break;
  	}
    case 0x71:{ //LD (HL),C
      memWrite(HL, C);
 	    break;
  	}
    case 0x72:{ //LD (HL),D
      memWrite(HL, D);
 	    break;
  	}
    case 0x73:{ //LD (HL),E
      memWrite(HL, E);
 	    break;
  	}
    case 0x74:{ //LD (HL),H
      memWrite(HL, H);
 	    break;
  	}
    case 0x75:{ //LD (HL),L
      memWrite(HL, L);
 	    break;
  	}
    case 0x76:{ //HALT
      //std::cout << "HALT" << std::endl;
      HALTed = true;
 	    break;
  	}
    case 0x77:{ //LD (HL),A
      memWrite(HL, A);
 	    break;
  	}
    case 0x78:{ //LD A,B
      A = B;
   	  break;
	  }
    case 0x79:{ //LD A,C
      A = C;
   	  break;
	  }
    case 0x7A:{ //LD A,D
      A = D;
   	  break;
	  }
    case 0x7B:{ //LD A,E
      A = E;
   	  break;
	  }
    case 0x7C:{ //LD A,H
      A = H;
   	  break;
	  }
    case 0x7D:{ //LD A,L
      A = L;
   	  break;
	  }
    case 0x7E:{ //LD A,(HL)
      A = memRead(HL);
   	  break;
	  }
    case 0x7F:{ //LD A,A
      A = A;
   	  break;
	  }
    case 0x80:{ //ADD B
 	    op_ADD(B);
   	  break;
	  }
    case 0x81:{ //ADD C
 	    op_ADD(C);
   	  break;
	  }
    case 0x82:{ //ADD D
 	    op_ADD(D);
   	  break;
	  }
    case 0x83:{ //ADD E
 	    op_ADD(E);
   	  break;
	  }
    case 0x84:{ //ADD H
 	    op_ADD(H);
   	  break;
	  }
    case 0x85:{ //ADD L
 	    op_ADD(L);
   	  break;
	  }
    case 0x86:{ //ADD (HL)
      uint8_t value = memRead(HL);
      op_ADD(value);
   	  break;
	  }
    case 0x87:{ //ADD A
 	    op_ADD(A);
   	  break;
	  }
    case 0x88:{ //ADC A,B
 	    op_ADC(B);
   	  break;
	  }
    case 0x89:{ //ADC A,C
 	    op_ADC(C);
   	  break;
	  }
    case 0x8A:{ //ADC A,D
 	    op_ADC(D);
   	  break;
	  }
    case 0x8B:{ //ADC A,E
 	    op_ADC(E);
   	  break;
	  }
    case 0x8C:{ //ADC A,H
 	    op_ADC(H);
   	  break;
	  }
    case 0x8D:{ //ADC A,L
 	    op_ADC(L);
   	  break;
	  }
    case 0x8E:{ //ADC A,(HL)
      uint8_t value = memRead(HL);
      op_ADC(value);
   	  break;
	  }
    case 0x8F:{ //ADC A,A
 	    op_ADC(A);
   	  break;
	  }
    case 0x90:{ //SUB B
 	    op_SUB(B);
   	  break;
	  }
    case 0x91:{ //SUB C
 	    op_SUB(C);
   	  break;
	  }
    case 0x92:{ //SUB D
 	    op_SUB(D);
   	  break;
	  }
    case 0x93:{ //SUB E
 	    op_SUB(E);
   	  break;
	  }
    case 0x94:{ //SUB H
 	    op_SUB(H);
   	  break;
	  }
    case 0x95:{ //SUB L
 	    op_SUB(L);
   	  break;
	  }
    case 0x96:{ //SUB (HL)
      uint8_t value = memRead(HL);
      op_SUB(value);
   	  break;
	  }
    case 0x97:{ //SUB A
 	    op_SUB(A);
   	  break;
	  }
    case 0x98:{ //SBC A,B
 	    op_SBC(B);
   	  break;
	  }
    case 0x99:{ //SBC A,C
 	    op_SBC(C);
   	  break;
	  }
    case 0x9A:{ //SBC A,D
 	    op_SBC(D);
   	  break;
	  }
    case 0x9B:{ //SBC A,E
 	    op_SBC(E);
   	  break;
	  }
    case 0x9C:{ //SBC A,H
 	    op_SBC(H);
   	  break;
	  }
    case 0x9D:{ //SBC A,L
 	    op_SBC(L);
   	  break;
	  }
    case 0x9E:{ //SBC A,(HL)
      uint8_t value = memRead(HL);
      op_SBC(value);
   	  break;
	  }
    case 0x9F:{ //SBC A,A
 	    op_SBC(A);
   	  break;
	  }
    case 0xA0:{ //AND B
      op_AND(B);
  	  break;
  	}
    case 0xA1:{ //AND C
      op_AND(C);
  	  break;
  	}
    case 0xA2:{ //AND D
      op_AND(D);
  	  break;
  	}
    case 0xA3:{ //AND E
      op_AND(E);
  	  break;
  	}
    case 0xA4:{ //AND H
      op_AND(H);
  	  break;
  	}
    case 0xA5:{ //AND L
      op_AND(L);
  	  break;
  	}
    case 0xA6:{ //AND (HL)
      uint8_t value = memRead(HL);
      op_AND(value);
  	  break;
  	}
    case 0xA7:{ //AND A
      op_AND(A);
  	  break;
  	}
    case 0xA8:{ //XOR B
      op_XOR(B);
  	  break;
  	}
    case 0xA9:{ //XOR C
      op_XOR(C);
  	  break;
  	}
    case 0xAA:{ //XOR D
      op_XOR(D);
  	  break;
  	}
    case 0xAB:{ //XOR E
      op_XOR(E);
  	  break;
  	}
    case 0xAC:{ //XOR H
      op_XOR(H);
  	  break;
  	}
    case 0xAD:{ //XOR L
      op_XOR(L);
  	  break;
  	}
    case 0xAE:{ //XOR (HL)
      uint8_t value = memRead(HL);
      op_XOR(value);
  	  break;
  	}
    case 0xAF:{ //XOR A
      op_XOR(A);
  	  break;
  	}
    case 0xB0:{ //OR B
      op_OR(B);
  	  break;
  	}
    case 0xB1:{ //OR C
      op_OR(C);
  	  break;
  	}
    case 0xB2:{ //OR D
      op_OR(D);
  	  break;
  	}
    case 0xB3:{ //OR E
      op_OR(E);
  	  break;
  	}
    case 0xB4:{ //OR H
      op_OR(H);
  	  break;
  	}
    case 0xB5:{ //OR L
      op_OR(L);
  	  break;
  	}
    case 0xB6:{ //OR (HL)
      uint8_t value = memRead(HL);
      op_OR(value);
  	  break;
  	}
    case 0xB7:{ //OR A
      op_OR(A);
  	  break;
  	}
    case 0xB8:{ //CP B
      op_CP(B);
  	  break;
  	}
    case 0xB9:{ //CP C
      op_CP(C);
  	  break;
  	}
    case 0xBA:{ //CP D
      op_CP(D);
  	  break;
  	}
    case 0xBB:{ //CP E
      op_CP(E);
  	  break;
  	}
    case 0xBC:{ //CP H
      op_CP(H);
  	  break;
  	}
    case 0xBD:{ //CP L
      op_CP(L);
  	  break;
  	}
    case 0xBE:{ //CP (HL)
      uint8_t value = memRead(HL);
      op_CP(value);
  	  break;
  	}
    case 0xBF:{ //CP A
      op_CP(A);
  	  break;
  	}
    case 0xC0:{ //RET NZ 
      CLOCK //when?
      if (!(F & 0x80)){ // NOT ZERO
        op_RET();
      }
  	  break;
  	}
    case 0xC1:{ //POP BC 
      op_POP(B, C);
  	  break;
  	}
    case 0xC2:{ //JP NZ,a16
      instruction[2] = memRead(PC); PC++;
      instruction[1] = memRead(PC); PC++;
      uint16_t addr = to16bits(instruction[1], instruction[2]);
      if (!(F & 0x80)){ // NOT ZERO
        op_JP(addr);
      }
   	  break;
	  }
    case 0xC3:{ //JP a16
      instruction[2] = memRead(PC); PC++;
      instruction[1] = memRead(PC); PC++;
      uint16_t addr = to16bits(instruction[1], instruction[2]);
      op_JP(addr);
  	  break;
  	}
    case 0xC4:{ //CALL NZ,a16
      instruction[2] = memRead(PC); PC++;
      instruction[1] = memRead(PC); PC++;
      uint16_t addr = to16bits(instruction[1], instruction[2]);
      if (!(F & 0x80)){ // NOT ZERO
        op_CALL(addr);
      }
   	  break;
	  }
    case 0xC5:{ //PUSH BC
      op_PUSH(B, C);
  	  break;
  	}
    case 0xC6:{ //ADD d8
      instruction[1] = memRead(PC); PC++;
      op_ADD(instruction[1]);
   	  break;
	  }
    case 0xC7:{ //RST 00H
      op_RST(0x0000);
   	  break;
	  }
    case 0xC8:{ //RET Z
      CLOCK //when?
      if (F & 0x80){ // ZERO
        op_RET();
      }
  	  break;
  	}
    case 0xC9:{ //RET
      op_RET();
  	  break;
  	}
    case 0xCA:{ //JP Z,a16
      instruction[2] = memRead(PC); PC++;
      instruction[1] = memRead(PC); PC++;
      uint16_t addr = to16bits(instruction[1], instruction[2]);
      if (F & 0x80){ // ZERO
        op_JP(addr);
      }
   	  break;
	  }
    case 0xCB:{ //PREFIX CB
      op_PREFIXCB();
  	  break;
  	}
    case 0xCC:{ //CALL Z,a16
      instruction[2] = memRead(PC); PC++;
      instruction[1] = memRead(PC); PC++;
      uint16_t addr = to16bits(instruction[1], instruction[2]);
      if (F & 0x80){ // ZERO
        op_CALL(addr);
      }
   	  break;
	  }
    case 0xCD:{ //CALL a16
      instruction[2] = memRead(PC); PC++;
      instruction[1] = memRead(PC); PC++;
      uint16_t addr = to16bits(instruction[1], instruction[2]);
      op_CALL(addr);
  	  break;
  	}
    case 0xCE:{ //ADC d8
      instruction[1] = memRead(PC); PC++;
      op_ADC(instruction[1]);
   	  break;
	  }
    case 0xCF:{ //RST 08H
      op_RST(0x0008);
   	  break;
	  }
    case 0xD0:{ //RET NC
      CLOCK //when?
      if (!(F & 0x10)){ // NOT CARRY
        op_RET();
      }
  	  break;
  	}
    case 0xD1:{ //POP DE 
      op_POP(D, E);
  	  break;
  	}
    case 0xD2:{ //JP NC,a16
      instruction[2] = memRead(PC); PC++;
      instruction[1] = memRead(PC); PC++;
      uint16_t addr = to16bits(instruction[1], instruction[2]);
      if (!(F & 0x10)){ // NOT CARRY
        op_JP(addr);
      }
   	  break;
	  }
    case 0xD4:{ //CALL NC,a16
      instruction[2] = memRead(PC); PC++;
      instruction[1] = memRead(PC); PC++;
      uint16_t addr = to16bits(instruction[1], instruction[2]);
      if (!(F & 0x10)){ // NOT CARRY
        op_CALL(addr);
      }
   	  break;
	  }
    case 0xD5:{ //PUSH DE
      op_PUSH(D, E);
  	  break;
  	}
    case 0xD6:{ //SUB d8
      instruction[1] = memRead(PC); PC++;
      op_SUB(instruction[1]);
   	  break;
	  }
    case 0xD7:{ //RST 10H
      op_RST(0x0010);
   	  break;
	  }
    case 0xD8:{ //RET C
      CLOCK //when?
      if (F & 0x10){ // ZERO
        op_RET();
      }
  	  break;
  	}
    case 0xD9:{ //RETI
      //std::cout << "RETI" << std::endl;
      op_RET();
      IME = true;
   	  break;
	  }
    case 0xDA:{ //JP C,a16
      instruction[2] = memRead(PC); PC++;
      instruction[1] = memRead(PC); PC++;
      uint16_t addr = to16bits(instruction[1], instruction[2]);
      if (F & 0x10){ // CARRY
        op_JP(addr);
      }
   	  break;
	  }
    case 0xDC:{ //CALL C,a16
      instruction[2] = memRead(PC); PC++;
      instruction[1] = memRead(PC); PC++;
      uint16_t addr = to16bits(instruction[1], instruction[2]);
      if (F & 0x10){ // CARRY
        op_CALL(addr);
      }
   	  break;
	  }
    case 0xDE:{ //SBC d8
      instruction[1] = memRead(PC); PC++;
      op_SBC(instruction[1]);
   	  break;
	  }
    case 0xDF:{ //RST 18H
      op_RST(0x0018);
   	  break;
	  }
    case 0xE0:{ //LDH (a8),A //LD ($FF00 + a8),A
      instruction[1] = memRead(PC); PC++;
      uint16_t IOaddr = 0xFF00 + static_cast<uint16_t>(instruction[1]);
      memWrite(IOaddr, A);
   	  break;
	  }
    case 0xE1:{ //POP HL
      op_POP(H, L);
  	  break;
  	}
    case 0xE2:{ //LD (C),A //LD ($FF00 + C),A
      uint16_t IOaddr = 0xFF00 + static_cast<uint16_t>(C);
      memWrite(IOaddr, A);
   	  break;
	  }
    case 0xE5:{ //PUSH HL
      op_PUSH(H, L);
  	  break;
  	}
    case 0xE6:{ //AND d8
      instruction[1] = memRead(PC); PC++;
      op_AND(instruction[1]);
   	  break;
	  }
    case 0xE7:{ //RST 20H
      op_RST(0x0020);
   	  break;
	  }
    case 0xE8:{ //ADD SP,r8
      ///00hc
      instruction[1] = memRead(PC); PC++;
      F &= 0x00; //CLEAR F
      
      uint16_t csum = (SP & 0xFF) + instruction[1];
      uint16_t hcSum = (SP & 0x0F) + (instruction[1] & 0x0F);
      F |= ((csum & 0xFF00) != 0x0) << 4; // SET F.C if Carry 15
      F |= ((hcSum & 0xF0) != 0x0) << 5; // SET F.HC if HC
      SP += 0xFF00 * (instruction[1] >> 7) + instruction[1];
      CLOCK
      CLOCK
      break;
    }
    case 0xE9:{ //JP (HL)
      PC = HL;
   	  break;
	  }
    case 0xEA:{ //LD (a16),A
      instruction[2] = memRead(PC); PC++;
      instruction[1] = memRead(PC); PC++;
      uint16_t addr = to16bits(instruction[1], instruction[2]);
      memWrite(addr, A);
   	  break;
	  }
    case 0xEE:{ //XOR d8
      instruction[1] = memRead(PC); PC++;
      op_XOR(instruction[1]);
   	  break;
	  }
    case 0xEF:{ //RST 28H
      op_RST(0x0028);
   	  break;
	  }
    case 0xF0:{ //LDH A,(a8) //LD A,($FF00 + a8)
      instruction[1] = memRead(PC); PC++;
      uint16_t IOaddr = 0xFF00 + static_cast<uint16_t>(instruction[1]);
      A = memRead(IOaddr);
   	  break;
	  }
    case 0xF1:{ //POP AF
      op_POP(A, F);
      F &= 0xF0; //ignore last 4 bits for F!
  	  break;
  	}
    case 0xF2:{ //LD A,(C) //LD A,($FF00 + C)
      uint16_t IOaddr = 0xFF00 + static_cast<uint16_t>(C);
      A = memRead(IOaddr);
   	  break;
	  }
    case 0xF3:{ //DI
	    IME = false;
   	  break;
	  }
    case 0xF5:{ //PUSH AF
      op_PUSH(A, F);
  	  break;
  	}
    case 0xF6:{ //OR d8
      instruction[1] = memRead(PC); PC++;
      op_OR(instruction[1]);
   	  break;
	  }
    case 0xF7:{ //RST 30H
      op_RST(0x0030);
   	  break;
	  }
    case 0xF8:{ //ADD HL,SP+r8
      ///00hc
      instruction[1] = memRead(PC); PC++;
      F &= 0x00; //CLEAR F
      
      uint16_t csum = (SP & 0xFF) + instruction[1];
      uint16_t hcSum = (SP & 0x0F) + (instruction[1] & 0x0F);
      F |= ((csum & 0xFF00) != 0x0) << 4; // SET F.C if Carry 15
      F |= ((hcSum & 0xF0) != 0x0) << 5; // SET F.HC if HC
      HL = SP + 0xFF00 * (instruction[1] >> 7) + instruction[1];
      CLOCK
      break;
    }
    case 0xF9:{ //LD SP,HL
      SP = HL;
   	  break;
	  }
    case 0xFA:{ //LD A,(a16)
      instruction[2] = memRead(PC); PC++;
      instruction[1] = memRead(PC); PC++;
      uint16_t addr = to16bits(instruction[1], instruction[2]);
      uint8_t data = memRead(addr);
      A = data;
   	  break;
	  }
    case 0xFB:{ //EI
	    IME = true;
   	  break;
	  }
    case 0xFE:{ //CP d8
      instruction[1] = memRead(PC); PC++;
      op_CP(instruction[1]);
   	  break;
	  }
    case 0xFF:{ //RST 38H
      op_RST(0x0038);
   	  break;
	  }
	  default:{
	    std::cout << "Not implemented opcode: 0x" << std::to_string(instruction[0]) << std::endl;
	    exit(1); // not the best way...
	    break;
	  }
  }
}

void CPU::op_PREFIXCB(){
  instruction[1] = memRead(PC); PC++;
  switch(instruction[1]){
    case 0x00:{ //RLC B
      op_CB_RLC(B);
 	    break;
  	}
    case 0x01:{ //RLC C
      op_CB_RLC(C);
 	    break;
  	}
    case 0x02:{ //RLC D
      op_CB_RLC(D);
 	    break;
  	}
    case 0x03:{ //RLC E
      op_CB_RLC(E);
 	    break;
  	}
    case 0x04:{ //RLC H
      op_CB_RLC(H);
 	    break;
  	}
    case 0x05:{ //RLC L
      op_CB_RLC(L);
 	    break;
  	}
    case 0x06:{ //RLC (HL)
      uint8_t val = memRead(HL);
      op_CB_RLC(val);
      memWrite(HL, val);
 	    break;
  	}
    case 0x07:{ //RLC A
      op_CB_RLC(A);
 	    break;
  	}
    case 0x08:{ //RRC B
      op_CB_RRC(B);
 	    break;
  	}
    case 0x09:{ //RRC C
      op_CB_RRC(C);
 	    break;
  	}
    case 0x0A:{ //RRC D
      op_CB_RRC(D);
 	    break;
  	}
    case 0x0B:{ //RRC E
      op_CB_RRC(E);
 	    break;
  	}
    case 0x0C:{ //RRC H
      op_CB_RRC(H);
 	    break;
  	}
    case 0x0D:{ //RRC L
      op_CB_RRC(L);
 	    break;
  	}
    case 0x0E:{ //RRC (HL)
      uint8_t val = memRead(HL);
      op_CB_RRC(val);
      memWrite(HL, val);
 	    break;
  	}
    case 0x0F:{ //RRC A
      op_CB_RRC(A);
 	    break;
  	}
    case 0x10:{ //RL B
      op_CB_RL(B);
 	    break;
  	}
    case 0x11:{ //RL C
      op_CB_RL(C);
 	    break;
  	}
    case 0x12:{ //RL D
      op_CB_RL(D);
 	    break;
  	}
    case 0x13:{ //RL E
      op_CB_RL(E);
 	    break;
  	}
    case 0x14:{ //RL H
      op_CB_RL(H);
 	    break;
  	}
    case 0x15:{ //RL L
      op_CB_RL(L);
 	    break;
  	}
    case 0x16:{ //RL (HL)
      uint8_t val = memRead(HL);
      op_CB_RL(val);
      memWrite(HL, val);
 	    break;
  	}
    case 0x17:{ //RL A
      op_CB_RL(A);
 	    break;
  	}
    case 0x18:{ //RR B
      op_CB_RR(B);
 	    break;
  	}
    case 0x19:{ //RR C
      op_CB_RR(C);
 	    break;
  	}
    case 0x1A:{ //RR D
      op_CB_RR(D);
 	    break;
  	}
    case 0x1B:{ //RR E
      op_CB_RR(E);
 	    break;
  	}
    case 0x1C:{ //RR H
      op_CB_RR(H);
 	    break;
  	}
    case 0x1D:{ //RR L
      op_CB_RR(L);
 	    break;
  	}
    case 0x1E:{ //RR (HL)
      uint8_t val = memRead(HL);
      op_CB_RR(val);
      memWrite(HL, val);
 	    break;
  	}
    case 0x1F:{ //RR A
      op_CB_RR(A);
 	    break;
  	}
    case 0x20:{ //SLA B
      op_CB_SLA(B);
 	    break;
  	}
    case 0x21:{ //SLA C
      op_CB_SLA(C);
 	    break;
  	}
    case 0x22:{ //SLA D
      op_CB_SLA(D);
 	    break;
  	}
    case 0x23:{ //SLA E
      op_CB_SLA(E);
 	    break;
  	}
    case 0x24:{ //SLA H
      op_CB_SLA(H);
 	    break;
  	}
    case 0x25:{ //SLA L
      op_CB_SLA(L);
 	    break;
  	}
    case 0x26:{ //SLA (HL)
      uint8_t val = memRead(HL);
      op_CB_SLA(val);
      memWrite(HL, val);
 	    break;
  	}
    case 0x27:{ //SLA A
      op_CB_SLA(A);
 	    break;
  	}
    case 0x28:{ //SRA B
      op_CB_SRA(B);
 	    break;
  	}
    case 0x29:{ //SRA C
      op_CB_SRA(C);
 	    break;
  	}
    case 0x2A:{ //SRA D
      op_CB_SRA(D);
 	    break;
  	}
    case 0x2B:{ //SRA E
      op_CB_SRA(E);
 	    break;
  	}
    case 0x2C:{ //SRA H
      op_CB_SRA(H);
 	    break;
  	}
    case 0x2D:{ //SRA L
      op_CB_SRA(L);
 	    break;
  	}
    case 0x2E:{ //SRA (HL)
      uint8_t val = memRead(HL);
      op_CB_SRA(val);
      memWrite(HL, val);
 	    break;
  	}
    case 0x2F:{ //SRA A
      op_CB_SRA(A);
 	    break;
  	}
    case 0x30:{ //SWAP B
      op_CB_SWAP(B);
 	    break;
  	}
    case 0x31:{ //SWAP C
      op_CB_SWAP(C);
 	    break;
  	}
    case 0x32:{ //SWAP D
      op_CB_SWAP(D);
 	    break;
  	}
    case 0x33:{ //SWAP E
      op_CB_SWAP(E);
 	    break;
  	}
    case 0x34:{ //SWAP H
      op_CB_SWAP(H);
 	    break;
  	}
    case 0x35:{ //SWAP L
      op_CB_SWAP(L);
 	    break;
  	}
    case 0x36:{ //SWAP (HL)
      uint8_t val = memRead(HL);
      op_CB_SWAP(val);
      memWrite(HL, val);
 	    break;
  	}
    case 0x37:{ //SWAP A
      op_CB_SWAP(A);
 	    break;
  	}
    case 0x38:{ //SRL B
      op_CB_SRL(B);
 	    break;
  	}
    case 0x39:{ //SRL C
      op_CB_SRL(C);
 	    break;
  	}
    case 0x3A:{ //SRL D
      op_CB_SRL(D);
 	    break;
  	}
    case 0x3B:{ //SRL E
      op_CB_SRL(E);
 	    break;
  	}
    case 0x3C:{ //SRL H
      op_CB_SRL(H);
 	    break;
  	}
    case 0x3D:{ //SRL L
      op_CB_SRL(L);
 	    break;
  	}
    case 0x3E:{ //SRL (HL)
      uint8_t val = memRead(HL);
      op_CB_SRL(val);
      memWrite(HL, val);
 	    break;
  	}
    case 0x3F:{ //SRL A
      op_CB_SRL(A);
 	    break;
  	}
    case 0x40:{ //BIT 0,B
      op_CB_BIT(0,B);
 	    break;
  	}
    case 0x41:{ //BIT 0,C
      op_CB_BIT(0,C);
 	    break;
  	}
    case 0x42:{ //BIT 0,D
      op_CB_BIT(0,D);
 	    break;
  	}
    case 0x43:{ //BIT 0,E
      op_CB_BIT(0,E);
 	    break;
  	}
    case 0x44:{ //BIT 0,H
      op_CB_BIT(0,H);
 	    break;
  	}
    case 0x45:{ //BIT 0,L
      op_CB_BIT(0,L);
 	    break;
  	}
    case 0x46:{ //BIT 0,(HL)
      uint8_t val = memRead(HL);
      op_CB_BIT(0,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0x47:{ //BIT 0,A
      op_CB_BIT(0,A);
 	    break;
  	}
    case 0x48:{ //BIT 1,B
      op_CB_BIT(1,B);
 	    break;
  	}
    case 0x49:{ //BIT 1,C
      op_CB_BIT(1,C);
 	    break;
  	}
    case 0x4A:{ //BIT 1,D
      op_CB_BIT(1,D);
 	    break;
  	}
    case 0x4B:{ //BIT 1,E
      op_CB_BIT(1,E);
 	    break;
  	}
    case 0x4C:{ //BIT 1,H
      op_CB_BIT(1,H);
 	    break;
  	}
    case 0x4D:{ //BIT 1,L
      op_CB_BIT(1,L);
 	    break;
  	}
    case 0x4E:{ //BIT 1,(HL)
      uint8_t val = memRead(HL);
      op_CB_BIT(1,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0x4F:{ //BIT 1,A
      op_CB_BIT(1,A);
 	    break;
  	}
    case 0x50:{ //BIT 2,B
      op_CB_BIT(2,B);
 	    break;
  	}
    case 0x51:{ //BIT 2,C
      op_CB_BIT(2,C);
 	    break;
  	}
    case 0x52:{ //BIT 2,D
      op_CB_BIT(2,D);
 	    break;
  	}
    case 0x53:{ //BIT 2,E
      op_CB_BIT(2,E);
 	    break;
  	}
    case 0x54:{ //BIT 2,H
      op_CB_BIT(2,H);
 	    break;
  	}
    case 0x55:{ //BIT 2,L
      op_CB_BIT(2,L);
 	    break;
  	}
    case 0x56:{ //BIT 2,(HL)
      uint8_t val = memRead(HL);
      op_CB_BIT(2,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0x57:{ //BIT 2,A
      op_CB_BIT(2,A);
 	    break;
  	}
    case 0x58:{ //BIT 3,B
      op_CB_BIT(3,B);
 	    break;
  	}
    case 0x59:{ //BIT 3,C
      op_CB_BIT(3,C);
 	    break;
  	}
    case 0x5A:{ //BIT 3,D
      op_CB_BIT(3,D);
 	    break;
  	}
    case 0x5B:{ //BIT 3,E
      op_CB_BIT(3,E);
 	    break;
  	}
    case 0x5C:{ //BIT 3,H
      op_CB_BIT(3,H);
 	    break;
  	}
    case 0x5D:{ //BIT 3,L
      op_CB_BIT(3,L);
 	    break;
  	}
    case 0x5E:{ //BIT 3,(HL)
      uint8_t val = memRead(HL);
      op_CB_BIT(3,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0x5F:{ //BIT 3,A
      op_CB_BIT(3,A);
 	    break;
  	}
    case 0x60:{ //BIT 4,B
      op_CB_BIT(4,B);
 	    break;
  	}
    case 0x61:{ //BIT 4,C
      op_CB_BIT(4,C);
 	    break;
  	}
    case 0x62:{ //BIT 4,D
      op_CB_BIT(4,D);
 	    break;
  	}
    case 0x63:{ //BIT 4,E
      op_CB_BIT(4,E);
 	    break;
  	}
    case 0x64:{ //BIT 4,H
      op_CB_BIT(4,H);
 	    break;
  	}
    case 0x65:{ //BIT 4,L
      op_CB_BIT(4,L);
 	    break;
  	}
    case 0x66:{ //BIT 4,(HL)
      uint8_t val = memRead(HL);
      op_CB_BIT(4,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0x67:{ //BIT 4,A
      op_CB_BIT(4,A);
 	    break;
  	}
    case 0x68:{ //BIT 5,B
      op_CB_BIT(5,B);
 	    break;
  	}
    case 0x69:{ //BIT 5,C
      op_CB_BIT(5,C);
 	    break;
  	}
    case 0x6A:{ //BIT 5,D
      op_CB_BIT(5,D);
 	    break;
  	}
    case 0x6B:{ //BIT 5,E
      op_CB_BIT(5,E);
 	    break;
  	}
    case 0x6C:{ //BIT 5,H
      op_CB_BIT(5,H);
 	    break;
  	}
    case 0x6D:{ //BIT 5,L
      op_CB_BIT(5,L);
 	    break;
  	}
    case 0x6E:{ //BIT 5,(HL)
      uint8_t val = memRead(HL);
      op_CB_BIT(5,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0x6F:{ //BIT 5,A
      op_CB_BIT(5,A);
 	    break;
  	}
    case 0x70:{ //BIT 6,B
      op_CB_BIT(6,B);
 	    break;
  	}
    case 0x71:{ //BIT 6,C
      op_CB_BIT(6,C);
 	    break;
  	}
    case 0x72:{ //BIT 6,D
      op_CB_BIT(6,D);
 	    break;
  	}
    case 0x73:{ //BIT 6,E
      op_CB_BIT(6,E);
 	    break;
  	}
    case 0x74:{ //BIT 6,H
      op_CB_BIT(6,H);
 	    break;
  	}
    case 0x75:{ //BIT 6,L
      op_CB_BIT(6,L);
 	    break;
  	}
    case 0x76:{ //BIT 6,(HL)
      uint8_t val = memRead(HL);
      op_CB_BIT(6,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0x77:{ //BIT 6,A
      op_CB_BIT(6,A);
 	    break;
  	}
    case 0x78:{ //BIT 7,B
      op_CB_BIT(7,B);
 	    break;
  	}
    case 0x79:{ //BIT 7,C
      op_CB_BIT(7,C);
 	    break;
  	}
    case 0x7A:{ //BIT 7,D
      op_CB_BIT(7,D);
 	    break;
  	}
    case 0x7B:{ //BIT 7,E
      op_CB_BIT(7,E);
 	    break;
  	}
    case 0x7C:{ //BIT 7,H
      op_CB_BIT(7,H);
 	    break;
  	}
    case 0x7D:{ //BIT 7,L
      op_CB_BIT(7,L);
 	    break;
  	}
    case 0x7E:{ //BIT 7,(HL)
      uint8_t val = memRead(HL);
      op_CB_BIT(7,val);
 	    break;
  	}
    case 0x7F:{ //BIT 7,A
      op_CB_BIT(7,A);
 	    break;
  	}
    case 0x80:{ //RES 0,B
      op_CB_RES(0,B);
 	    break;
  	}
    case 0x81:{ //RES 0,C
      op_CB_RES(0,C);
 	    break;
  	}
    case 0x82:{ //RES 0,D
      op_CB_RES(0,D);
 	    break;
  	}
    case 0x83:{ //RES 0,E
      op_CB_RES(0,E);
 	    break;
  	}
    case 0x84:{ //RES 0,H
      op_CB_RES(0,H);
 	    break;
  	}
    case 0x85:{ //RES 0,L
      op_CB_RES(0,L);
 	    break;
  	}
    case 0x86:{ //RES 0,(HL)
      uint8_t val = memRead(HL);
      op_CB_RES(0,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0x87:{ //RES 0,A
      op_CB_RES(0,A);
 	    break;
  	}
    case 0x88:{ //RES 1,B
      op_CB_RES(1,B);
 	    break;
  	}
    case 0x89:{ //RES 1,C
      op_CB_RES(1,C);
 	    break;
  	}
    case 0x8A:{ //RES 1,D
      op_CB_RES(1,D);
 	    break;
  	}
    case 0x8B:{ //RES 1,E
      op_CB_RES(1,E);
 	    break;
  	}
    case 0x8C:{ //RES 1,H
      op_CB_RES(1,H);
 	    break;
  	}
    case 0x8D:{ //RES 1,L
      op_CB_RES(1,L);
 	    break;
  	}
    case 0x8E:{ //RES 1,(HL)
      uint8_t val = memRead(HL);
      op_CB_RES(1,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0x8F:{ //RES 1,A
      op_CB_RES(1,A);
 	    break;
  	}
    case 0x90:{ //RES 2,B
      op_CB_RES(2,B);
 	    break;
  	}
    case 0x91:{ //RES 2,C
      op_CB_RES(2,C);
 	    break;
  	}
    case 0x92:{ //RES 2,D
      op_CB_RES(2,D);
 	    break;
  	}
    case 0x93:{ //RES 2,E
      op_CB_RES(2,E);
 	    break;
  	}
    case 0x94:{ //RES 2,H
      op_CB_RES(2,H);
 	    break;
  	}
    case 0x95:{ //RES 2,L
      op_CB_RES(2,L);
 	    break;
  	}
    case 0x96:{ //RES 2,(HL)
      uint8_t val = memRead(HL);
      op_CB_RES(2,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0x97:{ //RES 2,A
      op_CB_RES(2,A);
 	    break;
  	}
    case 0x98:{ //RES 3,B
      op_CB_RES(3,B);
 	    break;
  	}
    case 0x99:{ //RES 3,C
      op_CB_RES(3,C);
 	    break;
  	}
    case 0x9A:{ //RES 3,D
      op_CB_RES(3,D);
 	    break;
  	}
    case 0x9B:{ //RES 3,E
      op_CB_RES(3,E);
 	    break;
  	}
    case 0x9C:{ //RES 3,H
      op_CB_RES(3,H);
 	    break;
  	}
    case 0x9D:{ //RES 3,L
      op_CB_RES(3,L);
 	    break;
  	}
    case 0x9E:{ //RES 3,(HL)
      uint8_t val = memRead(HL);
      op_CB_RES(3,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0x9F:{ //RES 3,A
      op_CB_RES(3,A);
 	    break;
  	}
    case 0xA0:{ //RES 4,B
      op_CB_RES(4,B);
 	    break;
  	}
    case 0xA1:{ //RES 4,C
      op_CB_RES(4,C);
 	    break;
  	}
    case 0xA2:{ //RES 4,D
      op_CB_RES(4,D);
 	    break;
  	}
    case 0xA3:{ //RES 4,E
      op_CB_RES(4,E);
 	    break;
  	}
    case 0xA4:{ //RES 4,H
      op_CB_RES(4,H);
 	    break;
  	}
    case 0xA5:{ //RES 4,L
      op_CB_RES(4,L);
 	    break;
  	}
    case 0xA6:{ //RES 4,(HL)
      uint8_t val = memRead(HL);
      op_CB_RES(4,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0xA7:{ //RES 4,A
      op_CB_RES(4,A);
 	    break;
  	}
    case 0xA8:{ //RES 5,B
      op_CB_RES(5,B);
 	    break;
  	}
    case 0xA9:{ //RES 5,C
      op_CB_RES(5,C);
 	    break;
  	}
    case 0xAA:{ //RES 5,D
      op_CB_RES(5,D);
 	    break;
  	}
    case 0xAB:{ //RES 5,E
      op_CB_RES(5,E);
 	    break;
  	}
    case 0xAC:{ //RES 5,H
      op_CB_RES(5,H);
 	    break;
  	}
    case 0xAD:{ //RES 5,L
      op_CB_RES(5,L);
 	    break;
  	}
    case 0xAE:{ //RES 5,(HL)
      uint8_t val = memRead(HL);
      op_CB_RES(5,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0xAF:{ //RES 5,A
      op_CB_RES(5,A);
 	    break;
  	}
    case 0xB0:{ //RES 6,B
      op_CB_RES(6,B);
 	    break;
  	}
    case 0xB1:{ //RES 6,C
      op_CB_RES(6,C);
 	    break;
  	}
    case 0xB2:{ //RES 6,D
      op_CB_RES(6,D);
 	    break;
  	}
    case 0xB3:{ //RES 6,E
      op_CB_RES(6,E);
 	    break;
  	}
    case 0xB4:{ //RES 6,H
      op_CB_RES(6,H);
 	    break;
  	}
    case 0xB5:{ //RES 6,L
      op_CB_RES(6,L);
 	    break;
  	}
    case 0xB6:{ //RES 6,(HL)
      uint8_t val = memRead(HL);
      op_CB_RES(6,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0xB7:{ //RES 6,A
      op_CB_RES(6,A);
 	    break;
  	}
    case 0xB8:{ //RES 7,B
      op_CB_RES(7,B);
 	    break;
  	}
    case 0xB9:{ //RES 7,C
      op_CB_RES(7,C);
 	    break;
  	}
    case 0xBA:{ //RES 7,D
      op_CB_RES(7,D);
 	    break;
  	}
    case 0xBB:{ //RES 7,E
      op_CB_RES(7,E);
 	    break;
  	}
    case 0xBC:{ //RES 7,H
      op_CB_RES(7,H);
 	    break;
  	}
    case 0xBD:{ //RES 7,L
      op_CB_RES(7,L);
 	    break;
  	}
    case 0xBE:{ //RES 7,(HL)
      uint8_t val = memRead(HL);
      op_CB_RES(7,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0xBF:{ //RES 7,A
      op_CB_RES(7,A);
 	    break;
  	}
    case 0xC0:{ //SET 0,B
      op_CB_SET(0,B);
 	    break;
  	}
    case 0xC1:{ //SET 0,C
      op_CB_SET(0,C);
 	    break;
  	}
    case 0xC2:{ //SET 0,D
      op_CB_SET(0,D);
 	    break;
  	}
    case 0xC3:{ //SET 0,E
      op_CB_SET(0,E);
 	    break;
  	}
    case 0xC4:{ //SET 0,H
      op_CB_SET(0,H);
 	    break;
  	}
    case 0xC5:{ //SET 0,L
      op_CB_SET(0,L);
 	    break;
  	}
    case 0xC6:{ //SET 0,(HL)
      uint8_t val = memRead(HL);
      op_CB_SET(0,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0xC7:{ //SET 0,A
      op_CB_SET(0,A);
 	    break;
  	}
    case 0xC8:{ //SET 1,B
      op_CB_SET(1,B);
 	    break;
  	}
    case 0xC9:{ //SET 1,C
      op_CB_SET(1,C);
 	    break;
  	}
    case 0xCA:{ //SET 1,D
      op_CB_SET(1,D);
 	    break;
  	}
    case 0xCB:{ //SET 1,E
      op_CB_SET(1,E);
 	    break;
  	}
    case 0xCC:{ //SET 1,H
      op_CB_SET(1,H);
 	    break;
  	}
    case 0xCD:{ //SET 1,L
      op_CB_SET(1,L);
 	    break;
  	}
    case 0xCE:{ //SET 1,(HL)
      uint8_t val = memRead(HL);
      op_CB_SET(1,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0xCF:{ //SET 1,A
      op_CB_SET(1,A);
 	    break;
  	}
    case 0xD0:{ //SET 2,B
      op_CB_SET(2,B);
 	    break;
  	}
    case 0xD1:{ //SET 2,C
      op_CB_SET(2,C);
 	    break;
  	}
    case 0xD2:{ //SET 2,D
      op_CB_SET(2,D);
 	    break;
  	}
    case 0xD3:{ //SET 2,E
      op_CB_SET(2,E);
 	    break;
  	}
    case 0xD4:{ //SET 2,H
      op_CB_SET(2,H);
 	    break;
  	}
    case 0xD5:{ //SET 2,L
      op_CB_SET(2,L);
 	    break;
  	}
    case 0xD6:{ //SET 2,(HL)
      uint8_t val = memRead(HL);
      op_CB_SET(2,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0xD7:{ //SET 2,A
      op_CB_SET(2,A);
 	    break;
  	}
    case 0xD8:{ //SET 3,B
      op_CB_SET(3,B);
 	    break;
  	}
    case 0xD9:{ //SET 3,C
      op_CB_SET(3,C);
 	    break;
  	}
    case 0xDA:{ //SET 3,D
      op_CB_SET(3,D);
 	    break;
  	}
    case 0xDB:{ //SET 3,E
      op_CB_SET(3,E);
 	    break;
  	}
    case 0xDC:{ //SET 3,H
      op_CB_SET(3,H);
 	    break;
  	}
    case 0xDD:{ //SET 3,L
      op_CB_SET(3,L);
 	    break;
  	}
    case 0xDE:{ //SET 3,(HL)
      uint8_t val = memRead(HL);
      op_CB_SET(3,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0xDF:{ //SET 3,A
      op_CB_SET(3,A);
 	    break;
  	}
    case 0xE0:{ //SET 4,B
      op_CB_SET(4,B);
 	    break;
  	}
    case 0xE1:{ //SET 4,C
      op_CB_SET(4,C);
 	    break;
  	}
    case 0xE2:{ //SET 4,D
      op_CB_SET(4,D);
 	    break;
  	}
    case 0xE3:{ //SET 4,E
      op_CB_SET(4,E);
 	    break;
  	}
    case 0xE4:{ //SET 4,H
      op_CB_SET(4,H);
 	    break;
  	}
    case 0xE5:{ //SET 4,L
      op_CB_SET(4,L);
 	    break;
  	}
    case 0xE6:{ //SET 4,(HL)
      uint8_t val = memRead(HL);
      op_CB_SET(4,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0xE7:{ //SET 4,A
      op_CB_SET(4,A);
 	    break;
  	}
    case 0xE8:{ //SET 5,B
      op_CB_SET(5,B);
 	    break;
  	}
    case 0xE9:{ //SET 5,C
      op_CB_SET(5,C);
 	    break;
  	}
    case 0xEA:{ //SET 5,D
      op_CB_SET(5,D);
 	    break;
  	}
    case 0xEB:{ //SET 5,E
      op_CB_SET(5,E);
 	    break;
  	}
    case 0xEC:{ //SET 5,H
      op_CB_SET(5,H);
 	    break;
  	}
    case 0xED:{ //SET 5,L
      op_CB_SET(5,L);
 	    break;
  	}
    case 0xEE:{ //SET 5,(HL)
      uint8_t val = memRead(HL);
      op_CB_SET(5,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0xEF:{ //SET 5,A
      op_CB_SET(5,A);
 	    break;
  	}
    case 0xF0:{ //SET 6,B
      op_CB_SET(6,B);
 	    break;
  	}
    case 0xF1:{ //SET 6,C
      op_CB_SET(6,C);
 	    break;
  	}
    case 0xF2:{ //SET 6,D
      op_CB_SET(6,D);
 	    break;
  	}
    case 0xF3:{ //SET 6,E
      op_CB_SET(6,E);
 	    break;
  	}
    case 0xF4:{ //SET 6,H
      op_CB_SET(6,H);
 	    break;
  	}
    case 0xF5:{ //SET 6,L
      op_CB_SET(6,L);
 	    break;
  	}
    case 0xF6:{ //SET 6,(HL)
      uint8_t val = memRead(HL);
      op_CB_SET(6,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0xF7:{ //SET 6,A
      op_CB_SET(6,A);
 	    break;
  	}
    case 0xF8:{ //SET 7,B
      op_CB_SET(7,B);
 	    break;
  	}
    case 0xF9:{ //SET 7,C
      op_CB_SET(7,C);
 	    break;
  	}
    case 0xFA:{ //SET 7,D
      op_CB_SET(7,D);
 	    break;
  	}
    case 0xFB:{ //SET 7,E
      op_CB_SET(7,E);
 	    break;
  	}
    case 0xFC:{ //SET 7,H
      op_CB_SET(7,H);
 	    break;
  	}
    case 0xFD:{ //SET 7,L
      op_CB_SET(7,L);
 	    break;
  	}
    case 0xFE:{ //SET 7,(HL)
      uint8_t val = memRead(HL);
      op_CB_SET(7,val);
      memWrite(HL, val);
 	    break;
  	}
    case 0xFF:{ //SET 7,A
      op_CB_SET(7,A);
 	    break;
  	}
    default:{
	    std::cout << "CB ERROR: instruction - " << std::to_string(instruction[1]) << std::endl;
	    exit(1); // not the best way...
	    break;
    }
  }
}

bool CPU::handleInterrupt(){ //TODO improve
  if(HALTed && !IME){
    uint8_t IF = _mmu.read_addr(0xFF0F);
    uint8_t IE = _mmu.read_addr(0xFFFF);
    if((IF & IE) == 0x00){
      return false;
    }
    HALTed = false;
    cycles += 4;
    return false;
  }
  if(IME){
    uint8_t IF = _mmu.read_addr(0xFF0F);
    uint8_t IE = _mmu.read_addr(0xFFFF);
    if(((IF & IE) & 0x01 ) == 0x01){ //INTERRUPT PPRIORITY 0
        if(HALTed){
            HALTed = false;
            cycles += 4;
        }
      //std::cout << "VBLANK IRQ" << std::endl;
      IME = 0;
      op_RST(0x0040);
      _mmu.write_addr(0xFF0F, (IF & 0xFE)); // clear IF bit 0
      return true;
    }
    if(((IF & IE) & 0x02 ) == 0x02){ //INTERRUPT PPRIORITY 1
        if(HALTed){
            HALTed = false;
            cycles += 4;
        }
      //std::cout << "LCD STAT IRQ" << std::endl;
      IME = 0;
      op_RST(0x0048);
      _mmu.write_addr(0xFF0F, (IF & 0xFD)); // clear IF bit 1
      return true;
    }
    if(((IF & IE) & 0x04 ) == 0x04){ //INTERRUPT PPRIORITY 2
        if(HALTed){
            HALTed = false;
            cycles += 4;
        }
      //std::cout << "TIMER IRQ" << std::endl;
      IME = 0;
      op_RST(0x0050);
      _mmu.write_addr(0xFF0F, (IF & 0xFB)); // clear IF bit 2
      return true;
    }
    if(((IF & IE) & 0x08 ) == 0x08){ //INTERRUPT PPRIORITY 3
        if(HALTed){
            HALTed = false;
            cycles += 4;
        }
      std::cout << "SERIAL IRQ" << std::endl;
      IME = 0;
      op_RST(0x0058);
      _mmu.write_addr(0xFF0F, (IF & 0xF7)); // clear IF bit 3
      return true;
    }
    if(((IF & IE) & 0x10 ) == 0x10){ //INTERRUPT PPRIORITY 4
        if(HALTed){
            HALTed = false;
            cycles += 4;
        }
      std::cout << "JOYPAD IRQ" << std::endl;
      IME = 0;
      op_RST(0x0060);
      _mmu.write_addr(0xFF0F, (IF & 0xEF)); // clear IF bit 5
      return true;
    }
  }
  return false;
}
