import pygame
import time

from settings import NAV_HEIGHT, CHAR_SIZE
from pac import Pac
from cell import Cell
from berry import Berry
# from ghost import Ghost
from display import Display

class World:
	def __init__(self, screen, game_map, player_idx):
		self.screen = screen

		self.player_idx = player_idx
		self.players = {} #pygame.sprite.Group()
		self.ghosts = pygame.sprite.Group()
		self.walls = pygame.sprite.Group()
		self.berries = pygame.sprite.Group()

		self.width = game_map.width
		self.height = game_map.height
		self.map_text = game_map.map_text
		self.display = Display(self.screen, self.width, self.height)

		self.game_over = False
		self.reset_pos = False
		self.player_score = 0
		self.game_level = 1

		self._generate_world()


	# create and add player to the screen
	def _generate_world(self):
		# renders obstacle from the MAP table
		for y_index in range(0,self.height):
			for x_index in range(0,self.width):
				char = self.map_text[y_index * self.width + x_index]
				if char == "1":	# for walls
					self.walls.add(Cell(x_index, y_index, CHAR_SIZE, CHAR_SIZE))
				elif char == " ":	 # for paths to be filled with berries
					self.berries.add(Berry(x_index, y_index, CHAR_SIZE // 4))
				elif char == "B":	# for big berries
					self.berries.add(Berry(x_index, y_index, CHAR_SIZE // 2, is_power_up=True))

				# # for Ghosts's starting position
				# elif char == "s":
				# 	# self.ghosts.add(Ghost(x_index, y_index, "skyblue"))
				# elif char == "p":
				# 	# self.ghosts.add(Ghost(x_index, y_index, "pink"))
				# elif char == "o":
				# 	# self.ghosts.add(Ghost(x_index, y_index, "orange"))
				# elif char == "r":
				# 	# self.ghosts.add(Ghost(x_index, y_index, "red"))
				#
				# elif char == "P":	# for PacMan's starting position
				# 	# self.player.add(Pac(x_index, y_index))

	# displays nav
	def _dashboard(self):
		nav = pygame.Rect(0, self.height * CHAR_SIZE, self.width * CHAR_SIZE, NAV_HEIGHT)
		pygame.draw.rect(self.screen, pygame.Color("cornsilk4"), nav)

		for player in self.players.values():
			if player.player_idx == self.player_idx:
				self.display.show_life(player.life)
				self.display.show_score(player.pac_score)
		self.display.show_level(self.game_level)


	# def _check_game_state(self):
	# 	# checks if game over
	# 	if self.player.sprite.life == 0:
	# 		self.game_over = True
	#
	# 	# generates new level
	# 	if len(self.berries) == 0 and self.player.sprite.life > 0:
	# 		self.game_level += 1
	# 		for ghost in self.ghosts.sprites():
	# 			ghost.move_speed += self.game_level
	# 			ghost.move_to_start_pos()
	#
	# 		self.player.sprite.move_to_start_pos()
	# 		self.player.sprite.direction = (0, 0)
	# 		self.player.sprite.status = "idle"
	# 		self.generate_new_level()


	def update(self, game_state):
		self.game_level = game_state.level_number
		self.game_over =  not game_state.is_active

		updated_players = []
		#self.players.empty()
		for player in game_state.players:
			if player.player_id in self.players:
				self.players[player.player_id].update_data(player.position.x, player.position.y,
															player.status, player.life, player.score,
															player.immune)
			else:
				self.players[player.player_id] = Pac(player.position.x, player.position.y, player.player_id,
													  player.status, player.life, player.score)
			updated_players.append(player.player_id)

		self.players = {key: value for key, value in self.players.items() if key in updated_players}

		# for player in self.players:
		# 	if player not in updated_players:
		# 		del self.players[player]

		self.berries.empty()
		for berry in game_state.berries:
			if berry.is_power_up:
				self.berries.add(Berry(berry.position.x, berry.position.y, CHAR_SIZE // 2, berry.is_power_up))
			else:
				self.berries.add(Berry(berry.position.x, berry.position.y, CHAR_SIZE // 4, berry.is_power_up))

		# rendering
		[wall.update(self.screen) for wall in self.walls.sprites()]
		[berry.update(self.screen) for berry in self.berries.sprites()]

		for player in self.players.values():
			player.animate()
			player.update(self.screen)

		# [player.animate() for player in self.players.sprites()]
		# [player.update(self.screen) for player in self.players.sprites()]
		# [ghost.update(self.walls_collide_list) for ghost in self.ghosts.sprites()]
		# self.ghosts.draw(self.screen)

		self.display.game_over() if self.game_over else None

		self._dashboard()

		# # reset Pac and Ghosts position after PacMan get captured
		# if self.reset_pos and not self.game_over:
		# 	[ghost.move_to_start_pos() for ghost in self.ghosts.sprites()]
		# 	self.player.sprite.move_to_start_pos()
		# 	self.player.sprite.status = "idle"
		# 	self.player.sprite.direction = (0,0)
		# 	self.reset_pos = False
		#
		# # for restart button
		# if self.game_over:
		# 	pressed_key = pygame.key.get_pressed()
		# 	if pressed_key[pygame.K_r]:
		# 		self.game_over = False
		# 		self.restart_level()