#define SDL_MAIN_HANDLED

#include <iostream>
#include "SDLWindowNoOpenGL.hpp"

int main()
{
    try {
        SDLNoOpenGL::SDLWindow sdlWindow("SDL Window (No OpenGL)");
        sdlWindow.RunFrameLoop();
    }
    catch (const std::runtime_error& e)
    {
        std::cout << e.what() << "\n";
    }
}
