import time
import PacmanServer

from service_structs_impl import Vec2Impl, BerryDataImpl

from settings import MAP_WIDTH,MAP_HEIGHT

class Server(PacmanServer.GameState):
    def __init__(self, server_id, game_map):
        print("Create New Server with id: ", server_id)
        self.server_id = server_id
        self.level_number = 0
        self.is_active = True
        self.number_of_players = 0
        self.players = []
        self.number_of_ghosts = 0
        self.ghosts = []
        self.number_of_berries = 0
        self.berries = []
        self.max_players_per_server = 4
        self.player_default_pos = Vec2Impl(0,0)
        self.walls_collide_list = []

        self.update_map(game_map)

    def __init__(self, server_id, game_map, first_player):
        print("Create New Server with id: ", server_id)
        self.server_id = server_id
        self.level_number = 0
        self.is_active = True
        self.number_of_players = 0
        self.players = []
        self.number_of_ghosts = 0
        self.ghosts = []
        self.number_of_berries = 0
        self.berries = []
        self.max_players_per_server = 4
        self.player_default_pos = Vec2Impl(0,0)
        self.walls = []

        self.update_map(game_map)
        self.add_player(first_player)


    def add_player(self, player):
        print("Added player ID", player.player_id, " to server idx ", self.server_id)
        self.number_of_players += 1
        player.position = Vec2Impl(self.player_default_pos.x, self.player_default_pos.y)
        self.players.append(player)

    def delete_player(self, player):
        print("Remove player from server idx ", self.server_id, "player id", player.player_id)
        self.number_of_players -= 1
        self.players.remove(player)

    def update(self):
        self.update_players()

    def update_player_status(self, player_id, status):
        for player in self.players:
            if player_id == player.player_id:
                tempPos = Vec2Impl(player.position.x + player.directions[status].x,
                                   player.position.y + player.directions[status].y)
                collision = False
                for wall in self.walls:
                    if wall.x + 1 > tempPos.x > wall.x - 1 and wall.y + 1 > tempPos.y > wall.y - 1:
                        collision = True
                        break
                if not collision:
                    player.status = status

    def update_players(self):
        for player in self.players:
            if time.time() - player.request_time > 2:
               self.delete_player(player)
               continue

            tempPos = Vec2Impl(player.position.x + player.directions[player.status].x,
                               player.position.y + player.directions[player.status].y)

            collision = False
            for wall in self.walls:
                if wall.x + 1 > tempPos.x > wall.x - 1 and wall.y + 1 > tempPos.y > wall.y - 1:
                    collision = True
                    break

            if not collision:
                player.position.x += player.directions[player.status].x
                player.position.y += player.directions[player.status].y

            # teleporting to the other side of the map
            if player.position.x <= 0:
                player.position.x = MAP_WIDTH
            elif player.position.x >= MAP_WIDTH:
                player.position.x = 0

            if player.position.y <= 0:
                player.position.y = MAP_HEIGHT
            elif player.position.y >= MAP_HEIGHT:
                player.position.y = 0

            for berry in self.berries:
                if player.position.x == berry.position.x and player.position.y == berry.position.y:
                    if berry.is_power_up:
                        player.score += 50
                        player.immune = True
                        player.immune_start_time = time.time()
                    else:
                        player.score += 10
                    self.berries.remove(berry)
    #

    def update_map(self, game_map):
        self.walls.clear()
        self.berries.clear()
        for y_index in range(0, game_map.height):
            for x_index in range(0, game_map.width):
                char = game_map.map_text[y_index * game_map.width + x_index]
                if char == " ":  # for paths to be filled with berries
                    self.berries.append(BerryDataImpl(Vec2Impl(x_index, y_index), False))
                elif char == "B":  # for big berries
                    self.berries.append(BerryDataImpl(Vec2Impl(x_index, y_index), True))
                elif char == "P":  # for PacMan's starting position
                    self.player_default_pos = Vec2Impl(x_index, y_index)
                elif char == "1":  # for walls
                    self.walls.append(Vec2Impl(x_index, y_index))
            # self.walls_collide_list = [wall.rect for wall in self.walls.sprites()]
            # # for Ghosts's starting position
            # elif char == "s":
            # 	# self.ghosts.add(Ghost(x_index, y_index, "skyblue"))
            # elif char == "p":
            # 	# self.ghosts.add(Ghost(x_index, y_index, "pink"))
            # elif char == "o":
            # 	# self.ghosts.add(Ghost(x_index, y_index, "orange"))
            # elif char == "r":
            # 	# self.ghosts.add(Ghost(x_index, y_index, "red"))
