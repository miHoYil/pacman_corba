import pygame, sys, os
from random import random

# Get the parent directory
parent_dir = os.path.abspath(os.path.join(os.getcwd(), '..'))

# Add the parent directory to sys.path
sys.path.append(parent_dir)

from omniORB import CORBA
import CosNaming, PacmanServer

from settings import CHAR_SIZE, NAV_HEIGHT

import world
from world import World

def main():
    # Initialise the ORB
    orb = CORBA.ORB_init(sys.argv, CORBA.ORB_ID)

    # Obtain a reference to the root naming context
    obj = orb.resolve_initial_references("NameService")
    rootContext = obj._narrow(CosNaming.NamingContext)


    # Resolve the name "GameService/PacmanTest.Object"
    name = [CosNaming.NameComponent("GameService", ""),
            CosNaming.NameComponent("PacmanTest", "Object")]
            
    obj = rootContext.resolve(name)        
    
    # Narrow the object to an PacmanServer::GameService
    game_service = obj._narrow(PacmanServer.GameService)

    # Play the game
    # try:
    player_idx =  game_service.connect_to_server(1, "Player's name" + str(random()))
    print(f"Result: {player_idx}")
    # finally:
    #     print("Server is not active")
    #     exit(0)

    game_map = game_service.get_start_map(1, 0)

    pygame.init()
    screen = pygame.display.set_mode((game_map.width * CHAR_SIZE, game_map.height * CHAR_SIZE + NAV_HEIGHT))
    pygame.display.set_caption("PacMan")
    keys = {'left': pygame.K_LEFT, 'right': pygame.K_RIGHT, 'up': pygame.K_UP, 'down': pygame.K_DOWN}

    FPS = pygame.time.Clock()
    world = World(screen, game_map, player_idx)

    while True:
        screen.fill("black")

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                game_service.disconnect_from_server(1, player_idx)
                sys.exit()

        pressed_key = pygame.key.get_pressed()
        for key, key_value in keys.items():
            if pressed_key[key_value]:
                game_service.update_player_status(1, player_idx, key)

        game_state = game_service.get_game_state(1, player_idx)
        world.update(game_state)
        pygame.display.update()
        pygame.display.flip()
        FPS.tick(30)





if __name__ == "__main__":
    main()
