#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_HEIGHT 720
#define SCREEN_WIDTH 1280
#define MAZE_COL 20
#define MAZE_ROW 20
#define PI 3.14159265359

typedef struct {

  SDL_Renderer *rndr;
  SDL_Window *wndw;
} App;

typedef struct {
  int x;
  int y;
  float angle;
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
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
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
  int block_width = SCREEN_HEIGHT / MAZE_COL;

  for (int i = 0; i < MAZE_ROW; i++) {
    for (int j = 0; j < MAZE_COL; j++) {
      SDL_Rect block = {j * block_width, i * block_height, block_width - 1,
                        block_height - 1};

      Uint8 col;
      if (Is_wall(i, j)) {
        col = 255;
      } else {
        col = 0;
      }
      SDL_SetRenderDrawColor(app->rndr, col, 0, 0, 255);

      SDL_RenderFillRect(app->rndr, &block);
    }
  }
}

void RenderPlayer(App *app, PlayerPos *plyr) {
  SDL_Rect plybox = {plyr->x, plyr->y, 10, 10};
  SDL_SetRenderDrawColor(app->rndr, 0, 255, 0, 255);
  SDL_RenderFillRect(app->rndr, &plybox);

  int startptX = plyr->x + 10 / 2;
  int startptY = plyr->y + 10 / 2;

  int endptX = startptX + cos(plyr->angle) * 10;
  int endptY = startptY + sin(plyr->angle) * 10;

  SDL_SetRenderDrawColor(app->rndr, 0, 0, 255, 255);
  SDL_RenderDrawLine(app->rndr, startptX, startptY, endptX, endptY);
}

void RenderLine(App *app, PlayerPos *plyr) {

    int block_height = SCREEN_HEIGHT / MAZE_ROW;
    int block_width  = SCREEN_HEIGHT / MAZE_COL;

    float rayX = plyr->x + 5.0f;
    float rayY = plyr->y + 5.0f;

    float dirX = cosf(plyr->angle);
    float dirY = sinf(plyr->angle);

    while (1) {

        // Move the ray forward
        rayX += dirX;
        rayY += dirY;

        // Convert screen coordinates to maze coordinates
        int col = (int)(rayX / block_width);
        int row = (int)(rayY / block_height);

        // Have we hit a wall?
        if (Is_wall(row, col)) {
            break;
        }
    }

    SDL_SetRenderDrawColor(
        app->rndr,
        0,
        0,
        255,
        255
    );

    SDL_RenderDrawLine(
        app->rndr,
        plyr->x + 5,
        plyr->y + 5,
        (int)rayX,
        (int)rayY
    );
}

void Moveplayer(PlayerPos *plyr, SDL_Event *ev) {
  if (ev->type == SDL_KEYDOWN) {
    switch (ev->key.keysym.sym) {

    case SDLK_w:
      plyr->y = plyr->y - 5;
      break;
    case SDLK_s:
      plyr->y = plyr->y + 5;
      break;
    case SDLK_a:
      plyr->x = plyr->x - 5;
      break;
    case SDLK_d:
      plyr->x = plyr->x + 5;
      break;
    case SDLK_LEFT:
      plyr->angle = plyr->angle - 0.5*(PI/180);
      break;
    case SDLK_RIGHT:
      plyr->angle = plyr->angle + 0.5*(PI/180);
      break;
    default:
      break;
    }
  }
}
int main() {

  App app;

  PlayerPos plyr = {80, 80, 0.0};
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

  SDL_SetRenderDrawColor(app.rndr, 255, 255, 255, 255);

  SDL_RenderClear(app.rndr);

  RenderBlock(&app);
  RenderPlayer(&app, &plyr);

  RenderLine(&app, &plyr);
  SDL_RenderPresent(app.rndr);

  SDL_Event e;
  bool quit = false;
  while (quit == false) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        printf("quited the window");
        quit = true;
      } else {
        Moveplayer(&plyr, &e);
      }
    }

    SDL_SetRenderDrawColor(app.rndr, 255, 255, 255, 255);
    SDL_RenderClear(app.rndr);

    RenderBlock(&app);
    RenderPlayer(&app, &plyr);
    RenderLine(&app, &plyr);
    SDL_RenderPresent(app.rndr);
  }

  SDL_DestroyRenderer(app.rndr);
  SDL_DestroyWindow(app.wndw);
  SDL_Quit();
  return 0;
}
