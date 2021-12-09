#include <stdio.h>
#include <filesystem>
#include <complex>
#include <vector>
#include <iostream>
#include <BMP.h>
#include <main.h>
#include <SDL.h>
#undef main

using namespace std;



SDL_Window* window;
SDL_Surface* winSurface;
SDL_Surface* image1;



int main()
{
    #if 1
    const int height = 1080;
    const int width = 1920;  

    #else
    const int height = 720; 
    const int width = 1280;  
    #endif

    Uint32 rmask = 0x0000ff;
    Uint32 gmask = 0x00ff00;
    Uint32 bmask = 0xff0000;
    int depth = 24;
    int pitch = 3 * width;


    if (!init(height, width)) return 1;


    complex<double> middle  (-0.7, 0);
    double zoom = 3;
    int steps = 100;
    double translationConstant = 0.1;
    double zoomConstant = 0.8;


    auto* image = new unsigned char[height][width][BYTES_PER_PIXEL];
    double planeHeight;
    complex<double> topLeft;
    int i, j, k;
    int kMax, colorIndex;
    complex<double> c;
    SDL_Rect dest;

    bool gotoMainLoop = false;

    while (!gotoMainLoop) {
        SDL_Event e;

        memset(image, 0, sizeof(image[0][0]) * width * height);
        kMax = 0;

        planeHeight = zoom * ((double)height / (double)width);
        topLeft = middle + 0.5 * complex<double>(-1 * zoom, planeHeight);
        

        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                c = topLeft + complex<double>(((double)j / (double)width) * zoom, -1 * ((double)i / (double)height) * planeHeight);
                k = SequenceComplexNumber(c, steps);
                if (k > kMax){
                    kMax = k;
                }
                image[i][j][0] = (char)k;             
            }
        }

        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                colorIndex = (unsigned char)(((float)image[i][j][0] / (float)kMax) * (float)256);
                image[i][j][0] = turbo_srgb_bytes[colorIndex][0];
                image[i][j][1] = turbo_srgb_bytes[colorIndex][1];
                image[i][j][2] = turbo_srgb_bytes[colorIndex][2];
            }
        }


        SDL_Surface* image1 = SDL_CreateRGBSurfaceFrom((void*)image, width, height, depth, pitch, rmask, gmask, bmask, (Uint32)0);

        // Blit image to scaled portion of window
        dest.x = 0;
        dest.y = 0;
        dest.w = width;
        dest.h = height;
        SDL_BlitScaled(image1, NULL, winSurface, &dest);


        // Update window
        SDL_UpdateWindowSurface(window);

        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT:
                    gotoMainLoop = true;

                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_LEFT:
                            middle -= complex<double>(translationConstant * zoom, 0);
                            break;
                        case SDLK_RIGHT:
                            middle += complex<double>(translationConstant * zoom, 0);
                            break;
                        case SDLK_UP:
                            middle += complex<double>(0, translationConstant * zoom);
                            break;
                        case SDLK_DOWN:
                            middle -= complex<double>(0, translationConstant * zoom);
                            break;
                        case SDLK_a:
                            middle -= complex<double>(translationConstant * zoom, 0);
                            break;
                        case SDLK_d:
                            middle += complex<double>(translationConstant * zoom, 0);
                            break;
                        case SDLK_w:
                            middle += complex<double>(0, translationConstant * zoom);
                            break;
                        case SDLK_s:
                            middle -= complex<double>(0, translationConstant * zoom);
                            break;
                        case SDLK_e:
                            zoom *= zoomConstant;
                            break;
                        case SDLK_EQUALS:
                            zoom *= zoomConstant;
                            break;
                        case SDLK_q:
                            zoom /= zoomConstant;
                            break;
                        case SDLK_MINUS:
                            zoom /= zoomConstant;
                            break;
                        case SDLK_ESCAPE:
                            gotoMainLoop = true;     
                    }
                    break;
            }
        }
        SDL_Delay(10);
    }

    kill();

    return 0;
}

bool init(int height, int width) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cout << "Error initializing SDL: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }

    window = SDL_CreateWindow("Mandelbrot", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN || SDL_WINDOW_FULLSCREEN);
    if (!window) {
        cout << "Error creating window: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }


    winSurface = SDL_GetWindowSurface(window);
    if (!winSurface) {
        cout << "Error getting surface: " << SDL_GetError() << endl;
        system("pause");
        return false;
    }

    return true;
}

int SequenceComplexNumber(complex<double> c, int steps) {
    double x0 = c.real();
    double y0 = c.imag();
    double x = 0;
    double y = 0;
    double x2 = 0;
    double y2 = 0;
    int k;

    for (k = 0; (k < steps) && (x2 + y2 <= 4); k++) {
        y = 2 * x * y + y0;
        x = x2 - y2 + x0;
        x2 = x * x;
        y2 = y * y;
    }
    return k;
}


void kill() {
    // Free images
    SDL_FreeSurface(image1);

    // Quit
    SDL_DestroyWindow(window);
    SDL_Quit();
}