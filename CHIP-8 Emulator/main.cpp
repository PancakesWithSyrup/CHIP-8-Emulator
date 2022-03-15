#include <iostream>
#include <chrono>

#include "Chip8.h"
#include "Window.h"

int main(int argc, char* argv[]) {
	int videoScale = 8, cycleDelay = 1;
	char const* romFilename = "test_opcode.ch8";

	Window window("CHIP-8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, VIDEO_WIDTH, VIDEO_HEIGHT);

	Chip8 chip8;
	chip8.load_rom(romFilename);

	int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while (!quit)
	{
		quit = window.processInput(chip8.keys);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > cycleDelay)
		{
			lastCycleTime = currentTime;

			chip8.cycle();

			window.update(chip8.video, videoPitch);
		}
	}

    return 0;
}