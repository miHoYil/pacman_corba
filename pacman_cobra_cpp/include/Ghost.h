#pragma once
#include <map>
#include <string>
#include <vector>
#include <random>

#include "Pacman.hh"

#include <Settings.h>

class Ghost
{
	std::random_device rd;  // Obtain a random number from hardware
	std::mt19937 eng;

	std::random_device rd_ver;  // Obtain a random number from hardware
	std::mt19937 eng_ver;

	const static inline float move_speed = 0.25;

	const static inline std::map<PacmanServer::Movement, PacmanServer::Vec2> directions =
	{
		{PacmanServer::Movement::LEFT, {-move_speed, 0 }},
		{PacmanServer::Movement::RIGHT, {move_speed, 0 }},
		{PacmanServer::Movement::UP, {0, -move_speed}},
		{PacmanServer::Movement::DOWN, {0, move_speed}},
	};

public:
	PacmanServer::GhostData _data;
	PacmanServer::Vec2 _default_pos;

	Ghost(const Ghost& ghost) 
	{
		_data = ghost._data;
		_default_pos = ghost._default_pos;

		eng = std::mt19937(rd()); // Seed the generator
		eng_ver = std::mt19937(rd_ver());
	};
	Ghost operator=(const Ghost& ghost)
	{
		_data = ghost._data;
		_default_pos = ghost._default_pos;

		eng = std::mt19937(rd()); // Seed the generator
		eng_ver = std::mt19937(rd_ver());

		return *this;
	};
	Ghost(PacmanServer::GhostType itype, PacmanServer::Vec2 pos)
	{
		_default_pos = pos;
		
		_data.type = itype;
		_data.position = pos;
		eng = std::mt19937(rd()); // Seed the generator
		eng_ver = std::mt19937(rd_ver());
		_data.direction = PacmanServer::Movement::UP;
	};

	void update(const std::vector<PacmanServer::Vec2>& walls);
};

