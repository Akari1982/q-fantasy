#include "system/application.h"
#include "kernel/game.h"

#include "ofMain.h"



int main()
{
    ofGLWindowSettings settings;
    settings.setSize(1024, 768);
    settings.setGLVersion(3, 2);
    settings.windowMode = OF_WINDOW;

    auto window = ofCreateWindow(settings);
    window->setWindowTitle("Q-Fantasy v0.01 \"First Steps\"");

    ofRunApp(window, make_shared<Application>());

    GameMain();
}
