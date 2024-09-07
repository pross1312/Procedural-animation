#include <raylib.h>
#include <dlfcn.h>
#include "Plug.h"

void *dll = nullptr;
bool load_plug() {
    if (dll != nullptr) dlclose(dll);
    dll = dlopen("./libplug.so", RTLD_NOW);
    if (dll == nullptr) {
        TraceLog(LOG_ERROR, dlerror());
        return false;
    }
    #define PLUG(name, ret, ...) name = (ret (*)(__VA_ARGS__))dlsym(dll, #name);\
    if (name == nullptr) {\
        TraceLog(LOG_ERROR, dlerror());\
        return false;\
    }
    LIST_OF_PLUGS
    #undef PLUG
    return true;
}

int main(int argc, char** argv) {
    TraceLog(LOG_INFO, "W: %s", GetWorkingDirectory());
    TraceLog(LOG_INFO, "A: %s", GetApplicationDirectory());
    if (!load_plug()) {
        TraceLog(LOG_ERROR, "Can't load plug");
        return 1;
    }
    InitWindow(1300, 900, "Fex");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(100, 100);
    SetTargetFPS(60);
    plug_init(argc, argv);
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            void* state = plug_pre_reload();
            if (!load_plug()) {
                TraceLog(LOG_ERROR, "Can't load plug");
                return 1;
            }
            plug_post_reload(state);
        }
        plug_update(GetFrameTime());
    }
    CloseWindow();
    return 0;
}
