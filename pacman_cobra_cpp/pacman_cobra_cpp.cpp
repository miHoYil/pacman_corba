#include <iostream>
#include <chrono>
#include <thread>

#include <Pacman.hh>

#include <Server.h>
#include <GameService_i.h>

static CORBA::Boolean bindObjectToName(CORBA::ORB_ptr orb, CORBA::Object_ptr objref)
{
    CosNaming::NamingContext_var rootContext;

    try {
        // Obtain a reference to the root context of the Name service:
        CORBA::Object_var obj = orb->resolve_initial_references("NameService");

        // Narrow the reference returned.
        rootContext = CosNaming::NamingContext::_narrow(obj);
        if (CORBA::is_nil(rootContext)) {
            std::cerr << "Failed to narrow the root naming context." << std::endl;
            return 0;
        }
    }
    catch (CORBA::NO_RESOURCES&) {
        std::cerr << "Caught NO_RESOURCES exception. You must configure omniORB "
            << "with the location" << std::endl
            << "of the naming service." << std::endl;
        return 0;
    }
    catch (CORBA::ORB::InvalidName&) {
        // This should not happen!
        std::cerr << "Service required is invalid [does not exist]." << std::endl;
        return 0;
    }

    try {
        // Bind a context called "test" to the root context:

        CosNaming::Name contextName;
        contextName.length(1);
        contextName[0].id = (const char*)"GameService";       // string copied
        contextName[0].kind = (const char*)""; // string copied

        CosNaming::NamingContext_var testContext;
        try {
            // Bind the context to root.
            testContext = rootContext->bind_new_context(contextName);
        }
        catch (CosNaming::NamingContext::AlreadyBound& ex) {
            // If the context already exists, this exception will be raised.
            // In this case, just resolve the name and assign testContext
            // to the object returned:
            CORBA::Object_var obj = rootContext->resolve(contextName);
            testContext = CosNaming::NamingContext::_narrow(obj);
            if (CORBA::is_nil(testContext)) {
                std::cerr << "Failed to narrow naming context." << std::endl;
                return 0;
            }
        }

        // Bind objref with name Echo to the testContext:
        CosNaming::Name objectName;
        objectName.length(1);
        objectName[0].id = (const char*)"PacmanTest";   // string copied
        objectName[0].kind = (const char*)"Object"; // string copied

        try {
            testContext->bind(objectName, objref);
        }
        catch (CosNaming::NamingContext::AlreadyBound& ex) {
            testContext->rebind(objectName, objref);
        }
        // Note: Using rebind() will overwrite any Object previously bound
        //       to /test/Echo with obj.
        //       Alternatively, bind() can be used, which will raise a
        //       CosNaming::NamingContext::AlreadyBound exception if the name
        //       supplied is already bound to an object.
    }
    catch (CORBA::TRANSIENT& ex) {
        std::cerr << "Caught system exception TRANSIENT -- unable to contact the "
            << "naming service." << std::endl
            << "Make sure the naming server is running and that omniORB is "
            << "configured correctly." << std::endl;

        return 0;
    }
    catch (CORBA::SystemException& ex) {
        std::cerr << "Caught a CORBA::" << ex._name()
            << " while using the naming service." << std::endl;
        return 0;
    }
    return 1;
}


const int TARGET_FPS = 60; 
const int FRAME_DURATION = 1000 / TARGET_FPS; 

void gameLoop(GameService_i* main_server) {
	while (true) {
		auto frameStart = std::chrono::high_resolution_clock::now();

		auto frameEnd = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> frameTime = frameEnd - frameStart;

        std::vector<int> del_server_idx;
        for (auto& server : main_server->_servers) 
        {
            server.update();
            if (server.is_empty())
                del_server_idx.push_back(server._server.server_id);
        }

        for (size_t i = 0; i < main_server->_servers.size(); i++)
        {
            if (std::find(del_server_idx.begin(), del_server_idx.end(), main_server->_servers[i]._server.server_id) != del_server_idx.end())
                main_server->_servers.erase(main_server->_servers.cbegin() + i);
        }

		int sleepTime = FRAME_DURATION - static_cast<int>(frameTime.count());
		if (sleepTime > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
		}
	}
}

int main(int argc, char** argv) {
	try {

		CORBA::ORB_ptr orb = CORBA::ORB_init(argc, argv, "omniORB4");
		CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
		PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);


		PortableServer::Servant_var<GameService_i> gsi = new GameService_i();
		PortableServer::ObjectId_var myechoid = poa->activate_object(gsi);

		obj = gsi->_this();
		CORBA::String_var sior(orb->object_to_string(obj));
		std::cout << sior << std::endl;

		if (!bindObjectToName(orb, obj))
			return 1;

		PortableServer::POAManager_var pman = poa->the_POAManager();
		pman->activate();

		// Block until the ORB is shut down.
		//orb->run();

        std::cout << "Server started!\n";

        gameLoop(gsi);
	}
	catch (CORBA::SystemException& ex) {
		std::cerr << "Caught CORBA::" << ex._name() << std::endl;
	}
	catch (CORBA::Exception& ex) {
		std::cerr << "Caught CORBA::Exception: " << ex._name() << std::endl;
	}
	
	return 0;
}
