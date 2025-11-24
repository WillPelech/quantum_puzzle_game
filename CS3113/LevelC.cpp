#include "LevelC.h"

LevelC::LevelC()                                      : Scene { {0.0f}, nullptr   } {}


void LevelC::update_bullets(float deltaTime)
{
   if (!mGameState.map) return;
   float lb = mGameState.map->getLeftBoundary();
   float rb = mGameState.map->getRightBoundary();
   float tb = mGameState.map->getTopBoundary();
   float bb = mGameState.map->getBottomBoundary();

   std::vector<Entity*> alive;
   alive.reserve(mBullets.size());

   for (auto *b : mBullets) {
      Vector2 prev = b->getPosition();
      b->update(deltaTime, nullptr, mGameState.map, nullptr, 0);

      float xO=0.0f, yO=0.0f;
      bool hitTile = mGameState.map->isSolidTileAt(b->getPosition(), &xO, &yO);
      Vector2 p = b->getPosition();
      bool out = (p.x < lb || p.x > rb || p.y < tb || p.y > bb);
      bool stuck = (Vector2Distance(prev, p) < 0.5f);

      // Bullet-enemy collision check (AABB style)
      auto bulletHits = [b](Entity *target) -> bool {
         if (!target || !target->isActive()) return false;

         Vector2 bPos  = b->getPosition();
         Vector2 tPos  = target->getPosition();
         Vector2 bSize = b->getColliderDimensions();
         Vector2 tSize = target->getColliderDimensions();

         float xDistance = fabs(bPos.x - tPos.x) - ((bSize.x + tSize.x) / 2.0f);
         float yDistance = fabs(bPos.y - tPos.y) - ((bSize.y + tSize.y) / 2.0f);

         return (xDistance < 0.0f && yDistance < 0.0f);
      };

      bool hitEnemy = false;
      Entity *enemies[] = {
         real_enemy1, real_enemy2, real_enemy3, real_enemy4, real_enemy5,
         dead_enemy1, dead_enemy2, dead_enemy3, dead_enemy4, dead_enemy5
      };

      for (Entity *e : enemies) {
         if (bulletHits(e)) {
            e->setDeadOrAlive(DEAD);
            e->deactivate();
            hitEnemy = true;
         }
      }

      if (hitTile || out || stuck || hitEnemy) {
         delete b;
      } else {
         alive.push_back(b);
      }
   }

   mBullets.swap(alive);
}

void LevelC::render_bullets()
{
   for (auto *b : mBullets) {
      b->render();

      Vector2 pos = b->getPosition();

      const float trail_distance = 20.0f;
      Vector2 offset = { 0.0f, 0.0f };

      Direction d = b->getDirection();
      switch (d) {
         case LEFT:  offset.x =  trail_distance; break; // behind is to the right
         case RIGHT: offset.x = -trail_distance; break; // behind is to the left
         case UP:    offset.y =  trail_distance; break; // behind is down
         case DOWN:  offset.y = -trail_distance; break; // behind is up
         default:    offset.x = -trail_distance; break;
      }

      Vector2 spawnPos = { pos.x + offset.x, pos.y + offset.y };
      particle_system(spawnPos.x, spawnPos.y);
   }
}

