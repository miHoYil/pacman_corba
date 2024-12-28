import pygame

import PacmanServer

from settings import CHAR_SIZE
from animation import import_sprite


def get_ghost_type(ghost_type):
    if ghost_type == 0:
        return 'orange'
    elif ghost_type == 2:
        return 'pink'
    elif ghost_type == 3:
        return 'red'
    elif ghost_type == 1:
        return 'skyblue'

class Ghost(pygame.sprite.Sprite):
    def __init__(self, row, col, ghost_type, direction):
        super().__init__()

        self.abs_x = (row * CHAR_SIZE)
        self.abs_y = (col * CHAR_SIZE)

        self.status = 'up'
        self.ghost_type = 'orange'

        self.update_status(direction)
        self.ghost_type = get_ghost_type(ghost_type)

        # ghost animation
        self._import_character_assets()
        self.frame_index = 0
        self.animation_speed = 0.5
        self.image = self.animations["up"][self.frame_index]
        self.rect = self.image.get_rect(topleft=(self.abs_x, self.abs_y))
        self.mask = pygame.mask.from_surface(self.image)

    def update_status(self, direction):
        if direction == 0:
            self.status = 'up'
        elif direction == 1:
            self.status = 'down'
        elif direction == 2:
            self.status = 'left'
        elif direction == 3:
            self.status = 'right'

    def update_data(self, row, col, direction):
        self.abs_x = (row * CHAR_SIZE)
        self.abs_y = (col * CHAR_SIZE)
        self.rect = self.image.get_rect(topleft=(self.abs_x, self.abs_y))
        self.update_status(direction)

    # gets all the image needed for animating specific player action
    def _import_character_assets(self):
        character_path = "assets/ghosts/" + self.ghost_type
        self.animations = {
            "up": [],
            "down": [],
            "left": [],
            "right": [],
        }
        for animation in self.animations.keys():
            full_path = f"{character_path}/{animation}.png"
            img_surface = pygame.image.load(full_path).convert_alpha()
            self.animations[animation].append(img_surface)

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