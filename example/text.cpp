#include "rltextkerner.h"

// example using text kerning in C++
int main()
{
    InitWindow(1920, 1080, "raylib test with font kerning");
    SetTraceLogLevel(LOG_DEBUG);

    FontWithKerning bodyFont = LoadFontWithKerning("font/NotoSans-Light.ttf", 32);
    UpdateFontWithKerningBitmaps(&bodyFont, 60);
    if (!bodyFont.info) return 1;
    Image bodyImage = KernTextEx("C++ test", bodyFont, 60, 1820, 4000, 1, 1);
    Texture2D bodyTexture = LoadTextureFromImage(bodyImage);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);

            DrawTexture(bodyTexture, 0, 0, WHITE);
        EndDrawing();
    }

    UnloadFontWithKerning(bodyFont);
    UnloadImage(bodyImage);
    UnloadTexture(bodyTexture);
    CloseWindow();

    return 0;
}
