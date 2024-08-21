#include <raylib.h>
#include <raymath.h>
#include <vector>

constexpr Color head_color = RED;
constexpr float velocity = 250.0f;
constexpr float radius = 10.0f;
constexpr float distance = 13.0f;
constexpr float max_flexibility = 135.0f/180.0f*PI;// 90 degree
const size_t n_points = 75;
std::vector<Vector2> points;

Vector2 distance_constraint(Vector2 center, Vector2 point, float r) {
    return Vector2Add(center, Vector2Scale(Vector2Normalize(Vector2Subtract(point, center)), r));
}

void init_points() {
    float half_w = GetScreenWidth()/2.0f;
    float half_h = GetScreenHeight()/2.0f;
    points.clear();
    for (size_t i = 0; i < n_points; i++) {
        points.push_back({.x = half_w - distance*i, .y = half_h});
    }
    SetMousePosition(half_w, half_h);
}

extern "C" {
void plug_init(int argc, char** argv) {
    (void)argc, (void)argv;
    TraceLog(LOG_INFO, "-------------PLUG_INIT-------------");
    init_points();
}

void* plug_pre_reload() {
    return nullptr;
}

void plug_post_reload(void *state) {
    (void)state;
    TraceLog(LOG_INFO, "-------------PLUG_RELOAD-------------");
    init_points();
}

void plug_update(float delta) {
    BeginDrawing();

    ClearBackground(GetColor(0x101010ff));
    Vector2 vel {};
    if (IsKeyDown(KEY_S)) {
        vel.y = velocity * delta;
    } else if (IsKeyDown(KEY_W)) {
        vel.y = -velocity * delta;
    } else if (IsKeyUp(KEY_S) || IsKeyUp(KEY_W)) {
        vel.y = 0.0f;
    }
    if (IsKeyDown(KEY_A)) {
        vel.x = -velocity * delta;
    } else if (IsKeyDown(KEY_D)) {
        vel.x = velocity * delta;
    } else if (IsKeyUp(KEY_A) || IsKeyUp(KEY_D)) {
        vel.x = 0.0f;
    }
    points[0] = Vector2Add(points[0], vel);
    points[0] = GetMousePosition();
    for (size_t i = 1; i < points.size(); i++) {
        points[i] = distance_constraint(points[i-1], points[i], distance);
        Vector2 line1 = Vector2Subtract(points[i-1], points[i]);
        Vector2 line2 = Vector2Subtract(points[i+1], points[i]);
        float angle = Vector2Angle(line1, line2);
        if (abs(angle) < max_flexibility) {
            float rotating_angle = max_flexibility - abs(angle);
            // Vector2Rotate and Vector2Angle work clockwise
            // angle < 0 means actual angle > 180 thus need to rotate counter clockwise
            if (angle < 0) rotating_angle = -rotating_angle;
            Vector2 rotated = Vector2Rotate(line2, rotating_angle);
            points[i+1] = Vector2Add(points[i], rotated);
        }
    }
    if (IsKeyPressed(KEY_SPACE)) {
        Vector2 line1 = Vector2Subtract(points[0], points[1]);
        Vector2 line2 = Vector2Subtract(points[2], points[1]);
        float angle = Vector2Angle(line1, line2);
        TraceLog(LOG_INFO, "%f", angle*180.0f/PI);
    }

    DrawSplineLinear(points.data(), points.size(), 4.0f, WHITE);
    DrawCircleV(points[0], radius, head_color);
    EndDrawing();
}
}
