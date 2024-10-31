import sys, os
import pygame

# Get the parent directory
parent_dir = os.path.abspath(os.path.join(os.getcwd(), '..'))
# Add the parent directory to sys.path
sys.path.append(parent_dir)

from omniORB import CORBA, PortableServer
import CosNaming

from service_structs_impl import GameMapImpl
from game_service_impl import GameServiceImpl
from settings import MAIN_MAP, MAP_WIDTH, MAP_HEIGHT

def main():

    # Initialize the ORB
    orb = CORBA.ORB_init(sys.argv)

    # Create a POA (Portable Object Adapter)
    poa = orb.resolve_initial_references("RootPOA")

    # Create an instance of Echo_i and an Echo object reference
    gsi = GameServiceImpl([GameMapImpl(MAP_WIDTH, MAP_HEIGHT, ''.join(MAIN_MAP)), ])
    eo = gsi._this()

    # Register the object with the Name Service
    name_service = orb.resolve_initial_references("NameService")
    root_context = name_service._narrow(CosNaming.NamingContext)

    # Create a name for the game service
    name = [CosNaming.NameComponent("GameService", "")]
    try:
        testContext = root_context.bind_new_context(name)
        print("New test context bound")
    except CosNaming.NamingContext.AlreadyBound as ex:
        print("Test context already exists")
        obj = root_context.resolve(name)
        testContext = obj._narrow(CosNaming.NamingContext)
        if testContext is None:
            print("test.mycontext exists but is not a NamingContext")
            sys.exit(1)

    # Bind the Echo object to the test context
    name = [CosNaming.NameComponent("PacmanTest", "Object")]

    try:
        testContext.bind(name, eo)
        print("New PacmanTest object bound")
    except CosNaming.NamingContext.AlreadyBound:
        testContext.rebind(name, eo)
        print("PacmanTest binding already existed -- rebound")

    print("Game server is running...")

    # Activate the POA
    poaManager = poa._get_the_POAManager()
    poaManager.activate()

    pygame.init()
    FPS = pygame.time.Clock()

    while True:
        for server in gsi.servers:
            server.update()

        FPS.tick(30)
    # Block forever (or until the ORB is shut down)

if __name__ == "__main__":
    main()
