#ifndef CPUOPCODES_H
#define CPUOPCODES_H

#include <stdint.h>
#include <array>

constexpr std::array<const char*, 0x100> OPCODE = {
  "NOP", "LD BC, ${1}{2}", "LD (BC), A", "INC BC",   "INC B", "DEC B", "LD B, ${1}", "RLCA",     "LD ${1}, SP", "ADD HL, BC", "LD A, (BC)", "DEC BC",     "INC C", "DEC C", "LD C, ${1}", "RRCA",
  "STOP", "LD DE, ${1}{2}", "LD (DE), A", "INC DE",   "INC D", "DEC D", "LD D, ${1}", "RLA",     "JR ${1}", "ADD HL, DE", "LD A, (DE)", "INC DE",     "INC E", "DEC E", "LD E, ${1}", "RRA",
  "JR NZ, ${1}", "LD HL, ${1}{2}", "LD (HL+), A", "INC HL",   "INC H", "DEC H", "LD H, ${1}", "DAA",     "JR Z, ${1}", "ADD HL ,HL", "LD A, (HL+)", "DEC HL",     "INC L", "DEC L", "LD L, ${1}", "CPL",
  "JR NC, ${1}", "LD SP, ${1}{2}", "LD (HL-), A", "INC SP",   "INC (HL)", "DEC (HL)", "LD (HL), ${1}", "SCF",     "JR C, ${1}", "ADD HL, SP", "LD A, (HL-)", "DEC SP",     "INC A", "DEC A", "LD A, ${1}", "CCF",
  
  "LD B, B", "LD B, C", "LD B, D", "LD B, E",   "LD B, H", "LD B, L", "LD B, (HL)", "LD B, A",     "LD C, B", "LD C, C", "LD C, D", "LD C, E",     "LD C, H", "LD C, L", "LD C, (HL)", "LD C, A",
  "LD D, B", "LD D, C", "LD D, D", "LD D, E",   "LD D, H", "LD D, L", "LD D, (HL)", "LD D, A",     "LD E, B", "LD E, C", "LD E, D", "LD E, E",     "LD E, H", "LD E, L", "LD E, (HL)", "LD E, A",
  "LD H, B", "LD H, C", "LD H, D", "LD H, E",   "LD H, H", "LD H, L", "LD H, (HL)", "LD H, A",     "LD L, B", "LD L, C", "LD L, D", "LD L, E",     "LD L, H", "LD L, L", "LD L, (HL)", "LD L, A",
  "LD (HL), B", "LD (HL), C", "LD (HL), D", "LD (HL), E",   "LD (HL), H", "LD (HL), L", "HALT", "LD (HL), A",     "LD A, B", "LD A, C", "LD A, D", "LD A, E",     "LD A, H", "LD A, L", "LD A, (HL)", "LD A, A",
  
  "ADD B", "ADD C", "ADD D", "ADD E",   "ADD H", "ADD L", "ADD (HL)", "ADD A",     "ADC A, B", "ADC A, C", "ADC A, D", "ADC A, E",   "ADC A, H", "ADC A, L", "ADC A, (HL)", "ADC A, A",
  "SUB B", "SUB C", "SUB D", "SUB E",   "SUB H", "SUB L", "SUB (HL)", "SUB A",     "SBC A, B", "SBC A, C", "SBC A, D", "SBC A, E",   "SBC A, H", "SBC A, L", "SBC A, (HL)", "SBC A, A",
  "AND B", "AND C", "AND D", "AND E",   "AND H", "AND L", "AND (HL)", "AND A",     "XOR B", "XOR C", "XOR D", "XOR E",   "XOR H", "XOR L", "XOR (HL)", "XOR A",
  "OR B", "OR C", "OR D", "OR E",   "OR H", "OR L", "OR (HL)", "OR A",     "CP B", "CP C", "CP D", "CP E",   "CP H", "CP L", "CP (HL)", "CP A",
  
  "RET NZ", "POP BC", "JP NZ, ${1}{2}", "JP ${1}{2}",   "CALL NZ, ${1}{2}", "PUSH BC", "ADD A, ${1}", "RST 00H",     "RET Z", "RET", "JP Z, ${1}{2}", "CB-{1}",     "CALL Z, ${1}{2}", "CALL ${1}{2}", "ADC A, ${1}", "RST 08H",
  "RET NC", "POP DE", "JP NC, ${1}{2}", "-",   "CALL NC, ${1}{2}", "PUSH DE", "SUB ${1}", "RST 10H",     "RET C", "RETI", "JP C, ${1}{2}", "-",     "CALL C, ${1}{2}", "-", "SBC A, ${1}", "RST 18H",
  "LD ($FF00 + ${1}), A", "POP HL", "LD ($FF00 + C), A", "-",   "-", "PUSH HL", "AND ${1}", "RST 20H",     "ADD SP, ${1}", "JP (HL)", "LD (${1}{2}),A", "-",     "-", "-", "XOR ${1}", "RST 28H",
  "LD A, ($FF00 + ${1})", "POP AF", "LD A, ($FF00 + C)", "DI",   "-", "PUSH AF", "OR ${1}", "RST 30H",     "LD HL, SP+${1}", "LD SP, HL", "LD A, (${1}{2})", "EI",     "-", "-", "CP ${1}", "RST 38H"
};

constexpr std::array<const uint8_t, 0x100> OPCODE_SIZE = {
  1, 3, 1, 1,     1, 1, 2, 1,     2, 1, 1, 1,     1, 1, 2, 1,
  1, 3, 1, 1,     1, 1, 2, 1,     2, 1, 1, 1,     1, 1, 2, 1,
  2, 3, 1, 1,     1, 1, 2, 1,     2, 1, 1, 1,     1, 1, 2, 1,
  2, 3, 1, 1,     1, 1, 2, 1,     2, 1, 1, 1,     1, 1, 2, 1,

  1, 1, 1, 1,     1, 1, 1, 1,     1, 1, 1, 1,     1, 1, 1, 1,
  1, 1, 1, 1,     1, 1, 1, 1,     1, 1, 1, 1,     1, 1, 1, 1,
  1, 1, 1, 1,     1, 1, 1, 1,     1, 1, 1, 1,     1, 1, 1, 1,
  1, 1, 1, 1,     1, 1, 1, 1,     1, 1, 1, 1,     1, 1, 1, 1,

  1, 1, 1, 1,     1, 1, 1, 1,     1, 1, 1, 1,     1, 1, 1, 1,
  1, 1, 1, 1,     1, 1, 1, 1,     1, 1, 1, 1,     1, 1, 1, 1,
  1, 1, 1, 1,     1, 1, 1, 1,     1, 1, 1, 1,     1, 1, 1, 1,
  1, 1, 1, 1,     1, 1, 1, 1,     1, 1, 1, 1,     1, 1, 1, 1,

  1, 1, 3, 3,     3, 1, 2, 1,     1, 1, 3, 2,     3, 3, 2, 1,
  1, 1, 3, 8,     3, 1, 2, 1,     1, 1, 3, 8,     3, 8, 2, 1,
  2, 1, 1, 8,     8, 1, 2, 1,     2, 1, 3, 8,     8, 8, 2, 1,
  2, 1, 1, 1,     8, 1, 2, 1,     2, 1, 3, 1,     8, 8, 2, 1 
};

#endif /* CPUOPCODES_H */
