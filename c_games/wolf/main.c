#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <time.h>

#define SCREEN_HEIGHT 720
#define SCREEN_WIDTH 1280
#define MAZE_COL 20
#define MAZE_ROW 20
#define PI 3.14159265359
#define FOV_SCALE 0.66f
#define VIEW_BLOCK (SCREEN_WIDTH / 2 / MAZE_COL)
#define RAY_SPACING 8
#define MOVE_SPEED 0.05f
#define ROT_SPEED 0.04f
#define FOG_DIST 12.0f
typedef struct {

  SDL_Renderer *rndr;
  SDL_Window *wndw;
} App;

typedef struct {
  float x;
  float y;
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
  return maze[row][col] != 0;
}

bool CanMov(PlayerPos *plyr, float new_x, float new_y) {
  // FIX: was 0.5f -> the collision box was a full 1x1 tile, so it only
  // fit through 1-wide gaps (like "1 0 1") when PERFECTLY centered.
  // 0.25f makes the box half a tile wide -> you can walk through gaps.
  const float half = 0.25f;
  int c0 = (int)floorf(new_x - half);
  int c1 = (int)floorf(new_x + half);
  int r0 = (int)floorf(new_y - half);
  int r1 = (int)floorf(new_y + half);

  return !(Is_wall(r0, c0) || Is_wall(r0, c1) || Is_wall(r1, c0) ||
           Is_wall(r1, c1));
}

float Cast_ray(PlayerPos *plyr, float dir_x, float dir_y, float *hit_x,
               float *hit_y, bool *hit_side_x, int *hit_tile) {
  float pos_x = plyr->x;
  float pos_y = plyr->y;

  int map_x = (int)floorf(pos_x);
  int map_y = (int)floorf(pos_y);

  // if dist_x is very small val then 1e30f is very big value 1 X 10^3 else
  // dir_x = 0.1       → 10
  // dir_x = 0.01      → 100
  // dir_x = 0.001     → 1000
  // dir_x = 0.0001    → 10000
  // dir_x = 0.00001   → 100000
  // dir_x = 0.000001  → 1000000
  float d_dist_x = (fabsf(dir_x) < 0.0001f) ? 1e30f : fabsf(1.0f / dir_x);
  float d_dist_y = (fabsf(dir_y) < 0.0001f) ? 1e30f : fabsf(1.0f / dir_y);

  int step_x, step_y;
  float side_dist_x, side_dist_y;
  if (dir_x < 0.0f) {
    step_x = -1;
    side_dist_x = (pos_x - map_x) * d_dist_x;
  } else {
    step_x = 1;
    side_dist_x = (map_x + 1.0f - pos_x) * d_dist_x;
  }
  if (dir_y < 0.0f) {
    step_y = -1;
    side_dist_y = (pos_y - map_y) * d_dist_y;
  } else {
    step_y = 1;
    side_dist_y = (map_y + 1.0f - pos_y) * d_dist_y;
  }

  int side = 0;
  int guard = 0;
  while (guard++ < 100) {
    if (side_dist_x < side_dist_y) {
      side_dist_x += d_dist_x;
      map_x += step_x;
      side = 0;
    } else {
      side_dist_y += d_dist_y;
      map_y += step_y;
      side = 1;
    }

    if (Is_wall(map_y, map_x))
      break;
  }

  float perp = (side == 0) ? side_dist_x - d_dist_x : side_dist_y - d_dist_y;

  if (hit_x && hit_y) {
    if (side == 0) {
      *hit_x = (step_x > 0) ? (float)map_x : (float)(map_x + 1);
      *hit_y = pos_y + perp * dir_y;
    } else {

      *hit_y = (step_y > 0) ? (float)map_y : (float)(map_y + 1);
      *hit_x = pos_x + perp * dir_x;
    }
  }

  if (hit_side_x)
    *hit_side_x = (side == 0);
  if (hit_tile)
    *hit_tile = maze[map_y][map_x];
  return perp;
}
void RenderBlock(App *app) {
  // FIX: use VIEW_BLOCK (32 px), the SAME size the rays are scaled by.
  // before it was SCREEN_HEIGHT / MAZE_* = 36 px, so the ray lines
  // (drawn at 32 px per tile) never lined up with these squares.
  int block_height = VIEW_BLOCK;
  int block_width = VIEW_BLOCK;

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
  // FIX: plyr->x/y are in TILE units (2.5 = middle of tile 2), but SDL
  // wants PIXELS. Multiply by VIEW_BLOCK to convert, like the rays do.
  // before, the raw floats were used as pixels, so the player was
  // always drawn tiny in the top-left corner of the window.
  int cx = (int)(plyr->x * VIEW_BLOCK);
  int cy = (int)(plyr->y * VIEW_BLOCK);

  // center the 10 px square ON the player's position
  SDL_Rect plybox = {cx - 5, cy - 5, 10, 10};
  SDL_SetRenderDrawColor(app->rndr, 0, 255, 0, 255);
  SDL_RenderFillRect(app->rndr, &plybox);

  // a short line from the center showing where the player looks
  int endptX = cx + (int)(cos(plyr->angle) * 15);
  int endptY = cy + (int)(sin(plyr->angle) * 15);

  SDL_SetRenderDrawColor(app->rndr, 0, 255, 0, 255);
  SDL_RenderDrawLine(app->rndr, cx, cy, endptX, endptY);
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
  float hit_x, hit_y;
  Cast_ray(plyr, dir_x, dir_y, &hit_x, &hit_y, NULL, NULL);

  SDL_SetRenderDrawColor(app->rndr, 0, 156, 255, 255);
  SDL_RenderDrawLine(app->rndr, (int)(plyr->x * VIEW_BLOCK),
                     (int)(plyr->y * VIEW_BLOCK), (int)(hit_x * VIEW_BLOCK),
                     (int)(hit_y * VIEW_BLOCK));
}

