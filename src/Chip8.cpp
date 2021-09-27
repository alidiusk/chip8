#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdio.h>

#include "Chip8.h"

void Chip8::initialize() {
    pc = 0x200;
    I = 0;
    sp = 0;

    // Clear display
    gfx.fill(0);
    // Clear stack
    stack.fill(0);
    // Clear registers
    regs.fill(0);
    // Clear memory
    memory.fill(0);

    // Load fontset
    for (unsigned long i = 0; i < chip8_fontset.size(); i++) {
        memory[i] = chip8_fontset[i];
    }

    delay_timer = 0;
    sound_timer = 0;
}

void Chip8::load_program(std::string path) {
    std::streampos size;
    std::ifstream file (path, std::ios::in|std::ios::binary|std::ios::ate);

    unsigned char* program_loc = &memory[0x200];

    if (file.is_open()) {
        size = file.tellg();
        // Check and verify that size is not too large for memory
        if (size > 0xC9F) {
            throw std::runtime_error("Program is too large!");
        }

        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(program_loc), size);
        file.close();
    } else {
        throw std::runtime_error("Unable to open program file!");
    }

    // for (unsigned long i = 0; i < memory.size(); i++) {
    //     if (i % 8 == 0) {
    //         printf("\n");
    //         printf("%08lx: ", i);
    //     }
    //     printf("%04x ", memory[i]);
    // }
    // printf("\n");
}

void Chip8::emulate_cycle() {
    printf("PC: %04x\tOpcode: %04x\n", pc, (memory[pc] << 8) | memory[pc + 1]);
    handle_opcode();

    if (delay_timer > 0)
        delay_timer--;

    if (sound_timer > 0) {
        if (sound_timer == 1) {
            // TODO: Play noise
            std::cout << "BEEP!" << std::endl;
        }
        sound_timer--;
    }
}

void Chip8::handle_opcode() {
    int first = memory[pc];
    int second = memory[pc + 1];
    unsigned short opcode = static_cast<unsigned short>((first << 8) | (second & 0x00FF));
    pc += 2;

    unsigned short top = opcode & 0xF000;

    switch (top) {
        case 0x0000:
            handle_opcode_0(opcode);
            break;
        case 0x1000:
            handle_opcode_1(opcode);
            break;
        case 0x2000:
            handle_opcode_2(opcode);
            break;
        case 0x3000:
            handle_opcode_3(opcode);
            break;
        case 0x4000:
            handle_opcode_4(opcode);
            break;
        case 0x5000:
            handle_opcode_5(opcode);
            break;
        case 0x6000:
            handle_opcode_6(opcode);
            break;
        case 0x7000:
            handle_opcode_7(opcode);
            break;
        case 0x8000:
            handle_opcode_8(opcode);
            break;
        case 0x9000:
            handle_opcode_9(opcode);
            break;
        case 0xA000:
            handle_opcode_A(opcode);
            break;
        case 0xB000:
            handle_opcode_B(opcode);
            break;
        case 0xC000:
            handle_opcode_C(opcode);
            break;
        case 0xD000:
            handle_opcode_D(opcode);
            break;
        case 0xE000:
            handle_opcode_E(opcode);
            break;
        case 0xF000:
            handle_opcode_F(opcode);
            break;
        default:
            std::cout << "Opcode not supported!" << std::endl;
            break;
    }
}

void Chip8::handle_opcode_0(unsigned short opcode) {
    if (opcode == 0x00E0) {
        clear_display();
    } else if (opcode == 0x00EE) {
        return_from_subroutine();
    }
}

void inline Chip8::handle_opcode_1(unsigned short opcode) {
    // Opcode: 1NNN
    // Only one possible opcode: jump to address NNN
    pc = opcode & 0x0FFF;
}

void Chip8::clear_display() {
    gfx.fill(0);
}

void inline Chip8::handle_opcode_2(unsigned short opcode) {
    // Opcode: 2NNN
    // Only one possible opcode: calls subroutine at address NNN.
    call_subroutine(opcode & 0x0FFF);
}