void LevelC::particle_system(float x_pos, float y_pos)
{
   const int   NUM_PARTICLES = 10;
   const float MAX_OFFSET    = 12.0f;

   for (int i = 0; i < NUM_PARTICLES; ++i) {
      float halfW = (float)GetRandomValue(2, 4);
      float halfH = (float)GetRandomValue(2, 4);

      float offsetX = ((float)GetRandomValue(-100, 100) / 100.0f) * MAX_OFFSET;
      float offsetY = ((float)GetRandomValue(-100, 100) / 100.0f) * MAX_OFFSET;

      float cx = x_pos + offsetX;
      float cy = y_pos + offsetY;

      int x = (int)(cx - halfW);
      int y = (int)(cy - halfH);
      int w = (int)(halfW * 2.0f);
      int h = (int)(halfH * 2.0f);

      Rectangle rec = { (float)x, (float)y, (float)w, (float)h };
      Color redOutline = { 255, 0, 0, 255 };
      DrawRectangleLinesEx(rec, 2.0f, redOutline);
   }
}
LevelC::LevelC(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelC::~LevelC() { shutdown(); }

void LevelC::initialise()
{
   mGameState.nextSceneID = 0;
   mGameState.bgm = {0};
   mGameState.jumpSound = {0};
   mGameState.world = REAL;
   prev_state = mGameState.world;
   mGameState.bgm = LoadMusicStream("assets/background_music.mp3");
   SetMusicVolume(mGameState.bgm, 0.33f);
   PlayMusicStream(mGameState.bgm);

   mGameState.bulletSound = LoadSound("assets/gun_sound.mp3");
   mGameState.deathSound  = LoadSound("assets/death_sound.mp3");

   /*
      ----------- MAP -----------
   */
   mRealMap = new Map(
      LEVELC_WIDTH, LEVELC_HEIGHT,
      (unsigned int *) mLevelData1,
      "assets/cave_/cave.png",
      TILE_DIMENSION,
      5, 5,
      mOrigin
   );
   mGhostMap = new Map(
      LEVELC_WIDTH, LEVELC_HEIGHT,
      (unsigned int *) mLevelData2,
      "assets/ghost_tiles.png",
      TILE_DIMENSION,
      4, 4,
      mOrigin
   );
   mGameState.map = mRealMap;

   mBgReal = LoadTexture("assets/lava_floor.png");
   mBgGhost = LoadTexture("assets/ghost_floor.png");
   mCurrentBg = mBgReal;

   /*
      ----------- PROTAGONIST -----------
   */
  
   float sizeRatio  = 48.0f / 64.0f;

   // Assets from @see https://sscary.itch.io/the-adventurer-female
   mGameState.mouse = new Entity(
      {mOrigin.x - 300.0f, mOrigin.y - 200.0f}, // position
      {90.0f * sizeRatio, 90.0f},             // scale
      "assets/mouse.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 4, 4 },                                 // atlas dimensions
      mouseAnimationAtlas,                    // actual atlas
      PLAYER                                    // entity type
   );
   key = new Entity(
      {mOrigin.x + 300.0f, mOrigin.y -20.0f}, // position
      {50.0f , 50.0f*sizeRatio},             // scale
      "assets/key.png",                   // texture file address
      KEY 
   );
   door = new Entity(
      {mOrigin.x + 300.0f, mOrigin.y -20.0f}, // position
      {50.0f , 50.0f*sizeRatio},             // scale
      "assets/lock.png",                   // texture file address
      DOOR 
   );
   mGameState.mouse->setColliderDimensions({
      mGameState.mouse->getScale().x / 2.5f,
      mGameState.mouse->getScale().y / 2.1f
   });
   mGameState.mouse->setAcceleration({0.0f, 0.0f});
   
   /*
      ----------- CAMERA -----------
   */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mGameState.mouse->getPosition(); // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
   // Key and door start active in the REAL world; they will be ignored in GHOST world logic
   
   initialise_enemies();
   switch_enemy_set();
}
void LevelC::switch_worlds(){
   if (IsKeyPressed(KEY_V)){
      mGameState.world = (mGameState.world == REAL ? GHOST : REAL);
      if (mGameState.world == REAL){
         mGameState.map = mRealMap;
         mCurrentBg = mBgReal;
         mGameState.mouse->setTexture("assets/mouse.png");
         mGameState.mouse->setAnimationAtlas(mouseAnimationAtlas);
      } else {
         mGameState.map = mGhostMap;
         mCurrentBg = mBgGhost;
         mGameState.mouse->setTexture("assets/ghost.png");
         mGameState.mouse->setAnimationAtlas(ghostAnimationAtlas);
      }
      switch_enemy_set();
   }
}
void LevelC::initialise_enemies(){
    std::map<Direction, std::vector<int>> dragonAnimationAtlas = {
      {DOWN,  { 10,11,9}},
      {LEFT,  { 10,11,9}},
      {UP,    { 10,11,9}},
      {RIGHT, { 10,11,9}},
   };
   // Spawn enemies at fixed positions based on level origin (away from mouse start)
      real_enemy1 = new Entity(
      {mOrigin.x - 300.0f, mOrigin.y + 150.0f}, // position
      {50.0f , 50.0f },             // scale
      "assets/flying_twin_headed_dragon-red.png",                   // texture file address
      ATLAS,
      {4,3},
      dragonAnimationAtlas,
      NPC);
      real_enemy2 = new Entity(
      {mOrigin.x + 150.0f, mOrigin.y + 150.0f}, // position
      {50.0f , 50.0f },             // scale
      "assets/flying_dragon-gold.png",                   // texture file address
      ATLAS,
      {4,3},
      dragonAnimationAtlas,
      NPC);
      real_enemy3 = new Entity(
      {mOrigin.x + 300.0f, mOrigin.y + 120.0f}, // position
      {50.0f , 50.0f },             // scale
      "assets/flying_dragon-red.png",                   // texture file address
      ATLAS,
      {4,3},
      dragonAnimationAtlas,
      NPC);
      real_enemy4 = new Entity(
      {mOrigin.x + 200.0f, mOrigin.y - 200.0f}, // position
      {50.0f , 50.0f },             // scale
      "assets/flying_twin_headed_dragon-blue.png",                   // texture file address
      ATLAS,
      {4,3},
      dragonAnimationAtlas,
      NPC);
      real_enemy5 = new Entity(
      {mOrigin.x + 200.0f, mOrigin.y - 200.0f}, // position
      {50.0f , 50.0f },             // scale
      "assets/flying_twin_headed_dragon-blue.png",                   // texture file address
      ATLAS,
      {4,3},
      dragonAnimationAtlas,
      NPC);
     dead_enemy1 = new Entity(
      {mOrigin.x + 220.0f, mOrigin.y - 200.0f}, // position
      {50.0f , 50.0f },             // scale
      "assets/fire-fireball.gif",                   // texture file address
      NPC);
   dead_enemy2 = new Entity(
      {mOrigin.x - 120.0f, mOrigin.y + 150.0f}, // position
      {50.0f , 50.0f },             // scale
      "assets/fire-fireball.gif",                   // texture file address
      NPC);
   dead_enemy3 = new Entity(
      {mOrigin.x + 170.0f, mOrigin.y + 120.0f}, // position
      {50.0f , 50.0f },             // scale
      "assets/fire-fireball.gif",                   // texture file address
      NPC);
   dead_enemy4 = new Entity(
      {mOrigin.x , mOrigin.y}, // position
      {50.0f , 50.0f },             // scale
      "assets/fire-fireball.gif",                   // texture file address
      NPC);
   dead_enemy5 = new Entity(
      {mOrigin.x + 220.0f, mOrigin.y - 170.0f}, // position
      {50.0f , 50.0f },             // scale
      "assets/fire-fireball.gif",                   // texture file address
      NPC);
   key = new Entity(
      {mOrigin.x + 350.0f, mOrigin.y -100.0f}, // position
      {50.0f , 50.0f},             // scale
      "assets/key.png",                   // texture file address
      KEY 
   );
   door = new Entity(
      {mOrigin.x - 350.0f, mOrigin.y +50.0f}, // position
      {50.0f , 50.0f},             // scale
      "assets/lock.png",                   // texture file address
      DOOR  
   );
   real_enemy1->setAIType(FOLLOWER); real_enemy1->setAIState(FOLLOWING);
   real_enemy2->setAIType(FOLLOWER); real_enemy2->setAIState(FOLLOWING);
   real_enemy3->setAIType(WANDERER);
   real_enemy4->setAIType(FOLLOWER); real_enemy4->setAIState(FOLLOWING);
   real_enemy5->setAIType(FOLLOWER); real_enemy5->setAIState(FOLLOWING);

   dead_enemy1->setAIType(FOLLOWER); dead_enemy1->setAIState(FOLLOWING);
   dead_enemy2->setAIType(FOLLOWER); dead_enemy2->setAIState(FOLLOWING);
   dead_enemy3->setAIType(FOLLOWER); dead_enemy3->setAIState(FOLLOWING);
   dead_enemy4->setAIType(JUMPER); dead_enemy4->setAIState(JUMPING);
   dead_enemy5->setAIType(FOLLOWER); dead_enemy5->setAIState(FOLLOWING);

   // Make enemies move faster than the default speed
   const int ENEMY_SPEED = 100;
   real_enemy1->setSpeed(ENEMY_SPEED*4);
   real_enemy2->setSpeed(ENEMY_SPEED/2);
   real_enemy3->setSpeed(ENEMY_SPEED);
   real_enemy4->setSpeed(ENEMY_SPEED*2);
   real_enemy5->setSpeed(ENEMY_SPEED);

   dead_enemy1->setSpeed(ENEMY_SPEED*2);
   dead_enemy2->setSpeed(ENEMY_SPEED);
   dead_enemy3->setSpeed(ENEMY_SPEED);
   dead_enemy4->setSpeed(ENEMY_SPEED);
   dead_enemy5->setSpeed(ENEMY_SPEED);

   real_enemy1->setColliderDimensions({real_enemy1->getScale().x/3,real_enemy1->getScale().y/3});
   real_enemy2->setColliderDimensions({real_enemy2->getScale().x/3,real_enemy2->getScale().y/3});
   real_enemy3->setColliderDimensions({real_enemy3->getScale().x/3,real_enemy3->getScale().y/3});
   real_enemy4->setColliderDimensions({real_enemy4->getScale().x/3,real_enemy4->getScale().y/3});
   real_enemy5->setColliderDimensions({real_enemy5->getScale().x/3,real_enemy5->getScale().y/3});

   dead_enemy1->setColliderDimensions({dead_enemy1->getScale().x/3,dead_enemy1->getScale().y/3});
   dead_enemy2->setColliderDimensions({dead_enemy2->getScale().x/3,dead_enemy2->getScale().y/3});
   dead_enemy3->setColliderDimensions({dead_enemy3->getScale().x/3,dead_enemy3->getScale().y/3});
   dead_enemy4->setColliderDimensions({dead_enemy4->getScale().x/3,dead_enemy4->getScale().y/3});
   dead_enemy5->setColliderDimensions({dead_enemy5->getScale().x/3,dead_enemy5->getScale().y/3});

   dead_enemy4->setAcceleration({0.0f, 3900.0f});
   dead_enemy4->setJumpingPower(24000.0f);
}
void LevelC::switch_enemy_set(){
   if(mGameState.world == REAL){
      dead_enemy1->deactivate();
      dead_enemy2->deactivate();
      dead_enemy3->deactivate();
      dead_enemy4->deactivate();
      dead_enemy5->deactivate();
      real_enemy1->activate();
      real_enemy2->activate();
      real_enemy3->activate();
      real_enemy4->activate();
      real_enemy5->activate();
   }else{
      dead_enemy1->activate();
      dead_enemy2->activate();
      dead_enemy3->activate();
      dead_enemy4->activate();
      dead_enemy5->activate();
      real_enemy1->deactivate();
      real_enemy2->deactivate();
      real_enemy3->deactivate();
      real_enemy4->deactivate();
      real_enemy5->deactivate();
      
   }

}
void LevelC::update_enemies(float deltaTime){
   real_enemy1->update(
      deltaTime,
      mGameState.mouse,
      mGameState.map,
      nullptr,
      0
   );
   real_enemy2->update(
      deltaTime,
      mGameState.mouse,
      mGameState.map,
      nullptr,
      0
   );
   real_enemy3->update(
      deltaTime,
      mGameState.mouse,
      mGameState.map,
      nullptr,
      0
   );
   real_enemy4->update(
      deltaTime,
      mGameState.mouse,
      mGameState.map,
      nullptr,
      0
   );
   real_enemy5->update(
      deltaTime,
      mGameState.mouse,
      mGameState.map,
      nullptr,
      0
   );
   dead_enemy1->update(
      deltaTime,
      mGameState.mouse,
      mGameState.map,
      nullptr,
      0
   );
   dead_enemy2->update(
      deltaTime,
      mGameState.mouse,
      mGameState.map,
      nullptr,
      0
   );
   dead_enemy3->update(
      deltaTime,
      mGameState.mouse,
      mGameState.map,
      nullptr,
      0
   );
   dead_enemy4->update(
      deltaTime,
      mGameState.mouse,
      mGameState.map,
      nullptr,
      0
   );
   dead_enemy5->update(
      deltaTime,
      mGameState.mouse,
      mGameState.map,
      nullptr,
      0
   );
}
void LevelC::render_enemies(){
   dead_enemy1->render();
   dead_enemy2->render();
   dead_enemy3->render();
   dead_enemy4->render();
   dead_enemy5->render();
   real_enemy1->render();
   real_enemy2->render();
   real_enemy3->render();
   real_enemy4->render();
   real_enemy5->render();
}
void LevelC::update(float deltaTime)
{
   switch_worlds();
   UpdateMusicStream(mGameState.bgm);
   mFireTimer -= deltaTime;
   if (IsKeyPressed(KEY_SPACE) && mFireTimer <= 0.0f) {
      fire_bullet();
      mFireTimer = mFireCooldown;
   }
   mGameState.mouse->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      nullptr,        // collidable entities
      0               // col. entity count
   );

   update_bullets(deltaTime);

   update_enemies(deltaTime);

   Vector2 currentPlayerPosition = { mGameState.mouse->getPosition().x, mOrigin.y };

   if (mGameState.mouse->getPosition().y > 800.0f) mGameState.nextSceneID = 0;
   
   panCamera(&mGameState.camera, &currentPlayerPosition);
   if(prev_state != mGameState.world){
      prev_state = mGameState.world;
   }
   if (mGameState.world == REAL) {
      key -> update(deltaTime, mGameState.mouse, mGameState.map, nullptr, 0);
      door -> update(deltaTime, mGameState.mouse, mGameState.map, nullptr, 0);
   }
   // }
   // std::cout <<"made it to end game logic ";
   if (mGameState.mouse->getLives() <= 0)
   {
      mGameState.nextSceneID = 6;
   }
   else if (get_num_alive()== 0 && key->getActive()== INACTIVE && !door->getDeadOrAlive())
   {
      mGameState.nextSceneID = 7;
   }
   if (mGameState.mouse->getLives() < mPrevLives)
   {
      PlaySound(mGameState.deathSound);
   }
   mPrevLives = mGameState.mouse->getLives();
}

