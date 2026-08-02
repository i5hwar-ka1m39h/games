#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <stdbool.h>

#define SCREEN_HEIGHT 720
#define SCREEN_WIDTH 1280

typedef struct {

  SDL_Renderer *rndr;
  SDL_Window *wndw;
} App;

int main() {

  App app;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("error occured while initializing sdl %s", SDL_GetError());
    exit(1);
  }

  app.wndw =
      SDL_CreateWindow("chikat", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	if(!app.wndw){
		printf("error occured while creating window %s", SDL_GetError());
		exit(1);
	}


	app.rndr = SDL_CreateRenderer(app.wndw, -1 , SDL_RENDERER_ACCELERATED);

	if(!app.rndr){
		printf("error creating render %s", SDL_GetError());
		exit(1);
	}

	SDL_Surface* surface = SDL_GetWindowSurface(app.wndw);

	SDL_FillRect(surface, NULL,SDL_MapRGB(surface->format, 0xFF,0xFF,0xFF));

	SDL_UpdateWindowSurface(app.wndw
			 );

  SDL_RenderClear(app.rndr);
  SDL_SetRenderDrawColor(app.rndr, 120, 145, 160, 90);

  SDL_RenderDrawPoint(app.rndr, 50, 60);
 
  SDL_Rect rect = {50, 50, 60, 78};
  SDL_RenderFillRect(app.rndr, &rect);

  SDL_RenderPresent(app.rndr);


	SDL_Event e;
	bool quit = false;
	while(quit == false){
		while(SDL_PollEvent(&e)){
			if(e.type == SDL_QUIT){
				printf("quited the window");
				quit = true;
			}
		}
	}

	SDL_DestroyRenderer(app.rndr);
	SDL_DestroyWindow(app.wndw);
	SDL_Quit();
  return 0;
}
