#ifndef __WINDOW_H__
#define __WINDOW_H__

#include<GLFW/glfw3.h>
#include <stdlib.h>

#include"Defines.h"

#ifdef DEBUG
#include <stdio.h>
#endif

#define GLFWINIT_FAIL -1
#define WINDOW_FAIL -2
#define WINDOW_OK 0


typedef struct {
  int width;
  int height;
  const char* name;
  GLFWmonitor *monitor;
  GLFWwindow* window;
} Window;

void windowHint(int hint, int val);
void windowHintStr(int hint, const char* val);
Window* createWindow(int width, int height, const char* name, int isFullscreen, GLFWmonitor* monitor, GLFWwindow* GLFWwindow);
void freeWindow(Window* window);

#endif
