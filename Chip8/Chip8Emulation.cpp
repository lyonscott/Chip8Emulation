//
//  Chip8Emulation.cpp
//  Chip8Emulation
//
//  Created by leon on 14-10-5.
//  Copyright (c) 2014年 lyon. All rights reserved.
//

#include "Chip8Emulation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "glfw3.h"

#define VX (V[(opcode&0x0F00)>>8])
#define VY (V[(opcode&0x00F0)>>4])
#define VF (V[0xF])
#define V0 (V[0])
#define NNN (opcode&0x0FFF)
#define NN (opcode&0x00FF)
#define N (opcode&0x000F)
#define STEP pc+=2
#define SKIP pc+=4
#define JMP pc=
#define RAND ((rand())&0xFF)
#define ERROR(x) printf("<Error>0x%X\n",x)

unsigned char table_font[80]
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80, //F
};

Chip8Emulation::Chip8Emulation(){}
Chip8Emulation::~Chip8Emulation(){}

void Chip8Emulation::Init()
{
    pc=0x200;
    ir=0;
    sp=0;
    opcode=0;
    timer_delay=0;
    timer_sound=0;
    flag_draw=true;
    
    memset(buffer_gfx, 0, sizeof(buffer_gfx));
    memset(buffer_input, 0, sizeof(buffer_input));
    memset(V, 0, sizeof(V));
    memset(memory, 0, sizeof(memory));
    memset(stack, 0, sizeof(stack));
    
    for(int i=0;i<80;++i)
        memory[i]=table_font[i];
    
    for(int y=0;y<32;++y)
        for(int x=0;x<64;++x)
            buffer_scene[y][x][0]=buffer_scene[y][x][1]=buffer_scene[y][x][2]=0;
    
    printf("Emulation Inited!\n");
}

void Chip8Emulation::Step()
{
    opcode=memory[pc]<<8 | memory[pc+1];
    //printf("0x%X,",opcode);
    //opcode
    switch(opcode & 0xF000)
    {
        case 0x0000:
        {
            switch(opcode & 0x000F)
            {
                case 0x0000://00E0	Clears the screen.
                {
                    for(int i=0;i<2048;i++)
                        buffer_gfx[i]=0;
                    flag_draw=true;
                    STEP;
                }break;
                case 0x000E://00EE	Returns from a subroutine.
                {
                    --sp;
                    JMP stack[sp];
                    STEP;
                }break;
                default:
                    ERROR(opcode);
            }
        }break;
        case 0x1000://1NNN	Jumps to address NNN.
        {
            JMP NNN;
        }break;
        case 0x2000://2NNN	Calls subroutine at NNN.
        {
            stack[sp]=pc;
            ++sp;
            JMP NNN;
        }break;
        case 0x3000://3XNN	Skips the next instruction if VX equals NN.
        {
            if(VX == NN)
                SKIP;
            else
                STEP;
                
        }break;
        case 0x4000://4XNN	Skips the next instruction if VX doesn't equal NN.
        {
            if(VX != NN)
                SKIP;
            else
                STEP;
        }break;
        case 0x5000://5XY0	Skips the next instruction if VX equals VY.
        {
            if(VX != VY)
                SKIP;
            else
                STEP;
        }break;
        case 0x6000://6XNN	Sets VX to NN.
        {
            VX=NN;
            STEP;
        }break;
        case 0x7000://7XNN	Adds NN to VX.
        {
            VX+=NN;
            STEP;
        }break;
        case 0x8000:
        {
            switch(opcode & 0x000F)
            {
                case 0x0000://8XY0	Sets VX to the value of VY.
                {
                    VX=VY;
                    STEP;
                }break;
                case 0x0001://8XY1	Sets VX to VX or VY.
                {
                    VX|=VY;
                    STEP;
                }break;
                case 0x0002://8XY2	Sets VX to VX and VY.
                {
                    VX&=VY;
                    STEP;
                }break;
                case 0x0003://8XY3	Sets VX to VX xor VY.
                {
                    VX^=VY;
                    STEP;
                }break;
                case 0x0004://8XY4	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
                {
                    if(VY > (0xFF - VX))
                        VF=1;
                    else
                        VF=0;
                    VX+=VY;
                    STEP;
                }break;
                case 0x0005://8XY5	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                {
                    if(VY > VX)
                        VF=0;
                    else
                        VF=1;
                    VX-=VY;
                    STEP;
                }break;
                case 0x0006://8XY6	Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
                {
                    VF=VX&0x1;
                    VX>>=1;
                    STEP;
                }break;
                case 0x0007://8XY7	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                {
                    if(VX > VY)
                        VF=0;
                    else
                        VF=1;
                    VX=VY-VX;
                    STEP;
                }break;
                case 0x000E://8XYE	Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
                {
                    VF=VX>>7;
                    VX<<=1;
                    STEP;
                }break;
                default:
                    ERROR(opcode);
            }
        }break;
        case 0x9000://9XY0	Skips the next instruction if VX doesn't equal VY.
        {
            if(VX != VY)
                SKIP;
            else
                STEP;
        }break;
        case 0xA000://ANNN	Sets I to the address NNN.
        {
            ir=NNN;
            STEP;
        }break;
        case 0xB000://BNNN	Jumps to the address NNN plus V0.
        {
            JMP NNN+V0;
            STEP;
        }break;
        case 0xC000://CXNN	Sets VX to a random number and NN.
        {
            VX=RAND&NN;
            STEP;
        }break;
        case 0xD000://Sprites stored in memory at location in index register (I), maximum 8bits wide. Wraps around the screen. If when drawn, clears a pixel, register VF is set to 1 otherwise it is zero. All drawing is XOR drawing (e.g. it toggles the screen pixels)
        {
            unsigned short x=VX;
            unsigned short y=VY;
            unsigned short h=N;
            unsigned short pixel;
            VF=0;
            for(int yl=0;yl<h;++yl)
            {
                pixel=memory[ir+yl];
                for(int xl=0;xl<8;++xl)
                {
                    if((pixel & (0x80>>xl)) != 0)
                    {
                        if(buffer_gfx[x+xl+(y+yl)*64] == 1)
                            VF=1;
                        buffer_gfx[x+xl+(y+yl)*64]^=1;
                    }
                }
            }
            flag_draw=true;
            STEP;
        }break;
        case 0xE000:
        {
            switch(opcode & 0x00FF)
            {
                case 0x009E://EX9E	Skips the next instruction if the key stored in VX is pressed.
                {
                    if(buffer_input[VX] == 1)
                        SKIP;
                    else
                        STEP;
                }break;
                case 0x00A1://EXA1	Skips the next instruction if the key stored in VX isn't pressed.
                {
                    if(buffer_input[VX] == 0)
                        SKIP;
                    else
                        STEP;
                }break;
            }
        }break;
        case 0xF000:
        {
            switch(opcode & 0x00FF)
            {
                case 0x0007://FX07	Sets VX to the value of the delay timer.
                {
                    VX=timer_delay;
                    STEP;
                }break;
                case 0x000A://FX0A	A key press is awaited, and then stored in VX.
                {
                    bool press=false;
                    for(int i=0;i<16;++i)
                    {
                        if(buffer_input[i] != 0)
                        {
                            VX=i;
                            press=true;
                        }
                    }
                    if(!press)
                        return;
                    STEP;
                }break;
                case 0x0015://FX15	Sets the delay timer to VX.
                {
                    timer_delay=VX;
                    STEP;
                }break;
                case 0x0018://FX18	Sets the sound timer to VX.
                {
                    timer_sound=VX;
                }break;
                case 0x001E://FX1E	Adds VX to I.
                {
                    if((ir+VX) > 0xFFF)
                        VF=1;
                    else
                        VF=0;
                    ir+=VX;
                    STEP;
                }break;
                case 0x0029://FX29	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
                {
                    ir=VX*0x5;
                    STEP;
                }break;
                case 0x0033://FX33	Stores the Binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.)
                {
                    memory[ir]=VX/100;
                    memory[ir+1]=(VX/10)%10;
                    memory[ir+2]=(VX%100)%10;
                    STEP;
                }break;
                case 0x0055://FX55	Stores V0 to VX in memory starting at address I.
                {
                    for(int i=0;i<=VX;++i)
                        memory[ir+i]=V[i];
                    ir+=VX+1;
                    STEP;
                }break;
                case 0x0065://FX65	Fills V0 to VX with values from memory starting at address I.
                {
                    for(int i=0;i<=VX;++i)
                        V[i]=memory[ir+i];
                    ir+=VX+1;
                    STEP;
                }break;
                default:
                    ERROR(opcode);
            }
        }break;
        default:
            ERROR(opcode);
    }
    
    if(timer_delay>0)
        --timer_delay;
    if(timer_sound>0)
    {
        if(timer_sound==1)
            printf("Beep!\n");
        --timer_sound;
    }
}

