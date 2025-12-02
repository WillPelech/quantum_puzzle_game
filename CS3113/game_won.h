#include "Scene.h"

class game_won : public Scene {
public:
  game_won();
  game_won(Vector2 origin, const char *bgHexCode, const char *start_text);
  ~game_won();
  void initialise() override;
  void update(float deltaTime) override;
  void render() override;
  void shutdown() override;

private:
  std::string start_text;
};
