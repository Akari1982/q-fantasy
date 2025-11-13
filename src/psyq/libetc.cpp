#include "libetc.h"
#include "system/logger.h"
#include "psyq/psxgpu.h"
#include "ofUtils.h"
#include <thread>



bool l_thread = false;
std::thread l_vsync_thread;
void (*l_vsync_callback)() = nullptr;



void ThreadMain()
{
    static uint64_t last_update_micros = ofGetElapsedTimeMicros();
    static uint64_t counter_micros = 0;
    static uint64_t vsync_ntsc = (uint64_t)(1000000.0 / 60.0);

    while (l_thread == true)
    {
        uint64_t now = ofGetElapsedTimeMicros();
        uint64_t delta = now - last_update_micros;
        last_update_micros = now;
        counter_micros += delta;

        if ((counter_micros >= vsync_ntsc) && (g_gpu_vsync == true))
        {
            g_gpu_vsync = false;
            counter_micros -= vsync_ntsc;
            if (l_vsync_callback) l_vsync_callback();
        }

        std::this_thread::sleep_for (std::chrono::microseconds(1));
    }
}



void PsyqStopCallback()
{
    l_thread = false;
    if (l_vsync_thread.joinable()) l_vsync_thread.join();
}



void PsyqResetCallback()
{
    atexit(PsyqStopCallback);

    l_thread = true;
    l_vsync_thread = std::thread(ThreadMain);
    if (!l_vsync_thread.joinable())
    {
        LOG_ERROR("VSync callback thread failed to created.");
    }
    else
    {
        LOG_INFO("VSync callback thread was created.");
    }
}



void PsyqVsyncCallback(void (*func)())
{
    l_vsync_callback = func;
}
