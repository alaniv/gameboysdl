CFLAGS=-std=c++14 -Wfatal-errors -Wall -Werror -Wextra -pedantic -Wno-unused-parameter
SRC=./src/Main.cpp ./src/Cartidge.cpp ./src/CartReader.cpp ./src/CPU.cpp ./src/Gameboy.cpp ./src/GPU.cpp ./src/IO.cpp ./src/Memory.cpp ./src/MMU.cpp
LDIR=-L./lib
IDIR=-I./include -I./src
LIBS=-lmingw32 -lSDL2main -lSDL2

# if linker fails on mingw, due to "vsnprintf", 
# then do: "Installing MinGW packages mingw32-libmingwex-* will link an appropriate version of vsnprintf and avoid the linker error."

.PHONY: main
main:
	g++ $(CFLAGS) $(SRC) $(IDIR) $(LDIR) $(LIBS) -o ./bin/gbemu.exe

#blaarg cpu tests

.PHONY: cputestALL
cputestALL:
	cd ./bin && ./gbemu.exe ../tests/blaarg/cpu_instrs/"cpu_instrs.gb"

.PHONY: cputest01
cputest01:
	cd ./bin && ./gbemu.exe ../tests/blaarg/cpu_instrs/individual/"01-special.gb"

.PHONY: cputest02
cputest02:
	cd ./bin && ./gbemu.exe ../tests/blaarg/cpu_instrs/individual/"02-interrupts.gb"

.PHONY: cputest03
cputest03:
	cd ./bin && ./gbemu.exe ../tests/blaarg/cpu_instrs/individual/"03-op sp,hl.gb"

.PHONY: cputest04
cputest04:
	cd ./bin && ./gbemu.exe ../tests/blaarg/cpu_instrs/individual/"04-op r,imm.gb"

.PHONY: cputest05
cputest05:
	cd ./bin && ./gbemu.exe ../tests/blaarg/cpu_instrs/individual/"05-op rp.gb"

.PHONY: cputest06
cputest06:
	cd ./bin && ./gbemu.exe ../tests/blaarg/cpu_instrs/individual/"06-ld r,r.gb"

.PHONY: cputest07
cputest07:
	cd ./bin && ./gbemu.exe ../tests/blaarg/cpu_instrs/individual/"07-jr,jp,call,ret,rst.gb"

.PHONY: cputest08
cputest08:
	cd ./bin && ./gbemu.exe ../tests/blaarg/cpu_instrs/individual/"08-misc instrs.gb"

.PHONY: cputest09
cputest09:
	cd ./bin && ./gbemu.exe ../tests/blaarg/cpu_instrs/individual/"09-op r,r.gb"

.PHONY: cputest10
cputest10:
	cd ./bin && ./gbemu.exe ../tests/blaarg/cpu_instrs/individual/"10-bit ops.gb"

.PHONY: cputest11
cputest11:
	cd ./bin && ./gbemu.exe ../tests/blaarg/cpu_instrs/individual/"11-op a,(hl).gb"