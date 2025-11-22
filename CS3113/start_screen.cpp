#include "start_screen.h"

start_screen::start_screen() :Scene{ {0.0f},nullptr}{}
start_screen::start_screen(Vector2 origin, const char *bgHexCode, const char* start_text)
    : Scene { origin, bgHexCode }, 
    start_text(start_text)
{
}

start_screen::~start_screen() { shutdown(); }

void start_screen::initialise()
{
   mGameState.nextSceneID = 0;
   mGameState.camera = { 0 };
   mGameState.camera.target = mOrigin;
   mGameState.camera.offset = mOrigin;
   mGameState.camera.rotation = 0.0f;
   mGameState.camera.zoom = 1.0f;

   // mGameState.bgm = LoadMusicStream("assets/music/TheEntertainer.mp3");
   // SetMusicVolume(mGameState.bgm, 0.33f);
   // PlayMusicStream(mGameState.bgm);

   float sizeRatio  = 48.0f / 64.0f;
}

void start_screen::update(float deltaTime)
{
   if (IsKeyPressed(KEY_ENTER))
   {
      mGameState.nextSceneID = 5;
   }
}

void start_screen::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));
   int titleSize = 50;
   const char* title = "Both Sides";
   int titleX = 50;
   int titleY = 80;
   DrawText(title, titleX, titleY, titleSize, BLACK);

   int fontSize = 32;
   int x = 50;
   int y = titleY + titleSize + 30;
   DrawText(start_text.c_str(), x, y, fontSize, BLACK);
}

void start_screen::shutdown()
{
   // StopMusicStream(mGameState.bgm);
   // UnloadMusicStream(mGameState.bgm);
}