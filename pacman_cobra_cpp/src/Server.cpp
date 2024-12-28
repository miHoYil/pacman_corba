#include "Server.h"

Server::Server(int server_id, PacmanServer::GameMap game_map, int version)
{
	printf("Create New Server with id: %d", server_id);
	this->version = version;

	_server.server_id = server_id;
	_server.level_number = 0;
	_server.is_active = true;
	_server.number_of_players = 0;
	_server.number_of_ghosts = 0;
	_server.number_of_berries = 0;
	_player_default_pos = {0,0};

	_game_map = game_map;

	_server.players.replace(max_players_per_server, 0,
		PacmanServer::GameState::_players_seq::allocbuf(max_players_per_server));

	_server.berries.replace(game_map.width * game_map.height, 0,
		PacmanServer::GameState::_berries_seq::allocbuf(game_map.width * game_map.height));

	_server.ghosts.replace(max_ghost_number, 0, PacmanServer::GameState::_ghosts_seq::allocbuf(max_ghost_number));

	update_map(game_map);
	update_ghost();
}

Server::Server(int server_id, PacmanServer::GameMap game_map, PacmanServer::PlayerData first_player, int version)
{
	printf("Create New Server with id: %d\n", server_id);
	this->version = version;

	_server.server_id = server_id;
	_server.level_number = 0;
	_server.is_active = true;
	_server.number_of_players = 0;
	_server.number_of_ghosts = 0;
	_server.number_of_berries = 0;
	_player_default_pos = { 0,0 };

	_game_map = game_map;

	_server.players.replace(max_players_per_server,0,
						PacmanServer::GameState::_players_seq::allocbuf(max_players_per_server));

	_server.berries.replace(game_map.width * game_map.height, 0,
						PacmanServer::GameState::_berries_seq::allocbuf(game_map.width * game_map.height));

	_server.ghosts.replace(max_ghost_number, 0, PacmanServer::GameState::_ghosts_seq::allocbuf(max_ghost_number));

	update_map(game_map);
	update_ghost();
	add_player(first_player);
}

Server::~Server()
{
	printf("Server %d is destroyed \n", _server.server_id);
	//PacmanServer::GameState::_players_seq::freebuf(_server.players.get_buffer());
	//PacmanServer::GameState::_berries_seq::freebuf(_server.berries.get_buffer());
	//PacmanServer::GameState::_ghosts_seq::freebuf(_server.ghosts.get_buffer());
}

void Server::add_player(PacmanServer::PlayerData player)
{
	//player_mutex.lock();

	auto now = std::chrono::steady_clock::now();

	printf("Added player ID %d to server idx %d \n", player.player_id, _server.server_id);
	_server.number_of_players += 1;
	player.position = { _player_default_pos.x,_player_default_pos.y };

	const CORBA::ULong len = _server.players.length();
	_server.players.length(len + 1);
	_server.players[len] = player;

	_last_players_request_time[player.player_id] = now;
	_players_start_immune_time[player.player_id] = now;

	//player_mutex.unlock();
}

void Server::delete_player(PacmanServer::PlayerData player)
{
	//player_mutex.lock();

	printf("Remove player from server idx %d player id %d\n", _server.server_id, player.player_id);
	_server.number_of_players -= 1;

	for (int i = 0; i < _server.players.length(); i++) 
	{
		if (_server.players[i].player_id == player.player_id && i != _server.players.length() - 1)
		{
			memmove(_server.players.get_buffer() + i, _server.players.get_buffer() + i + 1, (_server.players.length() - 1) - i);
			break;
		}
	}

	const CORBA::ULong len = _server.players.length();
	_server.players.length(len - 1);

	_last_players_request_time.erase(player.player_id);
	_players_start_immune_time.erase(player.player_id);

	//player_mutex.unlock();
}

void Server::update()
{
	update_ghost();
	update_players();
}

bool Server::is_empty() const
{
	const CORBA::ULong len = _server.players.length();
	return len <=0;
}


void Server::update_player_status(int player_id, std::string status)
{
	player_mutex.lock_shared();

	for (int i = 0; i < _server.players.length(); i++) 
	{
		auto& player = _server.players[i];
		if (player_id == player.player_id) 
		{
			PacmanServer::Vec2 tempPos = { player.position.x + directions.at(status).x,
				player.position.y + directions.at(status).y };

			bool collision = false;

			for (auto& wall : _walls) 
			{
				if ((wall.x + 1 > tempPos.x) && (tempPos.x > wall.x - 1) && (wall.y + 1 > tempPos.y) && (tempPos.y > wall.y - 1))
				{
					collision = true;
					break;
				}
			}
			if (!collision)
				player.status = CORBA::String_var(status.c_str());
		}
	}

	player_mutex.unlock_shared();
}