void LevelC::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   float mapWidth  = LEVELC_WIDTH * TILE_DIMENSION;
   float mapHeight = LEVELC_HEIGHT * TILE_DIMENSION;
   Rectangle src = { 0.0f, 0.0f, (float)mCurrentBg.width, (float)mCurrentBg.height };
   Rectangle dst = { mOrigin.x - mapWidth/2.0f, mOrigin.y - mapHeight/2.0f, mapWidth, mapHeight };
   DrawTexturePro(mCurrentBg, src, dst, {0.0f, 0.0f}, 0.0f, WHITE);

   mGameState.map->render();
   render_enemies();
   render_bullets();
   if (mGameState.world == REAL) {
      key -> render();
      door -> render();
   }
   mGameState.mouse->render();

}

void LevelC::shutdown()
{
   delete mGameState.mouse;
   if (mRealMap) { delete mRealMap; mRealMap = nullptr; }
   if (mGhostMap) { delete mGhostMap; mGhostMap = nullptr; }
   mGameState.map = nullptr;

   for (auto *b : mBullets) delete b;
   mBullets.clear();

   if (mBgReal.id) UnloadTexture(mBgReal);
   if (mBgGhost.id) UnloadTexture(mBgGhost);

   // UnloadMusicStream(mGameState.bgm);
   // UnloadSound(mGameState.jumpSound);
}

