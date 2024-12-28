#pragma once
#include <vector>

#include <Pacman.hh>

#include <Server.h>
#include <Settings.h>

class GameService_i : public POA_PacmanServer::GameService
{
public:
    inline GameService_i()
    {
        PacmanServer::GameMap map;
        map.width = MAP_WIDTH;
        map.height = MAP_HEIGHT;
        map.map_text = MAIN_MAP;

        maps.push_back(map);
    }
    virtual ~GameService_i() {}

    ::CORBA::Long connect_to_server(::CORBA::Long version, const char* name);
    void disconnect_from_server(::CORBA::Long version, ::CORBA::Long player_id);

    PacmanServer::GameMap* get_start_map(::CORBA::Long version, ::CORBA::Long level_number);

    void update_player_status(::CORBA::Long version, ::CORBA::Long player_id, const char* status);

    PacmanServer::GameState* get_game_state(::CORBA::Long version, ::CORBA::Long player_id);
    PacmanServer::PlayerData* get_player_state(::CORBA::Long version, ::CORBA::Long player_id);

public:
    std::vector<PacmanServer::GameMap> maps;
    std::vector<Server> _servers;
    int number_of_players = 0;
    int number_of_servers = 0;
};
