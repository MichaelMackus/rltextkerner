#include <stdio.h>
#include "raylib.h"

#define RLTEXTKERNER_IMPLEMENTATION
#include "rltextkerner.h"

// example using native raylib text drawing functions (i.e. without kerning)
int main()
{
    InitWindow(1920, 1080, "raylib test with font kerning");
    SetTraceLogLevel(LOG_DEBUG);

    Font bodyFont = LoadFontEx("font/NotoSans-Light.ttf", 60, NULL, 0);

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);

            DrawTextEx(bodyFont, "AVATAR", (Vector2){ 0, 0 }, 60, 0, WHITE);
            DrawTextEx(bodyFont, "A C looks kinda weird, because the C has this curve that can usually fit quite snugly into the slope", (Vector2){ 0, 100 }, 60, 0, WHITE);
            DrawTextEx(bodyFont, "of the A like so: AC. Same thing goes for VA or WA; there's this nice parallel between the W and the", (Vector2){ 0, 160 }, 60, 0, WHITE);
            DrawTextEx(bodyFont, "A like so: AC. Same thing goes for VA or WA; there's this nice parallel between the W and the A that", (Vector2){ 0, 220 }, 60, 0, WHITE);
            DrawTextEx(bodyFont, "would otherwise be an unsightly void.", (Vector2){ 0, 280 }, 60, 0, WHITE);
        EndDrawing();
    }

    UnloadFont(bodyFont);
    CloseWindow();

    return 0;
}