void LevelC::fire_bullet()
{  
   if (num_fired>20){
      return;
   }
   PlaySound(mGameState.bulletSound);
   const char *tex = (mGameState.world == REAL) ? "assets/mouse_bullet.gif" : "assets/ghost_bullet.gif";
   Vector2 pos = mGameState.mouse->getPosition();
   Vector2 scale = { 24.0f, 24.0f };
   Entity *b = new Entity(pos, scale, tex, BULLET);
   b->setColliderDimensions(scale);
   b->setAcceleration({0.0f, 0.0f});
   b->setSpeed(400);

   Direction d = mGameState.mouse->getDirection();
   switch (d) {
      case LEFT:  b->moveLeft(); break;
      case RIGHT: b->moveRight(); break;
      case UP:    b->moveUp(); break;
      case DOWN:  b->moveDown(); break;
      default:    b->moveRight(); break;
   }

   mBullets.push_back(b);
   num_fired +=1;
}

int LevelC::get_num_alive(){
   int count = 0;
   // std::cout <<"made it here 1";
   count += real_enemy1->getDeadOrAlive();
   count += real_enemy2->getDeadOrAlive();
   count += real_enemy3->getDeadOrAlive();
   count += real_enemy4->getDeadOrAlive();
   count += real_enemy5->getDeadOrAlive();
   count += dead_enemy1->getDeadOrAlive();
   count += dead_enemy2->getDeadOrAlive();
   count += dead_enemy3->getDeadOrAlive();
   count += dead_enemy4->getDeadOrAlive();
   count += dead_enemy5->getDeadOrAlive();
   // std::cout <<"made it here2";
   return count;
}