void Server::restart_game() 
{
	printf("Server %d restarting!\n", _server.server_id);

	//PacmanServer::GameState::_ghosts_seq::freebuf(_server.ghosts.get_buffer());
	//_server.ghosts.replace(4, 0, PacmanServer::GameState::_ghosts_seq::allocbuf(4));
	_server.ghosts.length(0);
	_ghost.clear();

	update_map(_game_map);
	update_ghost();

	player_mutex.lock_shared();
	for (int i = 0; i < _server.players.length(); i++)
	{
		auto& player = _server.players[i];
		player.position = { _player_default_pos.x,_player_default_pos.y };
		player.score = 0;
		player.life = 3;
	}
	player_mutex.unlock_shared();

	_server.is_active = true;

	printf("Server %d finished restarting!\n", _server.server_id);

}


void Server::update_players()
{
	auto now = std::chrono::steady_clock::now();

	if (!_server.is_active &&
		std::chrono::duration_cast<std::chrono::seconds>(now - _game_end_time).count() > 5)
			restart_game();

	player_mutex.lock_shared();

	std::vector<PacmanServer::PlayerData> delete_players;
	std::vector< PacmanServer::BerryData> delete_berries;

	int dead_players = 0;
	for (int i = 0; i < _server.players.length(); i++)
	{
		auto& player = _server.players[i];
		if (player.life <= 0)
		{
			dead_players++;
			continue;
		}

		if (std::chrono::duration_cast<std::chrono::seconds>(
				now - _last_players_request_time[player.player_id]).count() > disconnect_timeout)
		{
			delete_players.push_back(player);
			continue;
		}

		if (player.immune)
		{
			if(std::chrono::duration_cast<std::chrono::seconds>(
				now - _players_start_immune_time[player.player_id]).count() > immune_duration)
				player.immune = false;
		}

		std::string player_status = (char*) player.status;

		//# teleporting to the other side of the map
		PacmanServer::Vec2 tempPos = { player.position.x + directions.at(player_status).x,
				player.position.y + directions.at(player_status).y };
		
		bool collision = false;
		for (auto& wall : _walls)
		{
			if ( (wall.x + 1 > tempPos.x) && (tempPos.x > wall.x - 1) && (wall.y + 1 > tempPos.y) && (tempPos.y > wall.y - 1))
			{
				collision = true;
				break;
			}
		}
		if (!collision)
		{
			player.position = tempPos;
		}
		
		if (player.position.x < 0)
			player.position.x = MAP_WIDTH - player_speed;
		else
			if (player.position.x > MAP_WIDTH)
				player.position.x = 0 + player_speed;

		if (player.position.y < 0)
			player.position.y = MAP_HEIGHT - player_speed;
		else
			if (player.position.y > MAP_HEIGHT)
				player.position.y = 0 + player_speed;


		for (int j = 0; j < _server.berries.length(); j++) 
		{
			auto berry = _server.berries[j];
			if (player.position.x == berry.position.x && player.position.y == berry.position.y) 
			{
				if (berry.is_power_up) 
				{
					player.score += 50;
					player.immune = true;
					_players_start_immune_time[player.player_id] = now;		
					delete_berries.push_back(berry);
				}
				else 
				{
					player.score += 10;
					delete_berries.push_back(berry);
				}
			}
		}

		for (auto& ghost : _ghost)
		{
			if ((ghost._data.position.x + 1 > player.position.x) && (player.position.x > ghost._data.position.x - 1)
				&& (ghost._data.position.y + 1 > player.position.y) && (player.position.y > ghost._data.position.y - 1))
			{
				if(player.immune)
				{
					ghost._data.position.x = ghost._default_pos.x;
					ghost._data.position.y = ghost._default_pos.y;
					player.score += 100;
				}
				else
				{
					player.position.y = _player_default_pos.y;
					player.position.x = _player_default_pos.x;
					player.life -= 1;
					player.status = "idle";
				}
			}
		}
	}

	if(dead_players == _server.players.length() && _server.is_active)
	{
		_server.is_active = false;
		_game_end_time = now;
	}


	PacmanServer::GameState::_berries_seq new_berries(
			_server.berries.length() - delete_berries.size(), _server.berries.length() - delete_berries.size(),
		PacmanServer::GameState::_berries_seq::allocbuf(_server.berries.length() - delete_berries.size()), false);
	
	int berry_count = 0;
	for (int ib = 0; ib < _server.berries.length(); ib++) 
	{
		auto& berry = _server.berries[ib];
		bool is_delete = false;
		for (int i = 0; i < delete_berries.size(); i++)
		{
			if (berry.position.x == delete_berries[i].position.x &&
				berry.position.y == delete_berries[i].position.y)
			{
				is_delete = true;
				break;
			}
		}

		if (!is_delete) 
		{
			new_berries[berry_count] = berry;
			berry_count++;
		}
	}
	_server.berries = new_berries;

	PacmanServer::GameState::_berries_seq::freebuf(new_berries.get_buffer());

	if (_server.berries.length() == 0 && _server.is_active)
	{
		_server.is_active = false;
		_game_end_time = now;
	}

	player_mutex.unlock_shared();

	player_mutex.lock();

	for (int i = 0; i < delete_players.size(); i++)
	{
		delete_player(delete_players[i]);
	}

	player_mutex.unlock();

}

