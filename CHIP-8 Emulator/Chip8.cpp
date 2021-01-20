#include "Chip8.h"
#include <iostream>
#include <fstream>
#include <algorithm>

Chip8::Chip8() 
	: randomGen(std::chrono::system_clock::now().time_since_epoch().count()) {
	randomByte = std::uniform_int_distribution<int>(0, 255);
	programCounter = ROM_START_ADDRESS;
}

void Chip8::cycle() {
	opcode = (memory[programCounter] << 8) | memory[programCounter + 1];
	programCounter += 2;

	switch (opcode & 0xF000) {
		case 0x0000:
			switch (opcode & 0x00FF) {
				case 0x00E0:
					OP_00E0();
					break;
				case 0x00EE:
					OP_00EE();
					break;
				default:
					break;
			}
			break;
		case 0x1000:
			OP_1nnn();
			break;
		case 0x2000:
			OP_2nnn();
			break;
		case 0x3000:
			OP_3xkk();
			break;
		case 0x4000:
			OP_4xkk();
			break;
		case 0x5000:
			OP_5xy0();
			break;
		case 0x6000:
			OP_6xkk();
			break;
		case 0x7000:
			OP_7xkk();
			break;
		case 0x8000:
			switch (opcode & 0x000F) {
				case 0x0:
					OP_8xy0();
					break;
				case 0x1:
					OP_8xy1();
					break;
				case 0x2:
					OP_8xy2();
					break;
				case 0x3:
					OP_8xy3();
					break;
				case 0x4:
					OP_8xy4();
					break;
				case 0x5:
					OP_8xy5();
					break;
				case 0x6:
					OP_8xy6();
					break;
				case 0x7:
					OP_8xy7();
					break;
				case 0xE:
					OP_8xyE();
					break;
				default:
					break;
			}
		case 0x9000:
			OP_9xy0();
			break;
		case 0xA000:
			OP_Annn();
			break;
		case 0xB000:
			OP_Bnnn();
			break;
		case 0xC000:
			OP_Cxkk();
			break;
		case 0xD000:
			OP_Dxyn();
			break;
		case 0xE000:
			switch (opcode & 0x00FF) {
				case 0x9E:
					OP_Ex9E();
					break;
				case 0xA1:
					OP_ExA1();
					break;
				default:
					break;
			}
			break;
		case 0xF000:
			switch (opcode & 0x00FF) {
				case 0x07:
					OP_Fx07();
					break;
				case 0x0A:
					OP_Fx0A();
					break;
				case 0x15:
					OP_Fx15();
					break;
				case 0x18:
					OP_Fx18();
					break;
				case 0x1E:
					OP_Fx1E();
					break;
				case 0x29:
					OP_Fx29();
					break;
				case 0x33:
					OP_Fx33();
					break;
				case 0x55:
					OP_Fx55();
					break;
				case 0x65:
					OP_Fx65();
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}

	if (delayTimer > 0) {
		delayTimer--;
	}
	if (soundTimer > 0) {
		soundTimer--;
	}
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

// SHL Vx {, Vy} - Set VF to 1 if Vx's most-siginficant byte is 1 then multiply
// Vx by 2
void Chip8::OP_8xyE() {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	registers[15] = (registers[Vx] & 0x80) >> 7;
	registers[Vx] <<= 1;
}

// SNE Vx, Vy - Skips the next instruction if Vx != Vy
void Chip8::OP_9xy0() {
	uint8_t Vx = (opcode & 0x0F00) >> 8, Vy = (opcode & 0x00F0) >> 4;
	if (registers[Vx] != registers[Vy]) {
		programCounter += 2;
	}
}

// LD I, addr - sets I = nnn
void Chip8::OP_Annn() {
	uint16_t address = opcode & 0x0FFF;
	index = address;
}

// JP V0, addr - Jump to the address at nnn + V0
void Chip8::OP_Bnnn() {
	uint16_t address = opcode & 0x0FFF;
	programCounter = registers[0] + address;
}

// RND Vx, byte - Set Vx = randomByte & kk
void Chip8::OP_Cxkk() {
	uint8_t Vx = (opcode & 0x0F00) >> 8, byte = opcode & 0x00FF;
	registers[Vx] = randomByte(randomGen) & byte;
}


// DRW Vx, Vy, nibble - Draw the sprite at memory address I at (Vx, Vy) and
//  set VF = 1 if there is a collision
void Chip8::OP_Dxyn() {
	uint8_t Vx = (opcode & 0x0F00) >> 8, Vy = (opcode & 0x00F0) >> 4, height = opcode & 0x000F;
	uint8_t xPos = registers[Vx] % VIDEO_WIDTH, yPos = registers[Vy] % VIDEO_HEIGHT;
	registers[15] = 0;
	for (int x = 0; x < height; x++) {
		uint8_t spriteByte = memory[index + x];
		for (int y = 0; y < 8; y++) {
			uint8_t spritePixel = spriteByte & (0x80 >> y);
			uint32_t* screenPixel = &video[(yPos + x) * VIDEO_WIDTH + (xPos + y)];

			if (spritePixel)
			{
				// Screen pixel also on - collision
				if (*screenPixel == 0xFFFFFFFF)
				{
					registers[15] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

// SKP Vx - Skips the next instruction if a key with the value in Vx is pressed
void Chip8::OP_Ex9E() {
	uint8_t Vx = (opcode & 0x0F00) >> 8, key = registers[Vx];
	if (keys[key]) {
		programCounter += 2;
	}
}

// SKNP Vx - Skips the next instruction if a key with the value in Vx is not pressed
void Chip8::OP_ExA1() {
	uint8_t Vx = (opcode & 0x0F00) >> 8, key = registers[Vx];
	if (!keys[key]) {
		programCounter += 2;
	}
}

// LD Vx, DT - Set Vx to the delay timer value
void Chip8::OP_Fx07() {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	registers[Vx] = delayTimer;
}

// LD Vx, K - Wait for a key press then store the key value in Vx
void Chip8::OP_Fx0A() {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	if (keys[0]) {
		registers[Vx] = 0;
	}
	else if (keys[1]) {
		registers[Vx] = 1;
	}
	else if (keys[2]) {
		registers[Vx] = 2;
	}
	else if (keys[3]) {
		registers[Vx] = 3;
	}
	else if (keys[4]) {
		registers[Vx] = 4;
	}
	else if (keys[5]) {
		registers[Vx] = 5;
	}
	else if (keys[6]) {
		registers[Vx] = 6;
	}
	else if (keys[7]) {
		registers[Vx] = 7;
	}
	else if (keys[8]) {
		registers[Vx] = 8;
	}
	else if (keys[9]) {
		registers[Vx] = 9;
	}
	else if (keys[10]) {
		registers[Vx] = 10;
	}
	else if (keys[11]) {
		registers[Vx] = 11;
	}
	else if (keys[12]) {
		registers[Vx] = 12;
	}
	else if (keys[13]) {
		registers[Vx] = 13;
	}
	else if (keys[14]) {
		registers[Vx] = 14;
	}
	else if (keys[15]) {
		registers[Vx] = 15;
	}
	else {
		// Decrement the program counter by 2 to rerun this instruction if nothing
		// is pressed
		programCounter -= 2;
	}
}

// LD DT, Vx - Sets the delay time to Vx
void Chip8::OP_Fx15() {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	delayTimer = registers[Vx];
}

// LD ST, Vx - Sets sound timer to Vx
void Chip8::OP_Fx18() {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	soundTimer = registers[Vx];
}

// ADD I, Vx - Sets index += Vx
void Chip8::OP_Fx1E() {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	index += registers[Vx];
}

// LD F, Vx - Sets index to the sprite location for the digit in Vx
void Chip8::OP_Fx29() {
	uint8_t Vx = (opcode & 0x0F00) >> 8, digit = registers[Vx];
	// Each digit is 5 bytes
	index = FONTSET_START_ADDRESS + (5 * digit);
}

// LD B, Vx - Store the binary-coded decimal version of Vx in I, I+1, I+2
void Chip8::OP_Fx33() {
	uint8_t Vx = (opcode & 0x0F00) >> 8, value = registers[Vx];
	// Ones-place
	memory[index + 2] = value % 10;
	value /= 10;

	// Tens-place
	memory[index + 1] = value % 10;
	value /= 10;

	// Hundreds-place
	memory[index] = value % 10;

}

// LD [I], Vx - Store registers V0 - Vx in memory at [I]
void Chip8::OP_Fx55() {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	for (int i = 0; i <= Vx; i++) {
		memory[index = i] = registers[i];
	}
}

// LD Vx, [I] - Read registers V0 - Vx in memory at [I]
void Chip8::OP_Fx65() {
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	for (int i = 0; i <= Vx; i++) {
		registers[i] = memory[index + i];
	}
}
