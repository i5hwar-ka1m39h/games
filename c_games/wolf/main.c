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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#define SCREEN_HEIGHT 720
#define SCREEN_WIDTH 1280
#define MAZE_COL 20
#define MAZE_ROW 20

typedef struct {

  SDL_Renderer *rndr;
  SDL_Window *wndw;
} App;

typedef struct {
  int x;
  int y;
} PlayerPos;

int maze[MAZE_ROW][MAZE_COL] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

bool Is_wall(int row, int col) {
  if (col < 0 || col >= MAZE_COL || row < 0 || row >= MAZE_ROW) {
    return true;
  }
  return maze[row][col] == 1;
}
void RenderBlock(App *app) {
  int block_height = SCREEN_HEIGHT / MAZE_ROW;
  int block_width = SCREEN_WIDTH / MAZE_COL;

  for (int i = 0; i < MAZE_ROW; i++) {
    for (int j = 0; j < MAZE_COL; j++) {
      SDL_Rect block = {i * block_width, j * block_height, block_width,
                        block_height};

			Uint8 col;
			if(Is_wall(i, j)){
					col = 255;
			}else{
				col = 0;
			}
      SDL_SetRenderDrawColor(app->rndr, col, 0, 0, 255);

      SDL_RenderFillRect(app->rndr, &block);
    }
  }

  SDL_RenderPresent(app->rndr);
}
int main() {

  App app;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("error occured while initializing sdl %s", SDL_GetError());
    exit(1);
  }

  app.wndw =
      SDL_CreateWindow("chikat", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

  if (!app.wndw) {
    printf("error occured while creating window %s", SDL_GetError());
    exit(1);
  }

  app.rndr = SDL_CreateRenderer(app.wndw, -1, SDL_RENDERER_ACCELERATED);

  if (!app.rndr) {
    printf("error creating render %s", SDL_GetError());
    exit(1);
  }

  SDL_Surface *surface = SDL_GetWindowSurface(app.wndw);

  SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));

  SDL_UpdateWindowSurface(app.wndw);

  SDL_RenderClear(app.rndr);

  SDL_RenderPresent(app.rndr);

  RenderBlock(&app);

  SDL_Event e;
  bool quit = false;
  while (quit == false) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
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