void Chip8Emulation::Draw()
{
    for(int y=0;y<32;++y)
    {
        for(int x=0;x<64;++x)
        {
            if(buffer_gfx[y*64+x] == 1)
            {
                glColor3f(1,1,1);
                int _x=x,_y=31-y;
                glBegin(GL_QUADS);
                glVertex2d(8*_x, 8*_y);
                glVertex2d(8*(_x+1), 8*_y);
                glVertex2d(8*(_x+1), 8*(_y+1));
                glVertex2d(8*_x, 8*(_y+1));
                glEnd();
            }else
            {
                glColor3f(0,0,0);
                int _x=x,_y=31-y;
                glBegin(GL_QUADS);
                glVertex2d(8*_x, 8*_y);
                glVertex2d(8*(_x+1), 8*_y);
                glVertex2d(8*(_x+1), 8*(_y+1));
                glVertex2d(8*_x, 8*(_y+1));
                glEnd();
            }
        }
    }
}

bool Chip8Emulation::LoadROM(const char *file_name)
{
    if(!inited)Init();
    printf("LoadROM:");
    FILE *file=fopen(file_name, "rb");
    if(file== nullptr)
    {
        printf("Not found file!\n");
        return false;
    }
    
    fseek(file, 0, SEEK_END);
    long size=ftell(file);
    rewind(file);
    
    char *tmp_buffer=(char*)malloc(sizeof(char)*size);
    if(tmp_buffer==nullptr)
    {
        printf("Memory fails!\n");
        return false;
    }
    
    if(size > (4096-512))
    {
        printf("Out of memory!\n");
        return false;
    }
    
    size_t result = fread (tmp_buffer, 1, size, file);
    if (result != size)
    {
        fputs("Reading error",stderr);
        return false;
    }
    
    for(int i=0;i<size;++i)
        memory[512+i]=tmp_buffer[i];
    fclose(file);
    free(tmp_buffer);
    
    return true;
}