//
//  Chip8Emulation.h
//  Chip8Emulation
//
//  Created by leon on 14-10-5.
//  Copyright (c) 2014年 lyon. All rights reserved.
//

#ifndef __Chip8Emulation__Chip8Emulation__
#define __Chip8Emulation__Chip8Emulation__

#include <stdio.h>

class Chip8Emulation
{
public:
    Chip8Emulation();
    ~Chip8Emulation();
    
    void                Init();
    void                Step();
    void                Draw();
    bool                LoadROM(const char* file_name);
    
    bool                        inited;
    bool                        flag_draw;
    unsigned char               buffer_gfx[64 * 32];
    unsigned char               buffer_scene[64][32][3];
    unsigned char               buffer_input[16];
private:
    unsigned short              opcode;
    unsigned short              pc; //program counter
    unsigned short              ir; //index register
    unsigned short              sp; //stack pointer
    
    unsigned char               V[16]; //registers
    unsigned short              stack[16];
    unsigned char               memory[1024 * 4];
    
    unsigned char               timer_delay;
    unsigned char               timer_sound;
};

#endif /* defined(__Chip8Emulation__Chip8Emulation__) */
