#include "PlayMode.hpp"
#include "ColorTextureProgram.hpp"
#include "GL.hpp"

#include <glm/glm.hpp>
//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>
#include "read_write_chunk.hpp"

#include <random>
#include <fstream>
float getrand(float l, float r)
{
	float t = (float)rand() / RAND_MAX;
	return l + (r - l) * t;
};
void print_clx(glm::u8vec4 cl)
{
	std::cout << (unsigned int)cl.x << ' ' << (unsigned int)cl.y << ' ' << (unsigned int)cl.z << ' ' << (unsigned int)cl.w << std::endl;
}

void print_palettex(const PPU466::Palette &pl)
{
	for (int i = 0; i < 4; i++)
	{
		print_clx(pl[i]);
	}
}
std::string level_path2(std::string const &filename, std::string const &suffix)
{
	return "../assets/" + filename + "." + suffix;
}

PlayMode::PlayMode()
{
	auto loading = [&](char const *o_str, int x, int y) {
		std::vector<PPU466::Tile> tl;
		std::vector<PPU466::Palette> pl;
		std::string name(o_str);
		std::ifstream tl_input(level_path2(name, "tl").c_str());
		std::ifstream pl_input(level_path2(name, "pl").c_str());
		size_t len = name.size();
		//std::cout << "succeed1\n";
		read_chunk(tl_input, (name.substr(len - 3, 3) + "t").c_str(), &tl);
		//std::cout << "succeed2\n";
		read_chunk(pl_input, (name.substr(len - 3, 3) + "p").c_str(), &pl);
		ppu.tile_table[x] = tl[0];
		ppu.palette_table[y] = pl[0];
	};
	//TODO:
	// you *must* use an asset pipeline of some sort to generate tiles.
	// don't hardcode them like this!
	// or, at least, if you do hardcode them like this,
	//  make yourself a script that spits out the code that you paste in here
	//   and check that script into your repository.

	//Also, *don't* use these tiles in your game:

	//use sprite 32 as a "player":
	p[1].pos.x = 156;
	loading("tankx", 7, 7);
	loading("tanky", 6, 6);
	loading("project", 3, 3);
	loading("explosion", 1, 1);
	loading("normal", 0, 0);
	ppu.palette_table[6][2] = {26, 134, 255, 255};
	ppu.palette_table[6][3] = {5, 197, 52, 255};

	p[0].self.cd.y = p[1].self.cd.y = 0.8f;
	for (int i = 0; i < 6; i++)
	{
		objcd ob;
		ob.cd = glm::vec2(0.0f, 0.5f);
		ob.pos = glm::vec2(getrand(0.0, 256.0f), getrand(120.0f, 230.0f));
		trees.push_back(ob);
	}
}

PlayMode::~PlayMode()
{
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size)
{

	if (evt.type == SDL_KEYDOWN)
	{
		if (evt.key.keysym.sym == SDLK_LEFT)
		{
			p[0].left.downs += 1;
			p[0].left.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_RIGHT)
		{
			p[0].right.downs += 1;
			p[0].right.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_UP)
		{
			p[0].up.downs += 1;
			p[0].up.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_DOWN)
		{
			p[0].down.downs += 1;
			p[0].down.pressed = true;
			return true;
		}
		if (evt.key.keysym.sym == SDLK_a)
		{
			p[1].left.downs += 1;
			p[1].left.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_d)
		{
			p[1].right.downs += 1;
			p[1].right.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_w)
		{
			p[1].up.downs += 1;
			p[1].up.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_s)
		{
			p[1].down.downs += 1;
			p[1].down.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_SPACE)
		{
			p[1].space.pressed = true;
			return true;
		}
	}
	else if (evt.type == SDL_KEYUP)
	{
		if (evt.key.keysym.sym == SDLK_LEFT)
		{
			p[0].left.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_RIGHT)
		{
			p[0].right.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_UP)
		{
			p[0].up.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_DOWN)
		{
			p[0].down.pressed = false;
			return true;
		}
		if (evt.key.keysym.sym == SDLK_a)
		{
			p[1].left.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_d)
		{
			p[1].right.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_w)
		{
			p[1].up.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_s)
		{
			p[1].down.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_SPACE)
		{
			p[1].space.pressed = false;
			return true;
		}
	}

	return false;
}
void capx(glm::vec2 &po, float l, float r)
{
	if (po.x < l)
		po.x = l;
	if (po.x > r)
		po.x = r;
}
void capy(glm::vec2 &po, float l, float r)
{
	if (po.y < l)
		po.y = l;
	if (po.y > r)
		po.y = r;
}
void PlayMode::update(float elapsed)
{
	constexpr float PlayerSpeed = 30.0f;
	for (int i = 0; i < 2; i++)
	{
		if (p[i].left.pressed)
			p[i].pos.x -= PlayerSpeed * elapsed;
		if (p[i].right.pressed)
			p[i].pos.x += PlayerSpeed * elapsed;
		if (p[i].down.pressed)
			p[i].pos.y -= PlayerSpeed * elapsed;
		if (p[i].up.pressed)
			p[i].pos.y += PlayerSpeed * elapsed;
		for (auto &proj : p[i].projectiles)
		{
			proj.pos.y += PlayerSpeed * elapsed;
			capx(proj.pos, 128.0f * i, 128.0f * (i + 1));
			capy(proj.pos, 0.0f, 256.0f);
		}
		//reset button press counters:
		p[i].self.cd.x += elapsed;
		p[i].left.downs = 0;
		p[i].right.downs = 0;
		p[i].up.downs = 0;
		p[i].down.downs = 0;
		capx(p[i].pos, 128.0f * i, 128.0f * (i + 1));
		capy(p[i].pos, 0.0f, 256.0f);

		if (p[i].space.pressed && p[i].self.cd.x > p[i].self.cd.y)
		{
			p[i].projectiles.push_back({glm::vec2(0.0f), p[i].pos});
			p[i].self.cd.x = 0;
		}
	}

	//std::cout<<std::endl;
}

void PlayMode::draw(glm::uvec2 const &drawable_size)
{
	//--- set ppu state based on game state ---
	ppu.background_color = glm::u8vec4(127, 127, 127, 255);
	//player sprite:
	ppu.sprites[0].x = int32_t(p[0].pos.x);
	ppu.sprites[0].y = int32_t(p[0].pos.y);
	ppu.sprites[0].index = 7;
	ppu.sprites[0].attributes = 7;

	ppu.sprites[1].x = int32_t(p[1].pos.x);
	ppu.sprites[1].y = int32_t(p[1].pos.y);
	ppu.sprites[1].index = 6;
	ppu.sprites[1].attributes = 6;
	//--- actually draw ---

	for (int i = 2; i < 8; i++)
	{
		ppu.sprites[i].x = int32_t(trees[i-2].pos.x);
		ppu.sprites[i].y = int32_t(trees[i-2].pos.y);
		ppu.sprites[i].index = 0;
		ppu.sprites[i].attributes = 0;
	}
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < p[i].projectiles.size(); j++)
		{
			auto &proj = p[i].projectiles[j];
			int k = (j + 1) * 2 + 6 + i;
			ppu.sprites[k].x = int32_t(proj.pos.x);
			ppu.sprites[k].y = int32_t(proj.pos.y);
			ppu.sprites[k].index = 3;
			ppu.sprites[k].attributes = 3;
		}
	}
	ppu.draw(drawable_size);
}
