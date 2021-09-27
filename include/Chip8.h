#ifndef CHIP8_H
#define CHIP8_H

#include <array>
#include <string>

class Chip8 {
    public:
        Chip8() {}

        std::array<unsigned char, 64 * 32> gfx;
        std::array<unsigned char, 16> keys;

        void initialize();
        void load_program(std::string path);
        void emulate_cycle();
    private:
        std::array<unsigned char, 4096> memory;

        std::array<unsigned char, 16> regs;
        unsigned short I;
        unsigned short pc;

        std::array<unsigned short, 16> stack;
        unsigned short sp;

        unsigned char delay_timer;
        unsigned char sound_timer;

        void setup_graphics();
        void setup_input();

        void clear_display();

        void call_subroutine(unsigned short addr);
        void return_from_subroutine();

        void handle_opcode();

        void handle_opcode_0(unsigned short opcode);
        void inline handle_opcode_1(unsigned short opcode);
        void inline handle_opcode_2(unsigned short opcode);
        void inline handle_opcode_3(unsigned short opcode);
        void inline handle_opcode_4(unsigned short opcode);
        void inline handle_opcode_5(unsigned short opcode);
        void inline handle_opcode_6(unsigned short opcode);
        void inline handle_opcode_7(unsigned short opcode);
        void handle_opcode_8(unsigned short opcode);
        void inline handle_opcode_9(unsigned short opcode);
        void inline handle_opcode_A(unsigned short opcode);
        void inline handle_opcode_B(unsigned short opcode);
        void handle_opcode_C(unsigned short opcode);
        void handle_opcode_D(unsigned short opcode);
        void handle_opcode_E(unsigned short opcode);
        void handle_opcode_F(unsigned short opcode);
};

const std::array<unsigned char, 80> chip8_fontset =
{
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

#endif
