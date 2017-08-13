// Setting up this project was a shit load of work. I need a better way.
// I have the raylib project referenced by this one and in the solution. That way it will be linked.
// I have to manually link to glfw3, openal doesn't even seem to be in the thing so I can't like that.
// I have to set includes and stuff so I can get at the source.
// The biggest issue I think is that I had to modify the raylib project to get it to see the right files.
// I also think they are like direct references not relative so this aint portable.
// Honestly vs projects suck a lot anyway.

// BLOG NOTES
// traversal of a GRAPH instead of reifying the sequence is dope and lets you do this without any memory.


// Maybe I should write a premake script that can generate a project that has all the right shit set up. That way I can just get going.
// For now chill on all that and write some code!

// Maybe don't worry about it too much, I'm not really going for a releaseable artifact, just playing with code.

#include "raylib.h"
//#include "raymath.h"
#include "rlgl.h"

int foo = 0;


// you have a string of characters.
// those are transformed to another string of characters.
// those are transformed to another.
// those are drawn.
// you don't need to reify the inbetween representations (or even the last one)
// you need a way to loop over the last one from the first one and a list of the transforms.
// maybe like a lazyness thing, we need to pass outputs through the replacement function n times.
// when everything is just 1 character prefix that aint bad but I dunno if that's always true.
// colors and other codes are suppossed to have multiple so we need to be able to deal with that.
// for char in result
//    interpret it.
// will probably save the last buffer of vertices because it's a precomputed thingy.
// for input character
//   c 
//   for n times through
//       for char in transform(c) do it agian, loop recur or whatever
//   each step generates more stuff.
//   so you just continually iterate through a lot of stuff
//   - internal buffers (what if they fill up?)
//   - just loop all the way each time? (like a depth first traversal)
//   I guess if you think of it as a tree, you do want an in order traversal of the lowest stuff and that's
//   like a depth first search!

// F where F->FF for 3 steps is
// f
// ff
// ffff
// or, as a tree
//       f
//     f   f
//   f  f f  f
// this is like a real breakthrough because I don't have to actually build the graph to traverse it! 

// what is that splice thingy that is going on in the dudes stuff.
// is that about splicing buffers together? Building up big results? That's what I need to do.
typedef struct {
    char val;
    char *replacement;
    char *replacement_len;
} Rule;

typedef struct {
    int level;
    char* production;
    int production_len;
    int production_pos;
} Frame;

void test()
{
    char *input = "FX";

    Rule rules[] = {
        { 'X', "X+YF+" },
        { 'Y', "-FX-Y" },
        // {'F', "FF"},
        // {'X', "ABC"},
        // {'Y', "YY"},
        // {'A', "X"},
        { 0, 0 },
    };

    Frame stack[26]; // Stack must be as large as the max lsystem applications.
    int sp = 0;

    int num_applications = 3;

    Frame current_frame;
    current_frame.level = 0;
    current_frame.production = input;
    current_frame.production_len = 2;
    current_frame.production_pos = 0;

    long sequence_length = 0;

    do {
        if (current_frame.level == num_applications) {
            // Bottom level
            for (int i = 0; i < current_frame.production_len; i++) {
                // @NOTE: You can use the whole production at once instead of by character for printing or whatever too.
                // @NOTE: this is where you DO THE THING.
                sequence_length++;
                TraceLog(INFO, "%c", current_frame.production[i]);
                // @NOTE: if we put another if in here we can get back and have this be an iterate function to call but that's just so dumb...
                // gotta do more research on coroutines in c.
            }
            // Move back up.
            current_frame = stack[--sp];
        } else if (current_frame.production_pos != current_frame.production_len) {
            // Going Down
            char c = current_frame.production[current_frame.production_pos++];
            stack[sp++] = current_frame;

            current_frame.level = current_frame.level + 1;
            current_frame.production_pos = 0;
            switch (c) {
            case 'X':
                current_frame.production = "X+YF+";
                current_frame.production_len = 5;
                break;
            case 'Y':
                current_frame.production = "-FX-Y";
                current_frame.production_len = 5;
                // @TODO: default case where there's no production
            }
        } else {
            //going up
            current_frame = stack[--sp];
        }
    } while (sp > 0);

    TraceLog(INFO, "Sequence Length is %i", sequence_length);

    // if level = num_applications that means we at bottom of the stack.
    //    iterate through the production.
    //    if we're done, pop the frame off the stack and set it to the last one.

    // if level < num_applications
    //    c = production[len]
    //    pos++;
    //    if we're done
    //      pop
    //    else
    //        push the current_frame on the stack
    //        create new frame
    //           level = level + 1
    //           production = rule(c)
    //           production_len = len(production)
    //           production_pos = 0
}

void draw()
{

    //DrawText("What a beauty! holy frack!", 200, 300, 48, DARKGREEN);
    DrawFPS(GetScreenWidth() - 80, 10);
    //DrawRectangle(200, 200, 100, 100, DARKBLUE);
    DrawLineEx((Vector2) { 0, 0 }, (Vector2) { 100, 100 }, 3, RED);
    //DrawText(FormatText("Hello %s %i", "steben", foo), 300, 300, 32, GREEN);
}

int main()
{
    int ScreenWidth = 1024;
    int ScreenHeight = 768;

    InitWindow(ScreenWidth, ScreenHeight, "lsystems");

    ShowLogo();
    TraceLog(INFO, "screen is %ix%i\n", GetScreenWidth(), GetScreenHeight());

    SetTargetFPS(60);
    test();

    while (!WindowShouldClose())
    {
        //test();
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            draw();
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}