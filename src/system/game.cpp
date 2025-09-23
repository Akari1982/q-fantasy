#include "game.h"
#include "kernel/system.h"



void GameRun()
{
    SystemMain();
}



void GameRender()
{
    if( g_AppWindow->getWindowShouldClose() )
    {
        std::exit( EXIT_SUCCESS );
    }

    ofGetMainLoop()->loopOnce();
    ofGetMainLoop()->pollEvents();
}
