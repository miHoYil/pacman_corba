from time import sleep

import pygame

import PacmanServer

from settings import CHAR_SIZE
from animation import import_sprite


class Pac(pygame.sprite.Sprite):
	def __init__(self, row, col, player_idx, status, life, pac_score):
		super().__init__()

		self.player_idx = player_idx
		self.abs_x = (row * CHAR_SIZE)
		self.abs_y = (col * CHAR_SIZE)

		# pac animation
		self._import_character_assets()
		self.frame_index = 0
		self.animation_speed = 0.5
		self.image = self.animations["idle"][self.frame_index]
		self.rect = self.image.get_rect(topleft = (self.abs_x, self.abs_y))
		self.mask = pygame.mask.from_surface(self.image)


		self.immune = False

		self.keys = {'left': pygame.K_LEFT, 'right': pygame.K_RIGHT, 'up': pygame.K_UP, 'down': pygame.K_DOWN}
		self.direction = (0, 0)
	
		# pac status
		self.status = status
		self.life = life
		self.pac_score = pac_score


	# gets all the image needed for animating specific player action
	def _import_character_assets(self):
		character_path = "assets/pac/"
		self.animations = {
			"up": [],
			"down": [],
			"left": [],
			"right": [],
			"idle": [],
			"power_up": []
		}
		for animation in self.animations.keys():
			full_path = character_path + animation
			self.animations[animation] = import_sprite(full_path)

	# update with sprite/sheets
	def animate(self):
		animation = self.animations[self.status]

		# loop over frame index
		self.frame_index += self.animation_speed
		if self.frame_index >= len(animation):
			self.frame_index = 0
		image = animation[int(self.frame_index)]
		self.image = pygame.transform.scale(image, (CHAR_SIZE, CHAR_SIZE))


	def update(self, screen):
		screen.blit(self.image, self.rect)
		# self.rect = self.image.get_rect(topleft=(self.rect.x, self.rect.y))