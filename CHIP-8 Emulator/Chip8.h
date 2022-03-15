#pragma once
#include <cstdint>
#include <chrono>
#include <random>

const unsigned int ROM_START_ADDRESS = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

class Chip8 {
public:
	Chip8();

	uint16_t opcode;

	// 1-15 are general purpose, 16 is flags
	uint8_t registers[16];
	uint16_t memory[4096];
	uint16_t programCounter;
	uint16_t index;

	uint16_t stack[16];
	uint8_t stackPointer;
	
	uint8_t delayTimer;
	uint8_t soundTimer;

	uint32_t video[64 * 32];
	uint8_t keys[16];
	
	std::default_random_engine randomGen;
	std::uniform_int_distribution<int> randomByte;

	void cycle();
	void load_rom(const char* romName);
	void load_fonts();

	void OP_00E0();
	void OP_00EE();
	void OP_1nnn();
	void OP_2nnn();
	void OP_3xkk();
	void OP_4xkk();
	void OP_5xy0();
	void OP_6xkk();
	void OP_7xkk();
	void OP_8xy0();
	void OP_8xy1();
	void OP_8xy2();
	void OP_8xy3();
	void OP_8xy4();
	void OP_8xy5();
	void OP_8xy6();
	void OP_8xy7();
	void OP_8xyE();
	void OP_9xy0();
	void OP_Annn();
	void OP_Bnnn();
	void OP_Cxkk();
	void OP_Dxyn();
	void OP_Ex9E();
	void OP_ExA1();
	void OP_Fx07();
	void OP_Fx0A();
	void OP_Fx15();
	void OP_Fx18();
	void OP_Fx1E();
	void OP_Fx29();
	void OP_Fx33();
	void OP_Fx55();
	void OP_Fx65();
};
