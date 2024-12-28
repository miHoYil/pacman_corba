#pragma once
#include <string>
#include <vector>
#include <map>
#include <shared_mutex>
#include <chrono>

#include <Pacman.hh>
#include <Settings.h>
#include <Ghost.h>
#include <mutex>

class Server
{
	
public:
	Server(const Server& server) 
	{
		version = server.version;

		_server = server._server;
		_walls = server._walls;
		_ghost = server._ghost;
		_game_map = server._game_map;

		_player_default_pos = server._player_default_pos;
		_last_players_request_time = server._last_players_request_time;
		_players_start_immune_time = server._players_start_immune_time;	
	};
	Server& operator=(const Server& server)
	{
		version = server.version;

		_server = server._server;
		_walls = server._walls;
		_ghost = server._ghost;
		_game_map = server._game_map;

		_player_default_pos = server._player_default_pos;
		_last_players_request_time = server._last_players_request_time;
		_players_start_immune_time = server._players_start_immune_time;

		return *this;
	};

	Server(int server_id, PacmanServer::GameMap game_map, int version);
	Server(int server_id, PacmanServer::GameMap game_map, PacmanServer::PlayerData first_player, int version);
	~Server();

	void add_player(PacmanServer::PlayerData  player);
	void delete_player(PacmanServer::PlayerData  player);
	void update();
	void update_player_status(int player_id, std::string status);
	void update_players();
	void update_ghost();
	void update_map(PacmanServer::GameMap game_map);
	bool is_empty() const; 
	void restart_game();

public:
	int version = 2;

	const static inline double immune_duration = 3.0;
	const static inline double disconnect_timeout = 2.0;
	const static inline int max_ghost_number = 4;

	std::shared_mutex player_mutex;

 	const static int inline max_players_per_server = 4;
	const static float inline player_speed = 0.25;
	const static inline std::map<std::string, PacmanServer::Vec2> directions =
	{
		{"left", {-player_speed, 0 }},
		{"right", {player_speed, 0 }},
		{"up", {0, -player_speed}},
		{"down", {0, player_speed}},
		{"idle", {0, 0}}
	};

	PacmanServer::GameState _server;
	std::vector<PacmanServer::Vec2 > _walls;
	std::vector<Ghost> _ghost;
	PacmanServer::Vec2 _player_default_pos;
	PacmanServer::GameMap _game_map;

	std::map<int, std::chrono::steady_clock::time_point> _last_players_request_time;
	std::map<int, std::chrono::steady_clock::time_point> _players_start_immune_time;

	std::chrono::steady_clock::time_point _game_end_time;
};

