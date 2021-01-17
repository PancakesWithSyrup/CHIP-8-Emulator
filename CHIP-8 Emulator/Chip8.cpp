#include "Chip8.h"
#include <iostream>
#include <fstream>
#include <algorithm>

Chip8::Chip8() 
	: randomGen(std::chrono::system_clock::now().time_since_epoch().count()) {
	randomByte = std::uniform_int_distribution<int>(0, 255);
	programCounter = ROM_START_ADDRESS;
}

void Chip8::load_rom(const char* romName) {
	std::ifstream romFile(romName, std::ios::binary | std::ios::ate);

	if (!romFile.is_open()) {
		std::cout << "Failed to open ROM file!" << std::endl;
		return;
	}

	// Create a buffer to hold the ROM file temporary
	std::streampos romSize = romFile.tellg();
	char* buffer = new char[romSize];

	romFile.seekg(0, romSize);
	romFile.read(buffer, romSize);
	romFile.close();

	for (long x = 0; x < romSize; x++) {
		memory[ROM_START_ADDRESS + x] = buffer[x];
	}
	
	delete[] buffer;
}	

void Chip8::load_fonts() {
	const unsigned int FONTSET_SIZE = 80;

	uint8_t fontSet[FONTSET_SIZE] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	for (int x = 0; x < FONTSET_SIZE; x++) {
		memory[FONTSET_START_ADDRESS + x] = fontSet[x];
	}

}

// CLS - Clears the display
void Chip8::OP_00E0() {
	std::fill(video, video+2048, 0);
}

// RET - Return from a subtroutine
void Chip8::OP_00EE() {
	stackPointer--;
	programCounter = stack[stackPointer];
}

// JP addr - Jump to the address
void Chip8::OP_1nnn() {
	uint16_t address = opcode & 0x0FFF;
	programCounter = address;
}

// CALL addr - Calls the subroutine at the address
void Chip8::OP_2nnn() {
	stack[stackPointer] = programCounter;
	stackPointer++;
	uint16_t address = opcode & 0x0FFF;
	programCounter = address;
}

// SE Vx, byte - Skips the next instruction if Vx = byte
void Chip8::OP_3xkk() {
	uint8_t Vx = (opcode & 0x0F00) >> 8, byte = opcode & 0x00FF;
	if (registers[Vx] == byte) {
		programCounter += 2;
	}
}

// SNE Vx, byte - Skips the next instruction if Vx != byte
void Chip8::OP_4xkk() {
	uint8_t Vx = (opcode & 0x0F00) >> 8, byte = opcode & 0x00FF;
	if (registers[Vx] != byte) {
		programCounter += 2;
	}
}

//SE Vx, Vy - Skips the next instruction if Vx = Vy
void Chip8::OP_5xy0() {
	uint8_t Vx = (opcode & 0x0F00) >> 8, Vy = (opcode & 0x00F0) >> 4;
	if (registers[Vx] == registers[Vy]) {
		programCounter += 2;
	}
}

// LD Vx, byte - Sets Vx to byte
void Chip8::OP_6xkk() {
	uint8_t Vx = (opcode & 0x0F00) >> 8, byte = opcode & 0x00FF;
	registers[Vx] = byte;
}

// ADD Vx, byte - Vx += byte
void Chip8::OP_7xkk() {
	uint8_t Vx = (opcode & 0x0F00) >> 8, byte = opcode & 0x00FF;
	registers[Vx] += byte;
}

// LD Vx, Vy - Vx = Vy
void Chip8::OP_8xy0() {
	uint8_t Vx = (opcode & 0x0F00) >> 8, Vy = (opcode & 0x00F0) >> 4;
	registers[Vx] = registers[Vy];
}

// OR Vx, Vy - Vx |= Vy
void Chip8::OP_8xy1() {
	uint8_t Vx = (opcode & 0x0F00) >> 8, Vy = (opcode & 0x00F0) >> 4;
	registers[Vx] |= registers[Vy];
}

// AND Vx, Vy - Vx &= Vy
void Chip8::OP_8xy2() {
	uint8_t Vx = (opcode & 0x0F00) >> 8, Vy = (opcode & 0x00F0) >> 4;
	registers[Vx] &= registers[Vy];
}

// XOR Vx, Vy - Vx ^= Vy
void Chip8::OP_8xy3() {
	uint8_t Vx = (opcode & 0x0F00) >> 8, Vy = (opcode & 0x00F0) >> 4;
	registers[Vx] ^= registers[Vy];
}

// ADD Vx, Vy - Add Vx and Vy and if the result is bigger than 255 set the flag
// register to 1, otherwise set to 0. Store the lowest 8 bits of the result in Vx.
void Chip8::OP_8xy4() {
	uint8_t Vx = (opcode & 0x0F00) >> 8, Vy = (opcode & 0x00F0) >> 4;
	uint16_t sum = registers[Vx] + registers[Vy];
	registers[Vx] = sum & 0xFF;
	if (sum > 255) {
		registers[15] = 1;
	}
	else {
		registers[15] = 0;
	}
}

// SUB Vx, Vy - Do Vx -= Vf and if Vx > Vf then set VF to 1, otherwise 0
void Chip8::OP_8xy5() {
	uint8_t Vx = (opcode & 0x0F00) >> 8, Vy = (opcode & 0x00F0) >> 4;
	registers[Vx] -= registers[Vy];
	if (registers[Vx] > registers[Vy]) {
		registers[15] = 1;
	}
	else {
		registers[15] = 0;
	}
}

// SHR Vx - Set VF to 1 if the least-sig bit of Vx is 1, otherwise 0, then
// divide Vx by 2
void Chip8::OP_8xy6() {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	registers[15] = registers[Vx] & 1;
	registers[Vx] >>= 1;
}

// SUBN Vx, Vy - Set VF to 1 if Vy > Vx, otherwise set to 0, then subtract Vx
// from Vy and store in Vx
void Chip8::OP_8xy7() {
	uint8_t Vx = (opcode & 0x0F00) >> 8, Vy = (opcode & 0x00F0) >> 4;
	if (registers[Vy] > registers[Vx]) {
		registers[15] = 1;
	}
	else {
		registers[15] = 0;
	}
	registers[Vx] = registers[Vy] - registers[Vx];
}


void Chip8::OP_8xyE() {

}

void Chip8::OP_9xy0() {

}

void Chip8::OP_Annn() {

}

void Chip8::OP_Bnnn() {

}

void Chip8::OP_Cxkk() {

}

void Chip8::OP_Dxyn() {

}

void Chip8::OP_Ex9E() {

}

void Chip8::OP_ExA1() {

}

void Chip8::OP_Fx07() {

}

void Chip8::OP_Fx0A() {

}

void Chip8::OP_Fx15() {

}

void Chip8::OP_Fx18() {

}

void Chip8::OP_1E() {

}

void Chip8::OP_Fx29() {

}

void Chip8::OP_Fx33() {

}

void Chip8::OP_Fx55() {

}

void Chip8::OP_Fx65() {

}
