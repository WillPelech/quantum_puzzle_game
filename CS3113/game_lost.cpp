#include "game_lost.h"

game_lost::game_lost() :Scene{ {0.0f},nullptr}{}
game_lost::game_lost(Vector2 origin, const char *bgHexCode, const char* start_text)
    : Scene { origin, bgHexCode }, 
    start_text(start_text)
{
}

game_lost::~game_lost() { shutdown(); }

void game_lost::initialise()
{
   mGameState.nextSceneID = 0;

   mGameState.bgm = LoadMusicStream("assets/music/TheEntertainer.mp3");
   SetMusicVolume(mGameState.bgm, 0.33f);
   PlayMusicStream(mGameState.bgm);

   float sizeRatio  = 48.0f / 64.0f;
}

void game_lost::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);
   if (IsKeyPressed(KEY_ENTER))
   {
      mGameState.nextSceneID = 5;
   }
}

void game_lost::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));
   int fontSize = 40;
   int x = 50;
   int y = 300;
   DrawText(start_text.c_str(), x, y, fontSize,RED);
}

void game_lost::shutdown()
{
   StopMusicStream(mGameState.bgm);
   UnloadMusicStream(mGameState.bgm);
}