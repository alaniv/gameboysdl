#include <iostream>

#include "Gameboy.h"
#include <array>
#include <SDL2/SDL.h>

#define TIME_PER_FRAME 16

int main(int argc, char *argv[])
{
    if(argc != 2){
      std::cout << "ROM path required" << std::endl;
      exit(1);
    }
    
    Gameboy gb;
    gb.LoadRom(argv[1]);
    gb.LoadBootRom("BOOT_ROM.bin");
    
    //SDL STUFF
    
    SDL_Init( SDL_INIT_EVERYTHING );
    atexit( SDL_Quit );

    SDL_Window* window = SDL_CreateWindow
        (
        "Gameboy Emu",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 572, SDL_WINDOW_SHOWN
        );

    SDL_Renderer* renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );



    SDL_RendererInfo info;
    SDL_GetRendererInfo( renderer, &info );
    std::cout << "Renderer name: " << info.name << std::endl;
    //std::cout << "Texture formats: " << std::endl;
    //for( Uint32 i = 0; i < info.num_texture_formats; i++ )
    //{
    //    std::cout << SDL_GetPixelFormatName( info.texture_formats[i] ) << std::endl;
    //}

    const unsigned int texWidth = 160;
    const unsigned int texHeight = 144;
    SDL_Texture* texture = SDL_CreateTexture
        (
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        texWidth, texHeight
        );


    SDL_Event e;
    bool running = true;
    Uint64 start = SDL_GetPerformanceCounter();
    while( running )
    {
        SDL_SetRenderDrawColor( renderer, 0, 0, 0, SDL_ALPHA_OPAQUE );
        SDL_RenderClear( renderer );

        while( SDL_PollEvent( &e ) )
        {
            if( ( SDL_QUIT == e.type ) ||
                ( SDL_KEYDOWN == e.type && SDL_SCANCODE_ESCAPE == e.key.keysym.scancode ) )
            {
                running = false;
                break;
            }
            else if( e.type == SDL_KEYDOWN ) {
              switch( e.key.keysym.sym ) {
                case SDLK_LEFT:
                    gb.input(GB_Key::LEFT_Press);
                    break;
                case SDLK_RIGHT:
                    gb.input(GB_Key::RIGHT_Press);
                    break;
                case SDLK_UP:
                    gb.input(GB_Key::UP_Press);
                    break;
                case SDLK_DOWN:
                    gb.input(GB_Key::DOWN_Press);
                    break;
                case SDLK_q:
                    gb.input(GB_Key::START_Press);
                    break;
                case SDLK_w:
                    gb.input(GB_Key::SELECT_Press);
                    break;
                case SDLK_z:
                    gb.input(GB_Key::B_Press);
                    break;
                case SDLK_x:
                    gb.input(GB_Key::A_Press);
                    break;
                default:
                    break;
              }
            }
            else if( e.type == SDL_KEYUP ) {
              switch( e.key.keysym.sym ) {
                case SDLK_LEFT:
                    gb.input(GB_Key::LEFT_Release);
                    break;
                case SDLK_RIGHT:
                    gb.input(GB_Key::RIGHT_Release);
                    break;
                case SDLK_UP:
                    gb.input(GB_Key::UP_Release);
                    break;
                case SDLK_DOWN:
                    gb.input(GB_Key::DOWN_Release);
                    break;
                case SDLK_q:
                    gb.input(GB_Key::START_Release);
                    break;
                case SDLK_w:
                    gb.input(GB_Key::SELECT_Release);
                    break;
                case SDLK_z:
                    gb.input(GB_Key::B_Release);
                    break;
                case SDLK_x:
                    gb.input(GB_Key::A_Release);
                    break;
                default:
                    break;
              }
            }
        }
        
        gb.emuCycle();
        if(gb.availableImage()){
          SDL_UpdateTexture
            (
            texture,
            NULL,
            &(gb.getBufferedImage())[0],
            texWidth * 4
            );

          SDL_RenderCopy( renderer, texture, NULL, NULL );
          SDL_RenderPresent( renderer );
          
          const Uint64 end = SDL_GetPerformanceCounter();
          const static Uint64 freq = SDL_GetPerformanceFrequency();
          const double seconds = ( end - start ) / static_cast< double >( freq );
          //std::cout << "Frame time: " << seconds * 1000.0 << "ms" << std::endl;
          if(1000 * seconds < 16){
            SDL_Delay( 16 - 1000 * seconds );
          }
          start = SDL_GetPerformanceCounter();
        }


    }

    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    SDL_Quit();
    return 0;
}
