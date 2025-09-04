#pragma once

#include "ofMain.h"



extern std::shared_ptr<ofAppBaseWindow> g_AppWindow;
extern bool g_AppRunning;
extern ofFbo g_GameVram;



void GameRun();
void GameRender();
