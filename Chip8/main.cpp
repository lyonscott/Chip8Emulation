//
//  main.cpp
//  Chip8-SIM
//
//  Created by leon on 14-10-5.
//  Copyright (c) 2014年 lyon. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "GLFW/glfw3.h"

#include "Chip8Emulation.h"

int ratio=5;
Chip8Emulation *chip8;

void inputKeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    if(key==GLFW_KEY_1 && action==GLFW_PRESS)chip8->buffer_input[0x1]=1;
    if(key==GLFW_KEY_2 && action==GLFW_PRESS)chip8->buffer_input[0x2]=1;
    if(key==GLFW_KEY_3 && action==GLFW_PRESS)chip8->buffer_input[0x3]=1;
    if(key==GLFW_KEY_4 && action==GLFW_PRESS)chip8->buffer_input[0xc]=1;
    if(key==GLFW_KEY_Q && action==GLFW_PRESS)chip8->buffer_input[0x4]=1;
    if(key==GLFW_KEY_W && action==GLFW_PRESS)chip8->buffer_input[0x5]=1;
    if(key==GLFW_KEY_E && action==GLFW_PRESS)chip8->buffer_input[0x6]=1;
    if(key==GLFW_KEY_R && action==GLFW_PRESS)chip8->buffer_input[0xD]=1;
    if(key==GLFW_KEY_A && action==GLFW_PRESS)chip8->buffer_input[0x7]=1;
    if(key==GLFW_KEY_S && action==GLFW_PRESS)chip8->buffer_input[0x8]=1;
    if(key==GLFW_KEY_D && action==GLFW_PRESS)chip8->buffer_input[0x9]=1;
    if(key==GLFW_KEY_F && action==GLFW_PRESS)chip8->buffer_input[0xE]=1;
    if(key==GLFW_KEY_Z && action==GLFW_PRESS)chip8->buffer_input[0xA]=1;
    if(key==GLFW_KEY_X && action==GLFW_PRESS)chip8->buffer_input[0x0]=1;
    if(key==GLFW_KEY_C && action==GLFW_PRESS)chip8->buffer_input[0xB]=1;
    if(key==GLFW_KEY_V && action==GLFW_PRESS)chip8->buffer_input[0xF]=1;
    
    if(key==GLFW_KEY_1 && action==GLFW_RELEASE)chip8->buffer_input[0x1]=0;
    if(key==GLFW_KEY_2 && action==GLFW_RELEASE)chip8->buffer_input[0x2]=0;
    if(key==GLFW_KEY_3 && action==GLFW_RELEASE)chip8->buffer_input[0x3]=0;
    if(key==GLFW_KEY_4 && action==GLFW_RELEASE)chip8->buffer_input[0xc]=0;
    if(key==GLFW_KEY_Q && action==GLFW_RELEASE)chip8->buffer_input[0x4]=0;
    if(key==GLFW_KEY_W && action==GLFW_RELEASE)chip8->buffer_input[0x5]=0;
    if(key==GLFW_KEY_E && action==GLFW_RELEASE)chip8->buffer_input[0x6]=0;
    if(key==GLFW_KEY_R && action==GLFW_RELEASE)chip8->buffer_input[0xD]=0;
    if(key==GLFW_KEY_A && action==GLFW_RELEASE)chip8->buffer_input[0x7]=0;
    if(key==GLFW_KEY_S && action==GLFW_RELEASE)chip8->buffer_input[0x8]=0;
    if(key==GLFW_KEY_D && action==GLFW_RELEASE)chip8->buffer_input[0x9]=0;
    if(key==GLFW_KEY_F && action==GLFW_RELEASE)chip8->buffer_input[0xE]=0;
    if(key==GLFW_KEY_Z && action==GLFW_RELEASE)chip8->buffer_input[0xA]=0;
    if(key==GLFW_KEY_X && action==GLFW_RELEASE)chip8->buffer_input[0x0]=0;
    if(key==GLFW_KEY_C && action==GLFW_RELEASE)chip8->buffer_input[0xB]=0;
    if(key==GLFW_KEY_V && action==GLFW_RELEASE)chip8->buffer_input[0xF]=0;

}

int main(int argc, const char * argv[])
{
    chip8=new Chip8Emulation();
    chip8->Init();
    if(chip8->LoadROM("chip8_games/INVADERS"))
        printf("Loaded!\n");
    
    GLFWwindow *window;
    if(!glfwInit())
        return -1;
    window=glfwCreateWindow(64*ratio, 64*ratio, "Emulation", nullptr, nullptr);
    
    if(!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glViewport(0.0f, 0.0f, 640, 640);
    glMatrixMode(GL_PROJECTION);
    glOrtho(0,640,0,640,-0.2f,100);
    glMatrixMode(GL_MODELVIEW);
    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.65f,0.65f,0.65f,1);
        
        chip8->Step();
        chip8->Draw();
        
        glfwSetKeyCallback(window,inputKeyCallBack);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
