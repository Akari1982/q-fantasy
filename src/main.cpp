#include "system/application.h"
#include "system/logger.h"
#include "kernel/game.h"

#include "ofMain.h"



std::thread l_game_thread;



int main()
{
    ofGLWindowSettings settings;
    settings.setSize(1024, 768);
    settings.setGLVersion(3, 2);
    settings.windowMode = OF_WINDOW;

    auto window = ofCreateWindow(settings);
    window->setWindowTitle("Q-Fantasy v0.01 \"First Steps\"");

    ofRunApp(window, make_shared<Application>());

    // Start game thread
    // Game will run at separate thread and may pause, create it's own threads.
    // Main thread will handle render stuff and imGui debug windows
    l_game_thread = std::thread(GameMain);
    if (!l_game_thread.joinable())
    {
        LOG_ERROR("GameMain thread failed to created.");
    }
    else
    {
        LOG_INFO("GameMain thread was created.");
    }

    // Run main loop that will handle all rendering and buttons updates
    ofRunMainLoop();

    // Main function can't exit correctly so detach it and system will kill
    // thread after main programm finishes.
    if (l_game_thread.joinable()) l_game_thread.detach();
}
