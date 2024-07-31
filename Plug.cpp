#include <raylib.h>
extern "C" {
void plug_init() {
}

void* plug_pre_reload() {
    return nullptr;
}

void plug_post_reload(void *state) {
    (void)state;
}

void plug_update() {
    BeginDrawing();
    ClearBackground(BLUE);
    EndDrawing();
}
}
