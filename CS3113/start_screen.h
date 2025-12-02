#include "Scene.h"

class start_screen : public Scene {
public:
  start_screen();
  start_screen(Vector2 origin, const char *bgHexCode, const char *start_text);
  ~start_screen();
  void initialise() override;
  void update(float deltaTime) override;
  void render() override;
  void shutdown() override;

private:
  std::string start_text;
};
