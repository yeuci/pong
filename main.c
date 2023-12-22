#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#define WIDTH 640
#define HEIGHT 480
#define BALL_SIZE 10


typedef struct Ball {
  float x;
  float y;
  float x_speed;
  float y_speed;
  int size;
} Ball;

Ball ball;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

bool coin_flip(void) {
  return rand() % 2 == 1 ? true : false;
}

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

Ball create_ball(int size) {
  const float SPEED = 120;
  Ball ball = {
    .x = (WIDTH / 2) - (size / 2),
    .y = (HEIGHT / 2) - (size / 2),
    .size = size,
    .x_speed = SPEED * (coin_flip() ? 1 : -1),
    .y_speed = SPEED * (coin_flip() ? 1 : -1)
  };

  return ball;
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

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    fprintf(stderr, "failed to create renderer: %s\n", SDL_GetError());
    return false;
  }

  ball = create_ball(BALL_SIZE);

  return true;
}

void update_ball(Ball* ball, float elapsed) {
  ball->x += ball->x_speed * elapsed;
  ball->y += ball->y_speed * elapsed;

  if (ball->x < 0) {
    ball->x_speed = fabs(ball->x_speed);
  }
  if (ball->x > WIDTH - BALL_SIZE) {
    ball->x_speed = -(fabs(ball->x_speed));
  }
  if (ball->y < 0) {
    ball->y_speed = fabs(ball->y_speed);
  }
  if (ball->y > HEIGHT - BALL_SIZE) {
    ball->y_speed = -(fabs(ball->y_speed));
  }
}

void render_ball(const Ball* ball) {
  int size = ball->size;
  int half_size = size / 2;
  SDL_Rect ball_rect = {
    .x = ball->x - half_size,
    .y = ball->y - half_size,
    .w = size,
    .h = size
  };
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(renderer, &ball_rect);
}


void update(float elapsed) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  update_ball(&ball, elapsed);
  render_ball(&ball);

  SDL_RenderPresent(renderer);
}

int main(int argc, const char** argv) {
  srand((unsigned int)time(NULL));
  atexit(cleanup);

  bool is_running = true;

  if (!initialize())
    exit(1);

  SDL_Event event;

  uint32_t last_tick = SDL_GetTicks();

  while (is_running) {
    recieve_input(&event, &is_running);

    uint32_t current_tick = SDL_GetTicks();
    uint32_t diff = current_tick - last_tick;
    float elapsed = diff / 1000.0f;

    update(elapsed);
    last_tick = current_tick;
  }

  cleanup();

  return 0;
}
