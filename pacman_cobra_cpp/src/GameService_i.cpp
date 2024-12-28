#include <GameService_i.h>

PacmanServer::PlayerData makeNewPlayer(int player_id, const std::string& name)
{
    PacmanServer::PlayerData player;
    printf("Create New Player with id: %d and name %s\n", player_id, name.c_str());
    player.player_id = player_id;
    player.position = { 0, 0 };
    player.name = name.c_str();
    player.score = 0;
    player.life = 3;
    player.status = "idle";
    player.immune = false;

    return player;
}

::CORBA::Long GameService_i::connect_to_server(::CORBA::Long version, const char* name)
{
    if (version == 1 || version == 2)
    {
        for (auto& server : _servers)
        {
            if (server._server.number_of_players < server.max_players_per_server 
                    && server._server.is_active && server.version == version)
            {
                server.player_mutex.lock();

                server.add_player(makeNewPlayer(number_of_players, name));
                number_of_players += 1;

                server.player_mutex.unlock();

                return number_of_players - 1;
            }
        }
        _servers.push_back(std::move(Server(number_of_servers, maps[0], makeNewPlayer(number_of_players, name), version)));
        number_of_players += 1;
        number_of_servers += 1;
        return number_of_players - 1;
    }
    else
        printf("Client tried to connect with unexpected version\n");

    return -1;
}

void GameService_i::disconnect_from_server(::CORBA::Long version, ::CORBA::Long player_id)
{
    if (version == 1 || version == 2)
    {
        for (auto& server : _servers)
        {
            server.player_mutex.lock();

            for (int i = 0; i < server._server.players.length(); i++)
            {
                auto& player = server._server.players[i];
                if (player.player_id == player_id)
                {
                    server.delete_player(player);
                    server.player_mutex.unlock();
                    return;
                }
            }
            server.player_mutex.unlock();
        }
    }
    else
    {
        printf("Client tried to disconnect with unexpected version\n");
    }
}

PacmanServer::GameMap* GameService_i::get_start_map(::CORBA::Long version, ::CORBA::Long level_number)
{
    if (version == 1 || version == 2)
    {
        auto* send_map = new PacmanServer::GameMap(maps[level_number]);
        return send_map;
    }
    else
    {
        printf("Client tried to connect with unexpected version\n");
    }
}

void GameService_i::update_player_status(::CORBA::Long version, ::CORBA::Long player_id, const char* status)
{
    printf("Player %d updated movement direction\n", player_id);

    if (version == 1 || version == 2)
    {
        for (auto& server : _servers)
        {
            for (int i = 0; i < server._server.players.length(); i++)
            {
                auto& player = server._server.players[i];
                if (player.player_id == player_id)
                {
                    server.update_player_status(player_id, std::string(status));
                    return;
                }
            }
        }
    }
    else
    {
        printf("Client tried to disconnect with unexpected version\n");
    }
}

PacmanServer::GameState* GameService_i::get_game_state(::CORBA::Long version, ::CORBA::Long player_id)
{
    auto now = std::chrono::steady_clock::now();

    if (version == 1 || version == 2)
    {
        for (auto& server : _servers)
        {
            server.player_mutex.lock();

            for (int i = 0; i < server._server.players.length(); i++)
            {
                auto& player = server._server.players[i];
                if (player.player_id == player_id)
                {
                    server._last_players_request_time[player_id] = now;

                    auto* send_server = new PacmanServer::GameState(server._server);
                    server.player_mutex.unlock();

                    return send_server;
                }
            }
            server.player_mutex.unlock();

        }

        return new PacmanServer::GameState();
    }
    else
    {
        printf("Client tried to disconnect with unexpected version\n");
    }
}

PacmanServer::PlayerData* GameService_i::get_player_state(::CORBA::Long version, ::CORBA::Long player_id)
{
    if (version == 1 || version == 2)
    {
        for (auto& server : _servers)
        {
            for (int i = 0; i < server._server.players.length(); i++)
            {
                auto& player = server._server.players[i];
                if (player.player_id == player_id)
                {
                    //server._last_players_request_time[player_id] = now;
                    auto* send_player = new PacmanServer::PlayerData(player);
                    return send_player;
                }
            }
        }

        return new PacmanServer::PlayerData();
    }
    else
    {
        printf("Client tried to disconnect with unexpected version\n");
    }
}
