#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#define WIDTH 640
#define HEIGHT 480
#define BALL_SIZE 10
#define PLAYER_WIDTH 20
#define PLAYER_HEIGHT 75
#define PLAYER_MARGIN 10
#define PLAYER_MOVE_SPEED 150.0f

typedef struct Ball {
  float x;
  float y;
  float x_speed;
  float y_speed;
  int size;
} Ball;

typedef struct Player {
  int score;
  float y_position;
} Player;

Ball ball;

Player player_1;
Player player_2;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

bool in_play = false;

bool coin_flip(void) {
  return rand() % 2 == 1 ? true : false;
}

void recieve_input(SDL_Event* event, bool* is_running) {
    while (SDL_PollEvent(event)) {
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
}

void cleanup(void) {
  if (window) SDL_DestroyWindow(window);
  if (renderer) SDL_DestroyRenderer(renderer);
  SDL_Quit();
}

Player create_player(void) {
  Player player = {
    .y_position = HEIGHT / 2,
  };

  return player;
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
  player_1 = create_player();
  player_2 = create_player();

  return true;
}


void update_players(float elapsed) {
  const uint8_t* keyboard_state = SDL_GetKeyboardState(NULL);

  if (keyboard_state[SDL_SCANCODE_SPACE]) {
    in_play = true;
  }

  if (keyboard_state[SDL_SCANCODE_W]) {
    player_1.y_position -= PLAYER_MOVE_SPEED * elapsed;
  }
  if (keyboard_state[SDL_SCANCODE_S]) {
    player_1.y_position += PLAYER_MOVE_SPEED * elapsed;
  }

  if (keyboard_state[SDL_SCANCODE_UP]) {
    player_2.y_position -= PLAYER_MOVE_SPEED * elapsed;
  }
  if (keyboard_state[SDL_SCANCODE_DOWN]) {
    player_2.y_position += PLAYER_MOVE_SPEED * elapsed;
  }

  if (player_1.y_position < PLAYER_HEIGHT / 2) {
    player_1.y_position = PLAYER_HEIGHT / 2;
  }

  if (player_1.y_position > HEIGHT - PLAYER_HEIGHT / 2) {
    player_1.y_position = HEIGHT - PLAYER_HEIGHT / 2;
  }

  if (player_2.y_position < PLAYER_HEIGHT / 2) {
    player_2.y_position = PLAYER_HEIGHT / 2;
  }

  if (player_2.y_position > HEIGHT - PLAYER_HEIGHT / 2) {
    player_2.y_position = HEIGHT - PLAYER_HEIGHT / 2;
  }

  SDL_Rect ball_rect = {
    .x = ball.x - (ball.size / 2),
    .y = ball.y - (ball.size / 2),
    .w = ball.size,
    .h = ball.size,
  };

  SDL_Rect player_1_rect = {
    .x = PLAYER_MARGIN,
    .y = (int)(player_1.y_position) - (PLAYER_HEIGHT / 2),
    .w = PLAYER_WIDTH,
    .h = PLAYER_HEIGHT,
  };

  if (SDL_HasIntersection(&ball_rect, &player_1_rect)) {
    ball.x_speed = fabs(ball.x_speed);
  }

  SDL_Rect player_2_rect = {
    .x = WIDTH - PLAYER_WIDTH - PLAYER_MARGIN,
    .y = (int)(player_2.y_position) - (PLAYER_HEIGHT / 2),
    .w = PLAYER_WIDTH,
    .h = PLAYER_HEIGHT,
  };

  if (SDL_HasIntersection(&ball_rect, &player_2_rect)) {
    ball.x_speed = -(fabs(ball.x_speed));
  }

}

void render_players(void) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_Rect player_1_rect = {
    .x = PLAYER_MARGIN,
    .y = (int)(player_1.y_position) - (PLAYER_HEIGHT / 2),
    .w = PLAYER_WIDTH,
    .h = PLAYER_HEIGHT,
  };
  SDL_RenderFillRect(renderer, &player_1_rect);

  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  SDL_Rect player_2_rect = {
    .x = WIDTH - PLAYER_WIDTH - PLAYER_MARGIN,
    .y = (int)(player_2.y_position) - (PLAYER_HEIGHT / 2),
    .w = PLAYER_WIDTH,
    .h = PLAYER_HEIGHT,
  };
  SDL_RenderFillRect(renderer, &player_2_rect);
}

void update_ball(Ball* ball, float elapsed) {
  if (!in_play) {
    return;
  }

  ball->x += ball->x_speed * elapsed;
  ball->y += ball->y_speed * elapsed;

  if (ball->x < BALL_SIZE / 2) {
    ball->x_speed = fabs(ball->x_speed);
  }
  if (ball->x > WIDTH - BALL_SIZE / 2) {
    ball->x_speed = -(fabs(ball->x_speed));
  }
  if (ball->y < BALL_SIZE / 2) {
    ball->y_speed = fabs(ball->y_speed);
  }
  if (ball->y > HEIGHT - (BALL_SIZE / 2)) {
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

  update_players(elapsed);
  render_players();

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
