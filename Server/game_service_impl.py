import time
import PacmanServer__POA

from service_structs_impl import PlayerDataImpl
from one_server_impl import Server

class GameServiceImpl(PacmanServer__POA.GameService):
    def __init__(self, maps):
        self.number_of_players = 0
        self.number_of_servers = 0
        self.servers = []
        self.maps = maps

    def connect_to_server(self, version, name):
        if version == 1:
            for server in self.servers:
                if server.number_of_players < server.max_players_per_server:
                    server.add_player(PlayerDataImpl(self.number_of_players, name))
                    self.number_of_players += 1
                    return self.number_of_players - 1

            self.servers.append(Server(self.number_of_servers, self.maps[0], PlayerDataImpl(self.number_of_players, name)))
            self.number_of_players += 1
            self.number_of_servers += 1
            return self.number_of_players - 1
        else:
            print("Client tried to connect with unexpected version\n")

    def disconnect_from_server(self, version, player_id):
        if version == 1:
            for server in self.servers:
                for player in server.players:
                    if player.player_id == player_id:
                        server.delete_player(player)
                        return
        else:
            print("Client tried to connect with unexpected version\n")

    def get_start_map(self, version, level_number):
        if version == 1:
            return self.maps[level_number]
        else:
            print("Client tried to connect with unexpected version\n")

    def update_player_status(self, version, player_id, status):
        print("Player ", player_id," updated movement direction")
        if version == 1:
            for server in self.servers:
                for player in server.players:
                    if player.player_id == player_id:
                        server.update_player_status(player_id, status)
                        # player.status = status
                        return
        else:
            print("Client tried to connect with unexpected version\n")

    def get_game_state(self, version, player_id):
        if version == 1:
            for server in self.servers:
                for player in server.players:
                    if player.player_id == player_id:
                        player.request_time = time.time()
                        return server
        else:
            print("Client tried to connect with unexpected version\n")

    def get_player_state(self, version,player_id):
        if version == 1:
           for server in self.servers:
               for player in server.players:
                   if player.player_id == player_id:
                       return  player
        else:
            print("Client tried to connect with unexpected version\n")
