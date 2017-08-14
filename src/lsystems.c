
// BLOG NOTES
// traversal of a tree instead of reifying the sequence is dope and lets you do this without any memory.
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

#define MIN(x,y) ((x < y) ? x : y)
#define MAX(x,y) ((x > y) ? x : y)

// @TODO: Rotations that you can pass in. (right now hardcoded to 90)
// @TODO: Stack for interpreting 
void interpret(char c, Vector2 *pos, Vector2 *dir, Vector2 *min, Vector2 *max)
{
    // @BUG: Rotations don't work :( 
    switch (c) {
    case('F'): {
        // draw a line
        Vector2 np = Vector2Add(*pos, *dir);
        Vector2 screen_pos = *pos;
        screen_pos.y = /*GetScreenHeight()*/ - screen_pos.y;
        Vector2 screen_np = np;
        screen_np.y = /*GetScreenHeight()*/ - screen_np.y;
        DrawLineEx(screen_pos, screen_np, 3, RED);
        *pos = np;

        min->x = MIN(min->x, pos->x);
        min->y = MIN(min->y, pos->y);
        max->x = MAX(max->x, pos->x);
        max->y = MAX(max->y, pos->y);
    } break;
    case('f'): {
        *pos = Vector2Add(*pos, *dir);
        min->x = MIN(min->x, pos->x);
        min->y = MIN(min->y, pos->y);
        max->x = MAX(max->x, pos->x);
        max->y = MAX(max->y, pos->y);
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

void lsystem_eval(char* input, Rule *rules, int num_applications, Vector2 *min, Vector2 *max)
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
    *min = pos;
    *max = pos;
    float angle = 90;

    //pos.x = (float)GetScreenWidth() / 2;
    //pos.y = (float)GetScreenHeight() / 2;

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
            interpret(c, &pos, &dir, min, max);
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

void debug_draw(Vector2 min, Vector2 center, Vector2 max)
{
    int y = 10;
    //int circle_x = GetScreenWidth() - 175;
    int text_x = GetScreenWidth() - 250;
    int circle_x = text_x - 50;
    DrawFPS(text_x, y);
   /* y += 30;
    DrawCircle(circle_x, y+10, 10, GREEN);
    DrawText(FormatText("(%.2f,%.2f)", min.x, min.y), text_x, y, 30, BLACK);
    y += 30;
    DrawCircle(circle_x, y + 10, 10, ORANGE);
    DrawText(FormatText("(%.2f,%.2f)", center.x, center.y), text_x, y, 30, BLACK);
    y += 30;
    DrawCircle(circle_x, y + 10, 10, BLUE);
    DrawText(FormatText("(%.2f,%.2f)", max.x, max.y), text_x, y, 30, BLACK);

    DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, 5, PINK);*/
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

    Vector2 min = (Vector2) { 0, 0 };
    Vector2 max = (Vector2) { 1, 1 };

    while (!WindowShouldClose())
    {
        // @TODO: Drag the canvas with the mouse.
        // @TODO: Make the zoom relative to the center of the screen.

        //if (IsKeyDown(KEY_RIGHT)) {
        //    camera.offset.x -= 2;
        //}
        //if (IsKeyDown(KEY_LEFT)) {
        //    camera.offset.x += 2;
        //}
        //if (IsKeyDown(KEY_UP)) {
        //    camera.offset.y += 2;
        //}
        //if (IsKeyDown(KEY_DOWN)) {
        //    camera.offset.y -= 2;
        //}

        // Camera zoom controls
        camera.zoom += ((float)GetMouseWheelMove()*0.05f);

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.1f) camera.zoom = 0.1f;

        Vector2 center = Vector2Subtract(max, min);
        Vector2Divide(&center, 2);
        center = Vector2Add(center, min);

        //camera.target = center;
        camera.target = (Vector2) { center.x, -center.y };
        //camera.offset = center;
        camera.offset = (Vector2) {GetScreenWidth()/2 - center.x,GetScreenHeight()/2 + center.y};

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

            lsystem_eval(input, rules, 13, &min, &max);

            //DrawCircle(min.x, -min.y, 10, GREEN);
            //DrawCircle(center.x, -center.y, 10, ORANGE);
            //DrawCircle(max.x, -max.y, 10, BLUE);
            //DrawCircle(0, 0, 10, BLACK);

            End2dMode();

            debug_draw(min, center, max);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}