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
#include <threads.h>

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

void RenderLineDDA(App *app, PlayerPos *plyr, float dir_x, float dir_y) {
  const float block_size = (float)SCREEN_HEIGHT / MAZE_ROW;

  const float start_x = plyr->x + 5.0f;
  const float start_y = plyr->y + 5.0f;

  // ---------- vertical wall scan ----------

  float ver_wall_x = start_x;
  float ver_wall_y = start_y;
  float ver_wall_distance = INFINITY;

  if (fabsf(dir_x) > 0.0001f) {

    // first vertical grid line the ray meets
    float current_grid_x = floorf(start_x / block_size) * block_size;
    if (dir_x > 0) current_grid_x += block_size;

    float dist_to_first_grid = (current_grid_x - start_x) / dir_x;
    float current_grid_y = start_y + dist_to_first_grid * dir_y;

    // how many pixels y moves each time x crosses one grid line
    float ver_x_step = dir_x > 0 ? block_size : -block_size;
    float ver_y_step = ver_x_step * dir_y / dir_x;

    while (1) {
      // x position -> maze column, y position -> maze row
      int maze_col = (int)(current_grid_x / block_size);
      int maze_row = (int)(current_grid_y / block_size);
      if (dir_x < 0) maze_col--; // going left: check the cell before the line

      if (Is_wall(maze_row, maze_col)) {
        ver_wall_x = current_grid_x;
        ver_wall_y = current_grid_y;
        ver_wall_distance =
            sqrtf((ver_wall_x - start_x) * (ver_wall_x - start_x) +
                  (ver_wall_y - start_y) * (ver_wall_y - start_y));
        break;
      }

      current_grid_x += ver_x_step;
      current_grid_y += ver_y_step;
    }
  }

  // ---------- horizontal wall scan ----------

  float hor_wall_x = start_x;
  float hor_wall_y = start_y;
  float hor_wall_distance = INFINITY;

  if (fabsf(dir_y) > 0.0001f) {

    // first horizontal grid line the ray meets
    float current_grid_y = floorf(start_y / block_size) * block_size;
    if (dir_y > 0) current_grid_y += block_size;

    float dist_to_first_grid = (current_grid_y - start_y) / dir_y;
    float current_grid_x = start_x + dist_to_first_grid * dir_x;

    // how many pixels x moves each time y crosses one grid line
    float hor_y_step = dir_y > 0 ? block_size : -block_size;
    float hor_x_step = hor_y_step * dir_x / dir_y;

    while (1) {
      int maze_col = (int)(current_grid_x / block_size);
      int maze_row = (int)(current_grid_y / block_size);
      if (dir_y < 0) maze_row--; // going up: check the cell before the line

      if (Is_wall(maze_row, maze_col)) {
        hor_wall_x = current_grid_x;
        hor_wall_y = current_grid_y;
        hor_wall_distance =
            sqrtf((hor_wall_x - start_x) * (hor_wall_x - start_x) +
                  (hor_wall_y - start_y) * (hor_wall_y - start_y));
        break;
      }

      current_grid_y += hor_y_step;
      current_grid_x += hor_x_step;
    }
  }

  // ---------- pick the nearest wall ----------

  float nearestwallX, nearestwallY;
  if (hor_wall_distance < ver_wall_distance) {
    nearestwallX = hor_wall_x;
    nearestwallY = hor_wall_y;
  } else {
    nearestwallX = ver_wall_x;
    nearestwallY = ver_wall_y;
  }

  SDL_SetRenderDrawColor(app->rndr, 0, 156, 255, 255);
  SDL_RenderDrawLine(app->rndr, start_x, start_y, nearestwallX, nearestwallY);
}

void EmitRays(App *app, PlayerPos *plyr) {
  const float fov_scale = 0.66f;
  const int ray_spacing = 12;

  const float dir_x = cosf(plyr->angle);
  const float dir_y = sinf(plyr->angle);

  // camera plane: a line perpendicular to the facing direction
  const float plane_x = -dir_y * fov_scale;
  const float plane_y = dir_x * fov_scale;

  // one ray every `ray_spacing` screen columns
  for (int x = 0; x < SCREEN_WIDTH; x += ray_spacing) {
    // -1 on the left edge, +1 on the right edge, 0 in the middle
    float camera_x = 2.0f * x / (float)SCREEN_WIDTH - 1.0f;

    RenderLineDDA(app, plyr,
                  dir_x + plane_x * camera_x,
                  dir_y + plane_y * camera_x);
  }
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

  EmitRays(&app, &plyr);
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
    EmitRays(&app, &plyr);
    SDL_RenderPresent(app.rndr);
  }

  SDL_DestroyRenderer(app.rndr);
  SDL_DestroyWindow(app.wndw);
  SDL_Quit();
  return 0;
}