void inline Chip8::handle_opcode_3(unsigned short opcode) {
    // Opcode: 0x3XNN
    // Only one possible opcode: `if (Vx == NN)`
    unsigned short X = (opcode & 0x0F00) >> 8;
    unsigned short NN = opcode & 0x00FF;

    if (regs[X] == NN) {
        pc += 2;
    }
}

void inline Chip8::handle_opcode_4(unsigned short opcode) {
    // Opcode: 0x4XNN
    // Only one possible opcode: `if (Vx != NN)`
    unsigned short X = (opcode & 0x0F00) >> 8;
    unsigned short NN = opcode & 0x00FF;

    if (regs[X] != NN) {
        pc += 2;
    }
}

void inline Chip8::handle_opcode_5(unsigned short opcode) {
    // Opcode: 0x5XY0
    // Only one possible opcode: `if (Vx == Vy)`
    unsigned short X = (opcode & 0x0F00) >> 8;
    unsigned short Y = (opcode & 0x00F0) >> 4;

    if (regs[X] == regs[Y]) {
        pc += 2;
    }
}

void inline Chip8::handle_opcode_6(unsigned short opcode) {
    // Opcode: 0x6XNN
    // Only one possible opcode: `Vx = NN`
    unsigned short X = (opcode & 0x0F00) >> 8;
    unsigned short NN = opcode & 0x00FF;

    regs[X] = static_cast<unsigned char>(NN);
}

void inline Chip8::handle_opcode_7(unsigned short opcode) {
    // Opcode: 0x7XNN
    // Only one possible opcode: `Vx = NN`
    unsigned short X = (opcode & 0x0F00) >> 8;
    unsigned short NN = opcode & 0x00FF;

    regs[X] += NN;
}

void Chip8::handle_opcode_8(unsigned short opcode) {
    // Opcode: 0x8XYN
    unsigned short X = (opcode & 0x0F00) >> 8;
    unsigned short Y = (opcode & 0x00F0) >> 4;
    unsigned short N = opcode & 0x000F;

    switch (N) {
        case 0x0:
            regs[X] = regs[Y];
            break;
        case 0x1:
            regs[X] = regs[X] | regs[Y];
            break;
        case 0x2:
            regs[X] = regs[X] & regs[Y];
            break;
        case 0x3:
            regs[X] = regs[X] ^ regs[Y];
            break;
        case 0x4:
            regs[X] += regs[Y];
            break;
        case 0x5:
            regs[X] -= regs[Y];
            break;
        case 0x6:
            regs[X] >>= 1;
            break;
        case 0x7:
            regs[X] = regs[Y] - regs[X];
            break;
        case 0xE:
            regs[X] <<= 1;
            break;
        default:
            throw std::runtime_error("Invalid opcode 8XYN encountered!");
    }
}

void inline Chip8::handle_opcode_9(unsigned short opcode) {
    // Opcode: 0x9XY0
    // Only one possible opcode: `if (Vx != Vy)`
    unsigned short X = (opcode & 0x0F00) >> 8;
    unsigned short Y = (opcode & 0x00F0) >> 4;

    if (regs[X] != regs[Y]) {
        pc += 2;
    }
}

void inline Chip8::handle_opcode_A(unsigned short opcode) {
    // Opcode: ANNN
    // Only one possible opcode: `I = NNN`.
    I = opcode & 0x0FFF;
}

void inline Chip8::handle_opcode_B(unsigned short opcode) {
    // Opcode: BNNN
    // Only one possible opcode: `PC = V0 + NNN`.
    pc = regs[0] + (opcode & 0x0FFF);
}

void Chip8::handle_opcode_C(unsigned short opcode) {
    // Opcode: CXNN
    // Only one possible opcode: `Vx = rand() & NN`.
    unsigned short X = (opcode & 0x0F00) >> 8;
    unsigned short NN = opcode & 0x00FF;

    regs[X] = static_cast<unsigned char>((rand() % 256) & NN);
}

