#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <map>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct objcd{
		glm::vec2 cd = glm::vec2(0.0f);
		glm::vec2 pos;
	};
	struct player{
		struct Button {
			uint8_t downs = 0;
			uint8_t pressed = 0;
		} left, right, down, up, space;
		glm::vec2 pos = glm::vec2(0.0f);
		objcd self;
		std::vector<objcd> projectiles;
		int score = 0;
	} p[2];

	std::vector<objcd> trees;



	//some weird background animation:
	float background_fade = 0.0f;

	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
