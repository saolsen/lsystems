
// BLOG NOTES
// traversal of a tree instead of reifying the sequence is dope and lets you do this without any memory.
// @TODO: Rotations that you can pass in. (right now hardcoded to 90)
// @TODO: Stack for interpreting 
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "raylib.h"
#include "raymath.h"

typedef struct {
    char val;
    char *replacement;
    size_t replacement_len;
} Rule;

#define RULE(c, rep) (Rule){.val=c, .replacement = rep, .replacement_len = strlen(rep)}

typedef struct {
    int level;
    char* production;
    size_t production_len;
    size_t production_pos;
} Frame;

void interpret(char c, Vector2 *pos, Vector2 *dir)
{
    // @BUG: Rotations don't work :( 
    switch (c) {
    case('F'): {
        // draw a line
        Vector2 np = Vector2Add(*pos, *dir);
        Vector2 screen_pos = *pos;
        screen_pos.y = GetScreenHeight() - screen_pos.y;
        Vector2 screen_np = np;
        screen_np.y = GetScreenHeight() - screen_np.y;
        DrawLineEx(screen_pos, screen_np, 3, RED);
        *pos = np;
    } break;
    case('f'): {
        *pos = Vector2Add(*pos, *dir);
    } break;
    case('-'): {
        float sn = 1;
        float cs = 0;
        //float sn = 0;
        //float cs = roundf(cosf(theta) * 100) / 100;
        //float sn = roundf(sinf(theta) * 100) / 100;
        Vector2 nd = { .x = (dir->x * cs) - (dir->y * sn),
            .y = (dir->x * sn) + (dir->y * cs) };
        *dir = nd;
    } break;
    case('+'): {
        float sn = -1;
        float cs = 0;
        //float cs = cosf(theta);
        //float sn = sinf(theta);
        Vector2 nd = { .x = (dir->x * cs) - (dir->y * sn),
            .y = (dir->x * sn) + (dir->y * cs) };
        *dir = nd;
    } break;
    }
}

void lsystem_eval(char* input, Rule *rules, int num_applications)
{
    assert(num_applications <= 26);
    Frame stack[26]; // Stack must be as large as the max lsystem applications.
    int sp = 0;

    Frame current_frame;
    current_frame.level = 0;
    current_frame.production = input;
    current_frame.production_len = strlen(input);
    current_frame.production_pos = 0;

    Vector2 pos = { .x = 0,.y = 0 };
    Vector2 dir = { .x = 0,.y = 10 };
    float angle = 90;

    pos.x = (float)GetScreenWidth() / 2;
    pos.y = (float)GetScreenHeight() / 2;

    for (;;) {
        char c = current_frame.production[current_frame.production_pos++];

        // Check for a rule unless we're in the last level.
        Rule *rule = NULL;
        if (current_frame.level != num_applications) {
            for (Rule *r = rules;
                r->val != 0;
                r++) {
                if (c == r->val) {
                    rule = r;
                    break;
                }
            }
        }

        if (rule) {
            // drop down into rule.
            stack[sp++] = current_frame;
            current_frame.level = current_frame.level++;
            current_frame.production = rule->replacement;
            current_frame.production_len = rule->replacement_len;
            current_frame.production_pos = 0;

        } else {
            interpret(c, &pos, &dir);
            while (current_frame.production_pos == current_frame.production_len) {
                if (sp == 0) {
                    return;
                } else {
                    current_frame = stack[--sp];
                }
            }
        }
    }
}

void debug_draw()
{
    DrawFPS(GetScreenWidth() - 80, 10);
}

int main()
{
    int ScreenWidth = 1024*2;
    int ScreenHeight = 768*2;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(ScreenWidth, ScreenHeight, "lsystems");

    ShowLogo();
    TraceLog(INFO, "screen is %ix%i\n", GetScreenWidth(), GetScreenHeight());

    SetTargetFPS(60);

    Camera2D camera;

    camera.target = (Vector2) { 0, 0 };
    camera.offset = (Vector2) { 0, 0 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    while (!WindowShouldClose())
    {
        // This is sorta shite, I would like zoom aware mouse drag please!

        if (IsKeyDown(KEY_RIGHT)) {
            camera.offset.x -= 2;
        }
        if (IsKeyDown(KEY_LEFT)) {
            camera.offset.x += 2;
        }
        if (IsKeyDown(KEY_UP)) {
            camera.offset.y += 2;
        }
        if (IsKeyDown(KEY_DOWN)) {
            camera.offset.y -= 2;
        }

        // Camera zoom controls
        camera.zoom += ((float)GetMouseWheelMove()*0.05f);

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.1f) camera.zoom = 0.1f;

        //test();
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            char * input = "FX";
            Rule rules[] = {
                RULE('X', "X+YF+"),
                RULE('Y', "-FX-Y"),
                {0,0,0}
            };
            Begin2dMode(camera);

            lsystem_eval(input, rules, 13);

            End2dMode();

            debug_draw();
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}