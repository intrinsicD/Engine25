//
// Created by alex on 04.12.24.
//

#ifndef ENGINE25_MAINLOOP_H
#define ENGINE25_MAINLOOP_H

#include "CommandDoubleBuffer.h"

namespace Bcg {
    class MainLoop {
    public:
        void Run();
        void Run2();

        void Stop();

        DoubleCommandBuffer begin;     // Engine initialization
        DoubleCommandBuffer prepare_scene;  // Combines begin_scene + update_scene
        DoubleCommandBuffer end_scene;      // Scene cleanup
        DoubleCommandBuffer prepare_render; // Combines begin_render + render_scene
        DoubleCommandBuffer end_render;     // Render cleanup
        DoubleCommandBuffer render_gui;     // Combines begin_gui, render_gui, end_gui
        DoubleCommandBuffer end_gui;     // Combines begin_gui, render_gui, end_gui
        DoubleCommandBuffer end;       // Final loop tasks
    };
}

#endif //ENGINE25_MAINLOOP_H