void EmitRays(App *app, PlayerPos *plyr) {

  const float dir_x = cosf(plyr->angle);
  const float dir_y = sinf(plyr->angle);

  // camera plane: a line perpendicular to the facing direction
  const float plane_x = -dir_y * FOV_SCALE;
  const float plane_y = dir_x * FOV_SCALE;

  // one ray every `ray_spacing` screen columns
  for (int x = 0; x < SCREEN_WIDTH; x += RAY_SPACING) {
    // -1 on the left edge, +1 on the right edge, 0 in the middle
    float camera_x = 2.0f * x / (float)SCREEN_WIDTH - 1.0f;

    RenderLineDDA(app, plyr, dir_x + plane_x * camera_x,
                  dir_y + plane_y * camera_x);
  }
}

void TileColor(int tile, Uint8 *r, Uint8 *g, Uint8 *b) {
  switch (tile) {

  case 1:
    *r = 169, *g = 169, *b = 169;
    break;

  case 2:
    *r = 255, *g = 0, *b = 0;
    break;
  case 3:
    *r = 0, *g = 0, *b = 255;
    break;
  case 4:
    *r = 0, *g = 255, *b = 0;
    break;
  case 5:
    *r = 255, *g = 0, *b = 255;
    break;
  default:
    *r = 0, *g = 0, *b = 0;
    break;
  }
}

void Render3D(App *app, PlayerPos *plyr) {
  const int halfW = SCREEN_WIDTH / 2;
  const float dir_x = cosf(plyr->angle);
  const float dir_y = sinf(plyr->angle);

  const float plane_x = -dir_y * FOV_SCALE;
  const float plane_y = dir_x * FOV_SCALE;

  SDL_Rect right_rect = {halfW, 0, halfW, SCREEN_HEIGHT};
  SDL_SetRenderDrawColor(app->rndr, 0, 0, 0, 255);
  SDL_RenderFillRect(app->rndr, &right_rect);

  for (int x = 0; x < halfW; x++) {
    float camera_x = 2.0f * x / (float)halfW - 1.0f;
    float ray_x = dir_x + plane_x * camera_x;
    float ray_y = dir_y + plane_y * camera_x;

    int hit_tile;
    float perp = Cast_ray(plyr, ray_x, ray_y, NULL, NULL, NULL, &hit_tile);

    Uint8 base_r, base_g, base_b;
TileColor(hit_tile, &base_r, &base_g, &base_b);

    // FIX: perp can be ~0 when a wall is right next to us, and
    // dividing by 0 gives an invalid huge number. clamp it to tiny.
    if (perp < 0.0001f)
      perp = 0.0001f;

    int line_height = (int)(SCREEN_HEIGHT/ perp);
    int draw_start = -line_height/2+ SCREEN_HEIGHT/2;
    if(draw_start < 0) draw_start = 0;
    int draw_end = line_height / 2 + SCREEN_HEIGHT / 2;
    if( draw_end >= SCREEN_HEIGHT) draw_end = SCREEN_HEIGHT-1;

    float t = perp/ FOG_DIST;
    if(t > 1.0f) t = 1.0f;
    float shade = 1.0f -t;
    

    SDL_SetRenderDrawColor(app->rndr, base_r*shade, base_g*shade, base_b*shade, 255);
    SDL_RenderDrawLine(app->rndr,halfW+x, draw_start, halfW+x, draw_end);
  }
}

void Moveplayer(PlayerPos *plyr, SDL_Event *ev) {

  float dx = 0.0f, dy = 0.0f;
  float dir_x = cosf(plyr->angle);
  float dir_y = sinf(plyr->angle);
  if (ev->type == SDL_KEYDOWN) {
    switch (ev->key.keysym.sym) {

    case SDLK_w:
        dx = dir_x;
        dy = dir_y;
      break;
    case SDLK_s:
        dx = -dir_x;
        dy = -dir_y;
      break;
    case SDLK_a:
        dx = dir_y;
        dy = -dir_x;
      break;
    case SDLK_d:
         dx = -dir_y;
        dy = dir_x;
      break;
    case SDLK_LEFT:
      plyr->angle -= ROT_SPEED; 
      return;
    case SDLK_RIGHT:
      plyr->angle += ROT_SPEED;
      return;
    default:
      break;
    }
  }

  dx *= MOVE_SPEED;
  dy *= MOVE_SPEED;

  // test: "if I take this SMALL step, will I touch a wall?"
  if(CanMov(plyr, plyr->x+dx, plyr->y+dy)){
    // FIX: move by the SAME step we just tested (dx/dy).
    // before it did "+= dir_x / dir_y", which:
    //   1) moved a full 1.0 tile while only 0.5 was tested -> wall clipping
    //   2) made 'a'/'d' (strafe) move FORWARD, since dx/dy were ignored
    plyr->x += dx;
    plyr->y += dy;
  }
}
int main() {

  App app;

  PlayerPos plyr = {2.5f, 2.5f, 0.0f};
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
    EmitRays(&app, &plyr);
    RenderPlayer(&app, &plyr);
    // FIX: removed the 2nd EmitRays() - it drew every ray twice per frame
    Render3D(&app, &plyr);
    SDL_RenderPresent(app.rndr);
    
  }

  SDL_DestroyRenderer(app.rndr);
  SDL_DestroyWindow(app.wndw);
  SDL_Quit();
  return 0;
}
