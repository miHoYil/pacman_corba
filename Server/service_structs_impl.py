import time
import PacmanServer

class Vec2Impl(PacmanServer.Vec2):
    def __init__(self):
        self.x = 0
        self.y = 0
    def __init__(self, x, y):
        self.x = x
        self.y = y

class BerryDataImpl(PacmanServer.BerryData):
    def __init__(self, pos, is_power_up):
        self.position = pos
        self.is_power_up = is_power_up

class PlayerDataImpl(PacmanServer.PlayerData):
    def __init__(self, player_id, name):
        print("Create New Player with id: ", player_id, " and name ", name)
        self.player_id = player_id
        self.position = Vec2Impl(0, 0)
        self.name = name
        self.score = 0
        self.life = 3
        self.status = "idle"
        self.immune = False
        self.immune_start_time = time.time()
        self.request_time = time.time()
        self.player_speed = 0.25
        self.directions = {'left': Vec2Impl(-self.player_speed,0) ,
                'right': Vec2Impl(self.player_speed,0),
                'up': Vec2Impl(0, -self.player_speed),
                    'down':  Vec2Impl(0, self.player_speed),
                'idle':  Vec2Impl(0, 0)}


class GameMapImpl(PacmanServer.GameMap):
    def __init__(self, width, height, map_text):
        self.width = width
        self.height = height
        self.map_text = map_text
