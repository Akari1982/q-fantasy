#include "ofMain.h"

#include "system/application.h"
#include "system/game.h"



std::shared_ptr<ofAppBaseWindow> g_AppWindow;
bool g_AppRunning = true;



int main()
{
    ofGLWindowSettings settings;
    settings.setSize( 1024, 768 );
    settings.windowMode = OF_WINDOW; //can also be OF_FULLSCREEN

    g_AppWindow = ofCreateWindow( settings );

    ofRunApp( g_AppWindow, make_shared<Application>() );

    GameRun();
}
