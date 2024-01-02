#include <stdexcept>
#include <iostream>
#include "SDLWindowNoOpenGL.hpp"

namespace SDLNoOpenGL
{
    
SDLWindow::SDLWindow(const std::string &windowTitle)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        throw std::runtime_error("Failure: SDL_Init");
    }
    
    sdlWindow = SDL_CreateWindow((windowTitle + windowSubtitle).c_str(), 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        windowWidth, windowHeight, 
        SDL_WINDOW_RESIZABLE | 
        SDL_WINDOW_MAXIMIZED | 
        SDL_WINDOW_ALLOW_HIGHDPI
    );

    if (!sdlWindow)
    {
        throw std::runtime_error("Failure: SDL_CreateWindow");
    }

    sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!sdlRenderer)
    {
        throw std::runtime_error("Failure: SDL_CreateRenderer");
    }

    PrintSDLVersionInfo();
}

SDLWindow::~SDLWindow()
{
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
}

void SDLWindow::RunFrameLoop()
{
    bool isQuit {false};

    while (!isQuit)
    {
        EnforceFrameRateLimit enforceFrameRateLimit {60u, 5u};
        RunEventsLoop(isQuit, enforceFrameRateLimit.GetSetViaRefIsGuiIdle());
        UpdateScene();
        DrawScene();
        PresentScene();
    }
}

void SDLWindow::RunEventsLoop(bool& isQuit, bool& isGuiIdle)
{
    static bool isMaximized {true}; // Starts off maximized due to flag SDL_WINDOW_MAXIMIZED
    static bool isFullScreen {false};

    SDL_Event event;
    
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            isQuit = true;
            break;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_CLOSE
                && event.window.windowID == SDL_GetWindowID(sdlWindow))
            {
                isQuit = true;
            }
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.mod == KMOD_RSHIFT && event.key.keysym.sym == SDLK_ESCAPE)
            {
                isQuit = true;
            }
            else if (event.key.keysym.sym == SDLK_F1)
            {
                if (isFullScreen)
                {
                    SDL_SetWindowFullscreen(sdlWindow, 0);
                    isFullScreen = false;
                }

                if (isMaximized)
                {
                    SDL_SetWindowSize(sdlWindow, windowWidth, windowHeight);
                    SDL_SetWindowPosition(sdlWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                    SDL_RestoreWindow(sdlWindow);
                }
                else
                {
                    SDL_MaximizeWindow(sdlWindow);
                }

                isMaximized = !isMaximized;
            }
            else if (event.key.keysym.sym == SDLK_F2)
            {
                if (isFullScreen)
                {
                    SDL_SetWindowFullscreen(sdlWindow, 0);
                    SDL_SetWindowSize(sdlWindow, windowWidth, windowHeight);
                    SDL_SetWindowPosition(sdlWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                    SDL_RestoreWindow(sdlWindow);
                }
                else
                {
                    SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
                }

                isMaximized = false;
                isFullScreen = !isFullScreen;
            }
            break;
        }
    }
}

void SDLWindow::UpdateScene()
{
    SDL_SetRenderDrawColor(sdlRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(sdlRenderer);
}

void SDLWindow::DrawScene()
{
    SDL_SetRenderDrawColor(sdlRenderer, 0xFF, 0x00, 0xFF, 0xFF);
    SDL_Rect rect = {0, 0, 100, 100}; // x, y, width, height
    SDL_RenderFillRect(sdlRenderer, &rect);
}

void SDLWindow::PresentScene()
{
    SDL_RenderPresent(sdlRenderer);
}

void SDLWindow::PrintSDLVersionInfo()
{
    SDL_version version;
    SDL_VERSION(&version);
    // std::cout << "SDL version: " << version.major << "." << version.minor << "." << version.patch << "\n"; 
    SDL_Log("SDL version %u.%u.%u \n", version.major, version.minor, version.patch);
}

SDLWindow::EnforceFrameRateLimit::EnforceFrameRateLimit(unsigned int framesPerSecBusy, unsigned int framesPerSecIdle)
    : framesPerSecBusy(framesPerSecBusy)
    , framesPerSecIdle(framesPerSecIdle)
    , timeStartProcessFrameMs(SDL_GetTicks64())
{}

SDLWindow::EnforceFrameRateLimit::~EnforceFrameRateLimit()
{
    frameDurationMs = isGuiIdle ? 1000 / framesPerSecIdle : 1000 / framesPerSecBusy;

    unsigned int timeTakenToProcessFrameMs = SDL_GetTicks64() - timeStartProcessFrameMs;

    if (timeTakenToProcessFrameMs < frameDurationMs)
    {
        SDL_Delay(frameDurationMs - timeTakenToProcessFrameMs);
    }
}

} // ~namespace SDLNoOpenGL
