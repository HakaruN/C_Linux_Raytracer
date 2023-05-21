#include "../include/Window.h"

void windowHint(int hint, int val)
{
  glfwWindowHint(hint, val);
}

void windowHintStr(int hint, const char* val)
{
  glfwWindowHintString(hint, val);
}


Window* createWindow(int width, int height, const char* name, int isFullscreen, GLFWmonitor* monitor, GLFWwindow* GLFWwindow)
{
  Window* window = malloc(sizeof(Window));
  #ifdef DEBUG
  printf("Window object allocated\n");
  #endif
  if(window)
    {
      //create window object
      window->width = width;
      window->height = height;
      window->name = name;
      window->monitor = monitor;
      window->window = glfwCreateWindow(window->width, window->height, window->name, isFullscreen ? window->monitor : NULL, NULL);
#ifdef DEBUG
      printf("Creating window: %dx%d\n", width, height);
#endif

      if(!window->window)
	{
	  glfwTerminate();
	  #ifdef DEBUG
	  printf("ERROR creating GLFW window\n");
	  #endif
	  return NULL;
	}
      else{
	#ifdef DEBUG
	printf("GLFW window object allocated\n");
	#endif
      }
      glfwMakeContextCurrent(window->window);
      return window;
    }
  #ifdef DEBUG
  printf("ERROR allocating window object\n");
  #endif
  return NULL;
}


void freeWindow(Window* window)
{
  if(window->window)
    glfwDestroyWindow(window->window);
  if(window)
    free(window);
  #ifdef DEBUG
  printf("Freed window\n");
  #endif
}
