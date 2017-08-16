#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "raylib.h"
#include "raymath.h"

typedef struct {
    char val;
    char *replacement;
    size_t replacement_len;
} Rule;

#define RULE(c, rep) (Rule){.val=c, .replacement = rep, .replacement_len = strlen(rep)}

#define MIN(x,y) ((x < y) ? x : y)
#define MAX(x,y) ((x > y) ? x : y)
#define PI 3.1415926535

typedef struct {
    Vector2 pos;
    Vector2 dir;
} TurtleFrame;

typedef struct {
    TurtleFrame frame;
    TurtleFrame stack[32];
    int sp;
    Vector2 min;
    Vector2 max;
} TurtleState;

// @TODO: Color codes.
void turtle_interpret(TurtleState *turtle, float angle, char c)
{
    if (turtle->frame.dir.x == 0 && turtle->frame.dir.y == 0) {
        turtle->frame.dir.y = 10;
    }

    switch (c) {
    case('['): {
        // push turtle state
        assert(turtle->sp < 32);
        turtle->stack[turtle->sp++] = turtle->frame;
    } break;
    case(']'): {
        // pop turtle state
        assert(turtle->sp > 0);
        turtle->frame = turtle->stack[--turtle->sp];
    } break;
    case('F'): {
        // draw line
        Vector2 np = Vector2Add(turtle->frame.pos, turtle->frame.dir);
        Vector2 screen_pos = turtle->frame.pos;
        screen_pos.y = -screen_pos.y;
        Vector2 screen_np = np;
        screen_np.y = -screen_np.y;
        DrawLineEx(screen_pos, screen_np, 3, BLACK);
        turtle->frame.pos = np;

        turtle->min.x = MIN(turtle->min.x, turtle->frame.pos.x);
        turtle->min.y = MIN(turtle->min.y, turtle->frame.pos.y);
        turtle->max.x = MAX(turtle->max.x, turtle->frame.pos.x);
        turtle->max.y = MAX(turtle->max.y, turtle->frame.pos.y);
    } break;
    case('f'): {
        // skip line
        turtle->frame.pos = Vector2Add(turtle->frame.pos, turtle->frame.dir);

        turtle->min.x = MIN(turtle->min.x, turtle->frame.pos.x);
        turtle->min.y = MIN(turtle->min.y, turtle->frame.pos.y);
        turtle->max.x = MAX(turtle->max.x, turtle->frame.pos.x);
        turtle->max.y = MAX(turtle->max.y, turtle->frame.pos.y);
    } break;
    case('-'): {
        // rotate clockwise
        float theta = angle * PI / 180;
        float cs = cos(theta);
        float sn = sin(theta);
       
        Vector2 nd = { .x = (turtle->frame.dir.x * cs) - (turtle->frame.dir.y * sn),
                       .y = (turtle->frame.dir.x * sn) + (turtle->frame.dir.y * cs) };
        turtle->frame.dir = nd;
    } break;
    case('+'): {
        // rotate counterclockwise
        float theta = -angle * PI / 180;
        float cs = cos(theta);
        float sn = sin(theta);

        Vector2 nd = { .x = (turtle->frame.dir.x * cs) - (turtle->frame.dir.y * sn),
                       .y = (turtle->frame.dir.x * sn) + (turtle->frame.dir.y * cs) };
        turtle->frame.dir = nd;
    } break;
    }
}

#define MAX_DEPTH 26

void lsystem_eval(char* input, Rule *rules, float angle, int num_applications, Vector2 *min, Vector2 *max)
{
    assert(num_applications <= 26);

    TurtleState turtle = {0};

    typedef struct {
        int level;
        char* production;
        size_t production_len;
        size_t production_pos;
    } EvalFrame;

    EvalFrame stack[MAX_DEPTH];
    int sp = 0;

    assert(num_applications <= MAX_DEPTH);

    EvalFrame current_frame;
    current_frame.level = 0;
    current_frame.production = input;
    current_frame.production_len = strlen(input);
    current_frame.production_pos = 0;

    for (;;) {
        char c = current_frame.production[current_frame.production_pos++];

        // check for a rule unless we're in the last level.
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
            // push into rule
            stack[sp++] = current_frame;
            current_frame.level = current_frame.level++;
            current_frame.production = rule->replacement;
            current_frame.production_len = rule->replacement_len;
            current_frame.production_pos = 0;

        } else {
            turtle_interpret(&turtle, angle, c);
            while (current_frame.production_pos == current_frame.production_len) {
                if (sp == 0) {
                    // done with input
                    *min = turtle.min;
                    *max = turtle.max;
                    return;
                } else {
                    // pop out of rule
                    current_frame = stack[--sp];
                }
            }
        }
    }
}

void debug_draw(Vector2 min, Vector2 center, Vector2 max)
{
    int y = 10;
    int text_x = GetScreenWidth() - 250;
    DrawFPS(text_x, y);
}

// Examples
typedef struct {
    char *input;
    Rule rules[10];
    float angle;
    int steps;
} Example;

int main()
{
    Example tree_1 = {
        .input = "FX",
        .rules = {
            RULE('X', "F - [[X] + X] + F[+FX] - X"),
            RULE('F', "FF"),
            {0,0,0}},
        .angle = 25,
        .steps = 6
    };

    Example tree_2 = {
        .input = "FX",
        .rules = {
        RULE('F', "FF-[-F+F]+[+F-F]"),
        RULE('X', "FF+[+F]+[-F]"),
        { 0,0,0 } },
        .angle = 25,
        .steps = 4
    };

    Example dragon_curve = {
        .input = "FX",
        .rules = {
        RULE('X', "X+YF+"),
        RULE('Y', "-FX-Y"),
        { 0,0,0 } },
        .angle = 90,
        .steps = 13
    };

    Example diamond_thingy = {
        .input = "L--F--L--F",
        .rules = {
        RULE('L', "+R-F-R+"),
        RULE('R', "-L+F+L-"),
        { 0,0,0 } },
        .angle = 45,
        .steps = 10
    };

    Example triangle_thingy = {
        .input = "-F",
        .rules = {
        RULE('F', "F+F-F-F+F"),
        { 0,0,0 } },
        .angle = 90,
        .steps = 5
    };

    Example one_with_skips = {
        .input = "F+F+F+F",
        .rules = {
        RULE('F', "F+f-FF+F+FF+Ff+FF-f+FF-F-FF-Ff-FFF"),
        RULE('f', "ffffff"),
        { 0,0,0 } },
        .angle = 90,
        .steps = 2
    };

    Example star = {
        .input = "F-F-F-F-F",
        .rules = {
        RULE('F', "F-F++F+F-F-F"),
        { 0,0,0 } },
        .angle = 72,
        .steps = 4
    };
   
    Example example = one_with_skips;

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
        // Camera zoom controls
        camera.zoom += ((float)GetMouseWheelMove()*0.05f);

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.1f) camera.zoom = 0.1f;

        Vector2 center = Vector2Subtract(max, min);
        Vector2Divide(&center, 2);
        center = Vector2Add(center, min);

        camera.target = (Vector2) { center.x, -center.y };
        camera.offset = (Vector2) {GetScreenWidth()/2 - center.x,GetScreenHeight()/2 + center.y};

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            Begin2dMode(camera);

            lsystem_eval(example.input, example.rules, example.angle, example.steps, &min, &max);

            End2dMode();

            debug_draw(min, center, max);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}