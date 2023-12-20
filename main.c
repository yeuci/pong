#include <SDL2/SDL.h>

int main(int argc, const char** argv) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "failed to initialize SDL: %s\n", SDL_GetError());
    exit(1);
  }

  SDL_Quit();
  return 0;
}