void Server::update_ghost()
{
	if (version < 2)
		return;

	int ghost_idx = 0;
	for (auto& ghost : _ghost)
	{
		ghost.update(_walls);
		_server.ghosts[ghost_idx] = ghost._data;
		//printf("Ghost Data: %f, %f, %d, %d\n", _server.ghosts[ghost_idx].position.x, _server.ghosts[ghost_idx].position.y,
			//_server.ghosts[ghost_idx].direction, _server.ghosts[ghost_idx].type);
		ghost_idx++;
	}

}

void Server::update_map(PacmanServer::GameMap game_map)
{
	_server.berries.length(0);
	_walls.clear();

	for (int y_index = 0; y_index < game_map.height; y_index++) 
	{
		for (int x_index = 0; x_index < game_map.width; x_index++)
		{
			char cell = game_map.map_text[y_index * game_map.width + x_index];

			if (cell == ' ') // # for paths to be filled with berries
			{
				const CORBA::ULong len = _server.berries.length();
				_server.berries.length(len + 1);
				PacmanServer::BerryData berry;
				berry.is_power_up = false;
				berry.position = { static_cast<float>(x_index) , static_cast<float>(y_index) };
				_server.berries[len] = berry;
			} else
			if (cell == 'B') // # for big berries
			{
				const CORBA::ULong len = _server.berries.length();
				_server.berries.length(len + 1);
				PacmanServer::BerryData berry;
				berry.is_power_up = true;
				berry.position = { static_cast<float>(x_index) , static_cast<float>(y_index) };
				_server.berries[len] = berry;
			} else
			if (cell == 'P') //  # for PacMan's starting position
			{
				_player_default_pos = { static_cast<float>(x_index) , static_cast<float>(y_index) };
			} else
			if (cell == '1') //  # for walls
			{
				_walls.push_back({ static_cast<float>(x_index) , static_cast<float>(y_index) });
			} else
			if (cell == 's' && version > 1)
			{
				Ghost ghost(PacmanServer::GhostType::BLUE, { static_cast<float>(x_index) , static_cast<float>(y_index) });
				const int len = _server.ghosts.length();
				_server.ghosts.length(len + 1);
				_ghost.push_back(ghost);
			} else
			if (cell == 'p' && version > 1)
			{
				Ghost ghost(PacmanServer::GhostType::PINK, { static_cast<float>(x_index) , static_cast<float>(y_index) });
				const int len = _server.ghosts.length();
				_server.ghosts.length(len + 1);
				_ghost.push_back(ghost);
			} else
			if (cell == 'o' && version > 1)
			{
				Ghost ghost(PacmanServer::GhostType::ORANGE, { static_cast<float>(x_index) , static_cast<float>(y_index) });
				const int len = _server.ghosts.length();
				_server.ghosts.length(len + 1);
				_ghost.push_back(ghost);
			}else
			if (cell == 'r' && version > 1)
			{
				Ghost ghost(PacmanServer::GhostType::RED, { static_cast<float>(x_index) , static_cast<float>(y_index) });
				const int len = _server.ghosts.length();
				_server.ghosts.length(len + 1);
				_ghost.push_back(ghost);
			}

		}
	}
}
