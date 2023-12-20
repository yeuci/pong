#include <SDL2/SDL.h>
#include <stdbool.h>

#define WIDTH 640
#define HEIGHT 480
#define BALL_SIZE 10

void recieve_input(SDL_Event* event, bool* is_running) {
    SDL_PollEvent(event);
    switch ((*event).type) {
      case SDL_QUIT:
        *is_running = false;
        break;
      case SDL_KEYDOWN:
        if ((*event).key.keysym.sym == SDLK_ESCAPE) {
          *is_running = false;
        }
        break;
      default:
        break;
    }
}

void cleanup(SDL_Window* window) {
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int main(int argc, const char** argv) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "failed to initialize SDL: %s\n", SDL_GetError());
    exit(1);
  }

  bool is_running = true;

  SDL_Window *window = SDL_CreateWindow(
    "Pong",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    WIDTH,
    HEIGHT,
    SDL_WINDOW_SHOWN
  );

  if (!window) {
    fprintf(stderr, "failed to create window: %s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_Rect ball_rect = {
    .x = (WIDTH / 2) - (BALL_SIZE / 2),
    .y = (HEIGHT / 2) - (BALL_SIZE / 2),
    .w = BALL_SIZE,
    .h = BALL_SIZE,
  };
  SDL_RenderFillRect(renderer, &ball_rect);

  SDL_RenderPresent(renderer);

  SDL_Event event;

  while (is_running) {
    recieve_input(&event, &is_running);
  }

  cleanup(window);

  return 0;
}
