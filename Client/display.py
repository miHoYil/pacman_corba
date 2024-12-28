import pygame

from settings import CHAR_SIZE

pygame.font.init()

class Display:
	def __init__(self, screen, width, height):
		self.screen = screen
		self.font = pygame.font.SysFont("ubuntumono", CHAR_SIZE)
		self.game_over_font = pygame.font.SysFont("dejavusansmono", 48)
		self.text_color = pygame.Color("crimson")
		self.width = width * CHAR_SIZE
		self.height = height * CHAR_SIZE
		self.table_font = pygame.font.Font(None, CHAR_SIZE)
				
	def show_life(self, life):
		img_path = "assets/life/life.png"
		life_image = pygame.image.load(img_path)
		life_image = pygame.transform.scale(life_image, (CHAR_SIZE, CHAR_SIZE))
		life_x = CHAR_SIZE // 2

		if life != 0:
			for life in range(life):
				self.screen.blit(life_image, (life_x, self.height + (CHAR_SIZE // 2)))
				life_x += CHAR_SIZE

	def show_level(self, level):
		level_x = self.width // 3
		level = self.font.render(f'Level {level}', True, self.text_color)
		self.screen.blit(level, (level_x, (self.height + (CHAR_SIZE // 2))))

	def show_score(self, score):
		score_x = self.width // 3
		score = self.font.render(f'{score}', True, self.text_color)
		self.screen.blit(score, (score_x * 2, (self.height + (CHAR_SIZE // 2))))

	# add game over message
	def game_over(self):
		message = self.game_over_font.render(f'GAME OVER!!', True, pygame.Color("chartreuse"))
		self.screen.blit(message, ((self.width // 4), (self.height // 3)))

	def game_finished(self, scores):
		max_score_entry = max(scores, key=lambda x: x["score"])
		message = self.game_over_font.render(f'PLAYER {max_score_entry['name']} WINS!!', True, pygame.Color("chartreuse"))
		self.screen.blit(message, ((self.width // 4), (self.height // 3)))

	# Function to draw the score table
	def draw_score_table(self, scores):
		# Draw the table header
		header_surface = self.table_font.render("Score Table", True, pygame.Color("chartreuse"))
		self.screen.blit(header_surface, (self.width // 2 - header_surface.get_width() / 2, (self.height // 3 + CHAR_SIZE * 2)))

		# Draw the column headers
		name_header_surface = self.table_font.render("Name", True, pygame.Color("chartreuse"))
		score_header_surface = self.table_font.render("Score", True, pygame.Color("chartreuse"))
		self.screen.blit(name_header_surface, ((self.width // 4), (self.height // 3 + CHAR_SIZE * 3)))
		self.screen.blit(score_header_surface, ((self.width // 2), (self.height // 3 + CHAR_SIZE * 3)))

		scores.sort(key=lambda x: x["score"], reverse=True)

		# Draw the scores
		for index, entry in enumerate(scores):
			name_surface = self.table_font.render(entry["name"], True, pygame.Color("chartreuse"))
			score_surface = self.table_font.render(str(entry["score"]), True, pygame.Color("chartreuse"))
			self.screen.blit(name_surface, ((self.width // 4), self.height // 3 + (index + 4) * CHAR_SIZE))
			self.screen.blit(score_surface, ((self.width // 2), self.height // 3 + (index + 4) * CHAR_SIZE))