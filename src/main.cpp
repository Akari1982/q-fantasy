#include "ofMain.h"

#include "system/application.h"
#include "kernel/game.h"



int main()
{
    ofGLWindowSettings settings;
    settings.setSize( 1024, 768 );
    settings.windowMode = OF_WINDOW; //can also be OF_FULLSCREEN

    g_application = ofCreateWindow( settings );

    ofRunApp( g_application, make_shared<Application>() );

    GameMain();
}
