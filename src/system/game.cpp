#include "game.h"
#include "kernel/system.h"



void GameRun()
{
    system_main();
}



void GameRender()
{
    if( g_AppWindow->getWindowShouldClose() ) g_AppRunning = false;

    ofGetMainLoop()->loopOnce();
    ofGetMainLoop()->pollEvents();
}
