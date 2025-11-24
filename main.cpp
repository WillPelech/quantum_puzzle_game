#include "CS3113/LevelA.h"
#include "CS3113/LevelB.h"
#include "CS3113/LevelC.h"
#include "CS3113/ShaderProgram.h"
#include "CS3113/game_lost.h"
#include "CS3113/game_won.h"
#include "CS3113/start_menu.h"
#include "CS3113/start_screen.h"
// Global Constants
constexpr int SCREEN_WIDTH = 1000, SCREEN_HEIGHT = 600, FPS = 120,
              NUMBER_OF_LEVELS = 3;

constexpr Vector2 ORIGIN = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f, gTimeAccumulator = 0.0f;

Scene *gCurrentScene = nullptr;
std::vector<Scene *> gLevels = {};
start_screen *gIntroScene = nullptr;
start_menu *gMenu = nullptr;
LevelA *gLevelA = nullptr;
LevelB *gLevelB = nullptr;
LevelC *gLevelC = nullptr;
game_lost *gGameLost = nullptr;
game_won *gGameWon = nullptr;

Effects *gEffects = nullptr;

ShaderProgram gShader;
Vector2 gLightPosition = {0.0f, 0.0f};

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene) {
  gCurrentScene = scene;
  gCurrentScene->initialise();
}

void initialise() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Effects & Shaders");
  InitAudioDevice();

  gShader.load("shaders/vertex.glsl", "shaders/fragment.glsl");
  gIntroScene =
      new start_screen(ORIGIN, "#42c2f5", "Press ENTER to continue");
  gMenu = new start_menu(ORIGIN, "#42c2f5");
  gLevelA = new LevelA(ORIGIN, "#24190aff");
  gLevelB = new LevelB(ORIGIN, "#011627");
  gLevelC = new LevelC(ORIGIN, "#011627");
  gGameLost =
      new game_lost(ORIGIN, "#1d1660ff", "You lost! Press ENTER for menu");
  gGameWon = new game_won(ORIGIN, "#1d1660ff", "You won! Press ENTER for menu");

  gLevels.push_back(gLevelA);
  gLevels.push_back(gLevelB); // id 2
  gLevels.push_back(gLevelC); // id 3

  gCurrentScene = gIntroScene;
  gCurrentScene->initialise();

  gEffects = new Effects(ORIGIN, (float)SCREEN_WIDTH * 1.5f,
                         (float)SCREEN_HEIGHT * 1.5f);

  // gEffects->start(SHRINK);
  // gEffects->setEffectSpeed(2.0f);

  SetTargetFPS(FPS);
}

void processInput() {
  if (gCurrentScene && gCurrentScene->getState().mouse) {
    gCurrentScene->getState().mouse->resetMovement();

    if (IsKeyDown(KEY_A))
      gCurrentScene->getState().mouse->moveLeft();
    else if (IsKeyDown(KEY_D))
      gCurrentScene->getState().mouse->moveRight();
    else if (IsKeyDown(KEY_W))
      gCurrentScene->getState().mouse->moveUp();
    else if (IsKeyDown(KEY_S))
      gCurrentScene->getState().mouse->moveDown();

    if (GetLength(gCurrentScene->getState().mouse->getMovement()) > 1.0f)
      gCurrentScene->getState().mouse->normaliseMovement();
  }

  if (IsKeyPressed(KEY_Q) || WindowShouldClose())
    gAppStatus = TERMINATED;
}

void update() {
  float ticks = (float)GetTime();
  float deltaTime = ticks - gPreviousTicks;
  gPreviousTicks = ticks;

  deltaTime += gTimeAccumulator;

  if (deltaTime < FIXED_TIMESTEP) {
    gTimeAccumulator = deltaTime;
    return;
  }

  while (deltaTime >= FIXED_TIMESTEP) {
    gCurrentScene->update(FIXED_TIMESTEP);
    if (gCurrentScene && gCurrentScene->getState().mouse) {
      Vector2 cameraTarget = gCurrentScene->getState().mouse->getPosition();
      gEffects->update(FIXED_TIMESTEP, &cameraTarget);

      gLightPosition = gCurrentScene->getState().mouse->getPosition();
    }

    deltaTime -= FIXED_TIMESTEP;
  }
}

void render() {
  BeginDrawing();
  BeginMode2D(gCurrentScene->getState().camera);
  gShader.begin();

  gShader.setVector2("lightPosition", gLightPosition);
  gCurrentScene->render();

  gShader.end();
  gEffects->render();
  EndMode2D();
  EndDrawing();
}

void shutdown() {
  delete gIntroScene;
  delete gMenu;
  delete gLevelA;
  delete gLevelB;
  delete gLevelC;
  delete gGameLost;
  delete gGameWon;

  for (int i = 0; i < NUMBER_OF_LEVELS; i++)
    gLevels[i] = nullptr;

  delete gEffects;
  gEffects = nullptr;

  gShader.unload();

  CloseAudioDevice();
  CloseWindow();
}

int main(void) {
  initialise();

  while (gAppStatus == RUNNING) {
    processInput();
    update();

    if (gCurrentScene->getState().nextSceneID > 0) {
      int id = gCurrentScene->getState().nextSceneID;

      switch (id) {
      case 1:
        switchToScene(gLevelA);
        break;
      case 2:
        switchToScene(gLevelB);
        break;
      case 3:
        switchToScene(gLevelC);
        break;
      case 4:
        switchToScene(gIntroScene);
        break;
      case 5:
        switchToScene(gMenu);
        break;
      case 6:
        switchToScene(gGameLost);
        break;
      case 7:
        switchToScene(gGameWon);
        break;
      default:
        break;
      }

      gCurrentScene->getState().nextSceneID = 0;
    }

    render();
  }

  shutdown();

  return 0;
}
