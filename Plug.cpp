#include <raylib.h>
#include <raymath.h>
#include <vector>

constexpr Color head_color = RED;
constexpr float velocity = 250.0f;
constexpr float radius = 10.0f;
constexpr float distance = 13.0f;
constexpr float eye_radius = 2.5f;
constexpr float max_flexibility = 135.0f/180.0f*PI;// 90 degree
const size_t n_points = 10;

std::vector<Vector2> vertices;

struct Joint {
    Vector2 pos;
    float radius;
};

std::vector<Joint> joints;;

Vector2 distance_constraint(Vector2 center, Vector2 point, float r) {
    return Vector2Add(center, Vector2Scale(Vector2Normalize(Vector2Subtract(point, center)), r));
}

void init() {
    float half_w = GetScreenWidth()/2.0f;
    float half_h = GetScreenHeight()/2.0f;

    joints.clear();
    for (size_t i = 0; i < n_points; i++) {
        joints.push_back({
            .pos = {.x = half_w - distance*i, .y = half_h},
            .radius = radius,
        });
    }
    joints[0].radius *= 1.4f;
    joints[1].radius *= 1.6f;
    joints[2].radius *= 1.4f;
    joints[3].radius *= 1.2f;
    SetMousePosition(half_w, half_h);

    vertices.clear();
    vertices.reserve(n_points);

    HideCursor();
}

extern "C" {
void plug_init(int argc, char** argv) {
    (void)argc, (void)argv;
    TraceLog(LOG_INFO, "-------------PLUG_INIT-------------");
    init();
}

void* plug_pre_reload() {
    return nullptr;
}

void plug_post_reload(void *state) {
    (void)state;
    TraceLog(LOG_INFO, "-------------PLUG_RELOAD-------------");
    init();
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
    // joints[0].pos = Vector2Add(joints[0].pos, vel);

    joints[0].pos = GetMousePosition();
    vertices.clear();
    for (size_t i = 1; i < joints.size(); i++) {
        joints[i].pos = distance_constraint(joints[i-1].pos, joints[i].pos, distance);
        if (i+1 < joints.size()) {
            Vector2 line1 = Vector2Subtract(joints[i-1].pos, joints[i].pos);
            Vector2 line2 = Vector2Subtract(joints[i+1].pos, joints[i].pos);
            float angle = Vector2Angle(line1, line2);
            if (abs(angle) < max_flexibility) {
                float rotating_angle = max_flexibility - abs(angle);
                // Vector2Rotate and Vector2Angle work clockwise
                // angle < 0 means actual angle > 180 thus need to rotate counter clockwise
                if (angle < 0) rotating_angle = -rotating_angle;
                Vector2 rotated = Vector2Rotate(line2, rotating_angle);
                joints[i+1].pos = Vector2Add(joints[i].pos, rotated);
            }
        }
        Vector2 direction = Vector2Normalize(Vector2Subtract(joints[i-1].pos, joints[i].pos));
        Vector2 left_v = Vector2Rotate(direction, -PI/2.0f);
        Vector2 right_v = Vector2Rotate(direction, PI/2.0f);
        if (i == 1) { // push head first
            float r = joints[i-1].radius;
            const Vector2& pos = joints[i-1].pos;
            Vector2 quarter_left_v = Vector2Scale(Vector2Rotate(direction, -PI/4.0f), r);
            Vector2 quarter_right_v = Vector2Scale(Vector2Rotate(direction, PI/4.0f), r);
            Vector2 head = Vector2Add(pos, Vector2Scale(direction, r));
            vertices.push_back(head);
            vertices.push_back(Vector2Add(pos, quarter_left_v));
            vertices.push_back(Vector2Add(pos, quarter_right_v));
            vertices.push_back(Vector2Add(pos, Vector2Scale(left_v, r)));
            vertices.push_back(Vector2Add(pos, Vector2Scale(right_v, r)));
        }
        vertices.push_back(Vector2Add(joints[i].pos, Vector2Scale(left_v, joints[i].radius)));
        vertices.push_back(Vector2Add(joints[i].pos, Vector2Scale(right_v, joints[i].radius)));
        if (i+1 == joints.size()) {
            Vector2 tail = Vector2Add(joints[i].pos, Vector2Scale(direction, -radius));
            vertices.push_back(tail);
        }
    }

    DrawTriangleStrip(vertices.data(), vertices.size(), GRAY);
    Vector2 eye_offset = Vector2Scale(Vector2Normalize(Vector2Subtract(vertices[3], vertices[4])), 4.5f);
    DrawCircleV(Vector2Subtract(vertices[3], eye_offset), eye_radius, SKYBLUE);
    DrawCircleV(Vector2Add(vertices[4], eye_offset), eye_radius, SKYBLUE);
    // DrawSplineLinear(points.data(), points.size(), 4.0f, WHITE);
    EndDrawing();
}
}
