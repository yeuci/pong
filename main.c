#include <SDL2/SDL.h>
#include <stdbool.h>

#define WIDTH 640
#define HEIGHT 480
#define BALL_SIZE 10

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

void recieve_input(SDL_Event* event, bool* is_running) {
    SDL_PollEvent(event);
    switch ((*event).type) {
      case SDL_QUIT:
        *is_running = false;
        break;
      case SDL_KEYDOWN:
        if ((*event).key.keysym.sym == SDLK_ESCAPE)
          *is_running = false;
        break;
      default:
        break;
    }
}

void cleanup(void) {
  if (window) SDL_DestroyWindow(window);
  if (renderer) SDL_DestroyRenderer(renderer);
  SDL_Quit();
}

bool initialize() {

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "failed to initialize SDL: %s\n", SDL_GetError());
    return false;
  }

  window = SDL_CreateWindow(
    "Pong",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    WIDTH,
    HEIGHT,
    SDL_WINDOW_SHOWN
  );

  if (!window) {
    fprintf(stderr, "failed to create window: %s\n", SDL_GetError());
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    fprintf(stderr, "failed to create renderer: %s\n", SDL_GetError());
    return false;
  }

  return true;
}

void update(float elapsed) {

}

int main(int argc, const char** argv) {
  atexit(cleanup);

  bool is_running = true;

  if (!initialize())
    exit(1);

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

  cleanup();

  return 0;
}
