#include "ofMain.h"

#include "system/application.h"
#include "kernel/game.h"



int main()
{
    ofGLWindowSettings settings;
    settings.setSize( 1024, 768 );
    settings.setGLVersion( 3, 2 );
    settings.windowMode = OF_WINDOW; //can also be OF_FULLSCREEN

    g_application = ofCreateWindow( settings );

    g_application->setWindowTitle( "Q-Fantasy v0.01 \"First Steps\"" );

    ofRunApp( g_application, make_shared<Application>() );

    GameMain();
}
