all:
	g++ main.cpp -o chip8_emulator `sdl2-config --cflags --libs`
