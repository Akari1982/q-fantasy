#include "game.h"
#include "field/field.h"



void
GameRun()
{
    field_main();
}



void
GameRender()
{
    if( g_AppWindow->getWindowShouldClose() ) g_AppRunning = false;

    ofGetMainLoop()->loopOnce();
    ofGetMainLoop()->pollEvents();
}
