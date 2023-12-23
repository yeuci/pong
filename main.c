#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#define WIDTH 640
#define HEIGHT 480
#define BALL_SIZE 10
#define PLAYER_WIDTH 20
#define PLAYER_HEIGHT 75
#define PLAYER_MARGIN 10
#define PLAYER_MOVE_SPEED 200.0f

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

SDL_Surface *text_surface = NULL;
SDL_Texture *text_texture = NULL;
SDL_Surface *players_text_surface = NULL;
SDL_Texture *players_text_texture = NULL;
TTF_Font *font = NULL;

bool in_play = false;

unsigned int player_1_score = 0;
unsigned int player_2_score = 0;

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

  TTF_Init();

  font = TTF_OpenFont("./assets/PressStart2P-Regular.ttf", 25);
  if (font == NULL) {
    printf("font loading failed: %s\n", TTF_GetError());
    return false;
  }

  text_surface = TTF_RenderText_Blended_Wrapped(
    font,
    "Pong",
    (SDL_Color){255, 255, 255, 255},
    500
  );
  players_text_surface = TTF_RenderText_Blended_Wrapped(
    font,
    "0 - 0",
    (SDL_Color){255, 255, 255, 255},
    500
  );

  if (text_surface == NULL || players_text_surface == NULL) {
    printf("surface creation failed: %s\n", TTF_GetError());
    return false;
  }

  text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
  players_text_texture = SDL_CreateTextureFromSurface(renderer, players_text_surface);
  if (text_surface == NULL || players_text_surface == NULL) {
    printf("texture creation failed: %s\n", TTF_GetError());
    return false;
  }

  ball = create_ball(BALL_SIZE);
  player_1 = create_player();
  player_2 = create_player();

  return true;
}


void update_text() {
  char buf[50];
  snprintf(buf, sizeof(buf), "%d - %d", player_1_score, player_2_score);

  SDL_Surface *u_players_text_surface = TTF_RenderText_Blended_Wrapped(
    font,
    buf,
    (SDL_Color){255, 255, 255, 255},
    500
  );
  SDL_Texture *u_players_text_texture = SDL_CreateTextureFromSurface(renderer, u_players_text_surface);

  SDL_Surface *last_players_text_surface = players_text_surface;
  SDL_Texture *last_players_text_texture = players_text_texture;

  players_text_surface = u_players_text_surface;
  players_text_texture = u_players_text_texture;

  //free(last_players_text_surface);
  //free(last_players_text_texture);
  SDL_FreeSurface(last_players_text_surface);
  SDL_DestroyTexture(last_players_text_texture);
}

void update_ai(float elapsed) {
  if (!in_play) return;
  if (ball.y > player_2.y_position)
      player_2.y_position += PLAYER_MOVE_SPEED * elapsed;
  if (ball.y < player_2.y_position)
      player_2.y_position -= PLAYER_MOVE_SPEED * elapsed;
}

void update_players(float elapsed) {
  const uint8_t* keyboard_state = SDL_GetKeyboardState(NULL);

  if (keyboard_state[SDL_SCANCODE_SPACE]) {
    in_play = true;
  }

  if (in_play) {
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
      ball.x_speed += 60;
    }

    SDL_Rect player_2_rect = {
      .x = WIDTH - PLAYER_WIDTH - PLAYER_MARGIN,
      .y = (int)(player_2.y_position) - (PLAYER_HEIGHT / 2),
      .w = PLAYER_WIDTH,
      .h = PLAYER_HEIGHT,
    };

    if (SDL_HasIntersection(&ball_rect, &player_2_rect)) {
      ball.x_speed = -(fabs(ball.x_speed));
      ball.x_speed -= 60;
    }
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

void reset_game(int size) {
  const float SPEED = 120;

  ball.x = (WIDTH / 2) - (size / 2);
  ball.y = (HEIGHT / 2) - (size / 2);
  ball.size = size;
  ball.x_speed = SPEED * (coin_flip() ? 1 : -1);
  ball.y_speed = SPEED * (coin_flip() ? 1 : -1);

  player_1.y_position = HEIGHT / 2;
  player_2.y_position = HEIGHT / 2;

  in_play = false;
}

void update_ball(Ball* ball, float elapsed) {
  if (!in_play) {
    return;
  }

  ball->x += ball->x_speed * elapsed;
  ball->y += ball->y_speed * elapsed;

  if (ball->x < BALL_SIZE / 2) {
    ball->x_speed = fabs(ball->x_speed);
    reset_game(BALL_SIZE);
    player_2_score++;
    update_text();
  }
  if (ball->x > WIDTH - BALL_SIZE / 2) {
    ball->x_speed = -(fabs(ball->x_speed));
    reset_game(BALL_SIZE);
    player_1_score++;
    update_text();
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


void render_text() {
  SDL_RenderCopy(renderer,
    text_texture,
    NULL,
   &(SDL_Rect){
       (WIDTH / 2) - ((text_surface->w) / 2),
       PLAYER_MARGIN,
       text_surface->w,
       text_surface->h
   });

  SDL_RenderCopy(renderer,
    players_text_texture,
    NULL,
   &(SDL_Rect){
       (WIDTH / 2) - ((text_surface->w) / 2),
       (PLAYER_MARGIN * 2) + text_surface->h,
       text_surface->w,
       text_surface->h
   });

}

void update(float elapsed) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  render_text();

  update_ball(&ball, elapsed);
  render_ball(&ball);

  update_players(elapsed);
  render_players();

  update_ai(elapsed);

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
