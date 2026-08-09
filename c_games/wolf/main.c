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

  SDL_SetRenderDrawColor(app->rndr, 0, 255, 0, 255);
  SDL_RenderDrawLine(app->rndr, startptX, startptY, endptX, endptY);
}

void RenderLine(App *app, PlayerPos *plyr) {

  int block_height = SCREEN_HEIGHT / MAZE_ROW;
  int block_width = SCREEN_HEIGHT / MAZE_COL;

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

  SDL_SetRenderDrawColor(app->rndr, 0, 0, 255, 255);

  SDL_RenderDrawLine(app->rndr, plyr->x + 5, plyr->y + 5, (int)rayX, (int)rayY);
}
void RenderLineImpr(App *app, PlayerPos *plyr) {

    const float blockWidth  = (float)SCREEN_WIDTH / MAZE_COL;
    const float blockHeight = (float)SCREEN_HEIGHT / MAZE_ROW;

    // Player center
    const float px = (float)plyr->x + 5.0f;
    const float py = (float)plyr->y + 5.0f;

    // Direction of the ray
    const float rayDirX = cosf(plyr->angle);
    const float rayDirY = sinf(plyr->angle);

    /*
     * ---------------------------------------------------------
     * HORIZONTAL INTERSECTIONS
     * ---------------------------------------------------------
     */

    float horizontalHitX = 0.0f;
    float horizontalHitY = 0.0f;
    float horizontalDistance = INFINITY;

    // Don't calculate horizontal intersections when ray is
    // almost perfectly horizontal.
    if (fabsf(rayDirY) > 0.0001f) {

        float firstHorizontalY;

        if (rayDirY > 0) {
            // Ray is pointing DOWN

            firstHorizontalY =
                floorf(py / blockHeight) * blockHeight
                + blockHeight;
        }
        else {
            // Ray is pointing UP

            firstHorizontalY =
                floorf(py / blockHeight) * blockHeight;
        }

        // How far along the ray until we reach that Y?
        float distance =
            (firstHorizontalY - py) / rayDirY;

        // X position at that distance
        float firstHorizontalX =
            px + distance * rayDirX;

        float stepY;

        if (rayDirY > 0) {
            stepY = blockHeight;
        }
        else {
            stepY = -blockHeight;
        }

        // Every next horizontal grid intersection
        float currentX = firstHorizontalX;
        float currentY = firstHorizontalY;

        while (1) {

            // Convert intersection into maze coordinates
            int col = (int)(currentX / blockWidth);

            int row;

            if (rayDirY > 0) {
                row = (int)(currentY / blockHeight);
            }
            else {
                row = (int)(currentY / blockHeight) - 1;
            }

            // Did we leave the maze?
            if (col < 0 || col >= MAZE_COL ||
                row < 0 || row >= MAZE_ROW) {
                break;
            }

            // Is this cell a wall?
            if (Is_wall(row, col)) {

                horizontalHitX = currentX;
                horizontalHitY = currentY;

                horizontalDistance =
                    sqrtf(
                        (currentX - px) * (currentX - px) +
                        (currentY - py) * (currentY - py)
                    );

                break;
            }

            currentX += rayDirX * (stepY / rayDirY);
            currentY += stepY;
        }
    }


    /*
     * ---------------------------------------------------------
     * VERTICAL INTERSECTIONS
     * ---------------------------------------------------------
     */

    float verticalHitX = 0.0f;
    float verticalHitY = 0.0f;
    float verticalDistance = INFINITY;

    // Don't calculate vertical intersections when ray is
    // almost perfectly vertical.
    if (fabsf(rayDirX) > 0.0001f) {

        float firstVerticalX;

        if (rayDirX > 0) {
            // Ray is pointing RIGHT

            firstVerticalX =
                floorf(px / blockWidth) * blockWidth
                + blockWidth;
        }
        else {
            // Ray is pointing LEFT

            firstVerticalX =
                floorf(px / blockWidth) * blockWidth;
        }

        // How far along the ray until we reach that X?
        float distance =
            (firstVerticalX - px) / rayDirX;

        // Y position at that distance
        float firstVerticalY =
            py + distance * rayDirY;

        float stepX;

        if (rayDirX > 0) {
            stepX = blockWidth;
        }
        else {
            stepX = -blockWidth;
        }

        float currentX = firstVerticalX;
        float currentY = firstVerticalY;

        while (1) {

            int row = (int)(currentY / blockHeight);

            int col;

            if (rayDirX > 0) {
                col = (int)(currentX / blockWidth);
            }
            else {
                col = (int)(currentX / blockWidth) - 1;
            }

            // Did we leave the maze?
            if (row < 0 || row >= MAZE_ROW ||
                col < 0 || col >= MAZE_COL) {
                break;
            }

            // Is this cell a wall?
            if (Is_wall(row, col)) {

                verticalHitX = currentX;
                verticalHitY = currentY;

                verticalDistance =
                    sqrtf(
                        (currentX - px) * (currentX - px) +
                        (currentY - py) * (currentY - py)
                    );

                break;
            }

            currentX += stepX;
            currentY += rayDirY * (stepX / rayDirX);
        }
    }


    /*
     * ---------------------------------------------------------
     * CHOOSE THE CLOSEST HIT
     * ---------------------------------------------------------
     */

    float hitX;
    float hitY;

    if (horizontalDistance < verticalDistance) {

        // Horizontal wall is closer
        hitX = horizontalHitX;
        hitY = horizontalHitY;

    }
    else {

        // Vertical wall is closer
        hitX = verticalHitX;
        hitY = verticalHitY;
    }


    /*
     * ---------------------------------------------------------
     * DRAW RAY
     * ---------------------------------------------------------
     */

    SDL_SetRenderDrawColor(
        app->rndr,
        0,
        0,
        255,
        255
    );

    SDL_RenderDrawLine(
        app->rndr,
        (int)px,
        (int)py,
        (int)hitX,
        (int)hitY
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
      plyr->angle = plyr->angle - 0.5 * (PI / 180);
      break;
    case SDLK_RIGHT:
      plyr->angle = plyr->angle + 0.5 * (PI / 180);
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
    SDL_RenderPresent(app.rndr);
  }

  SDL_DestroyRenderer(app.rndr);
  SDL_DestroyWindow(app.wndw);
  SDL_Quit();
  return 0;
}
