#pragma once
#include <cstdint>
#include <chrono>
#include <random>

const unsigned int ROM_START_ADDRESS = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x50;

class Chip8 {
public:
	Chip8();
private:
	uint16_t opcode;

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
	
	
	std::default_random_engine randGen;
	std::uniform_int_distribution<int> randByte;

	void load_rom(const char* romName);
	void load_fonts();
};