void Chip8::handle_opcode_D(unsigned short opcode) {
    // Opcode: DXYN
    // Only one possible opcode: `draw(Vx, Vy, N)`.
    unsigned short X = (opcode & 0x0F00) >> 8;
    unsigned short Y = (opcode & 0x00F0) >> 4;
    unsigned short N = opcode & 0x000F;

    unsigned short start_col = regs[X];
    unsigned short start_row = regs[Y];

    regs[15] = 0;
    for (unsigned short i = 0; i < N; i++) {
        unsigned short mem_value = memory[I + i];
        unsigned short row = start_row + i;

        for (unsigned short j = 0; j < 8; j++) {
            unsigned short col = start_col + j;
            unsigned short pixel = (mem_value & (0x80 >> j)) >> (7 - j);
            unsigned short gfx_index = (col % 64) + (row % 32) * 64;
            unsigned short gfx_value = gfx[gfx_index];
            unsigned short xored = gfx_value ^ pixel;

            regs[15] |= gfx_value && pixel;
            gfx[gfx_index] = static_cast<unsigned char>(xored);
        }
    }
}

void Chip8::handle_opcode_E(unsigned short opcode) {
    // Opcode: EX9E or EXA1
    unsigned short X = (opcode & 0x0F00) >> 8;
    if ((opcode & 0x00FF) == 0x009E) {
        if (keys[regs[X]]) {
            pc += 2;
        }
    } else {
        if (!keys[regs[X]]) {
            pc += 2;
        }
    }
}

void Chip8::handle_opcode_F(unsigned short opcode) {
    // Opcode: FXNN
    unsigned short X = (opcode & 0x0F00) >> 8;
    unsigned short NN = opcode & 0x00FF;

    switch (NN) {
        case 0x07:
            regs[X] = delay_timer;
            break;
        case 0x0A: {
            // Wait for key press, then store in Vx
            // BLOCKING OPERATION

            bool key_pressed = false;

            for (unsigned long i = 0; i < keys.size(); i++) {
                if (keys[i] != 0) {
                    regs[X] = static_cast<unsigned char>(i);
                    key_pressed = true;
                    break;
                }
            }

            // Rerun instruction until key is pressed
            if (!key_pressed)
                pc -= 2;

            break;
        }
        case 0x15:
            delay_timer = regs[X];
            break;
        case 0x18:
            sound_timer = regs[X];
            break;
        case 0x1E:
            I += regs[X];
            break;
        case 0x29:
            // Sets I to location of sprite for char in Vx
            // Chars 0-F are represented by a 4x5 font.
            printf("%x %x\n", X, regs[X]);
            I = regs[X] * 5;
            break;
        case 0x33: {
            // Stores the BCD representation of Vx at I
            unsigned char value = regs[X];

            unsigned char ones = value % 10;
            value = value / 10;
            unsigned char tens = value % 10;
            unsigned char hundreds = value / 10;

            memory[I] = hundreds;
            memory[I + 1] = tens;
            memory[I + 2] = ones;

            break;
        }
        case 0x55:
            // Stores V0 to Vx in memory starting at address I.
            // I is left unmodified.
            for (unsigned long i = 0; i <= X; i++) {
                memory[I + i] = regs[i];
            }
            break;
        case 0x65:
            // Fills V0 to Vx with values in memory starting at address I.
            // I is left unmodified.
            for (unsigned long i = 0; i <= X; i++) {
                regs[i] = memory[I + i];
            }
            break;
        default:
            throw std::runtime_error("Invalid opcode FXNN encountered!");
    }
}

void Chip8::call_subroutine(unsigned short addr) {
    if (sp == 16) {
        throw std::runtime_error("No place on stack for return address!");
    }

    stack[sp++] = pc;
    pc = addr;
}

void Chip8::return_from_subroutine() {
    if (sp == 0) {
        throw std::runtime_error("Nowhere to return to on stack!");
    }

    pc = stack[--sp];
}
