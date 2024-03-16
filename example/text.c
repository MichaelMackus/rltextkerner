#include <stdio.h>
#include "raylib.h"

#define RLTEXTKERNER_IMPLEMENTATION
#include "rltextkerner.h"

// example using text kerning in C
int main()
{
    InitWindow(1920, 1080, "raylib test with font kerning");
    SetTraceLogLevel(LOG_DEBUG);

    FontWithKerning bodyFont = LoadFontWithKerning("font/NotoSans-Light.ttf", 32);
    UpdateFontWithKerningBitmaps(&bodyFont, 60);
    if (!bodyFont.info) return 1;
    Image bodyImage = KernTextEx("AVATAR\n\ntestingareallyreallyreallylonglinetestingareallyreallyreallylonglinetestingareallyreallyreallylonglinetestingareallyreallyreallylonglinetestingareallyreallyreallylonglinetestingareallyreallyreallylonglinetestingareallyreallyreallylonglinetestingareallyreallyreallylonglinetestingareallyreallyreallylonglinetestingareallyreallyreallylonglinetestingareallyreallyreallylonglinetestingareallyreallyreallylongline\n\nA C looks kinda weird, because the C has this curve that can usually fit quite snugly into the slope of the A like so: AC. Same thing goes for VA or WA; there's this nice parallel between the W and the A that would otherwise be an unsightly void.\n\nLorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris semper tellus ante, in consectetur lacus pretium in. Sed vel semper leo. Ut non nunc vitae tellus sollicitudin elementum. Nunc tempus consectetur urna, sit amet consectetur justo fermentum at. Cras pulvinar pretium felis, a efficitur leo condimentum id. Vivamus ex risus, tristique sed pretium eu, mollis ut nisl. Donec tincidunt sed tortor ac sollicitudin. Morbi consectetur posuere ligula non pretium.\n\nDonec dignissim urna eget nisl gravida mattis. Suspendisse mattis ornare porttitor. Nam varius blandit sapien vel porta. Fusce in elit volutpat, placerat erat ut, tempus lectus. In iaculis nisi at imperdiet varius. Integer dapibus egestas lobortis. Vivamus vel ultricies ante. Nunc dictum quis neque nec consequat. Morbi sed orci a dui rutrum ultrices. Integer porttitor massa ut nisl imperdiet elementum. Aliquam quis ex in nibh pellentesque commodo at in neque. Proin at lacinia tortor. Sed ultricies mauris ut mollis tincidunt. In condimentum lorem enim, in maximus orci lobortis id.\n\nInteger facilisis lobortis egestas. Maecenas urna odio, auctor sit amet nunc sit amet, faucibus congue purus. Duis fermentum imperdiet luctus. Sed ullamcorper, ligula ac congue posuere, neque lectus fermentum lacus, in vehicula nunc felis nec dolor. Cras a erat accumsan, dignissim massa nec, tincidunt nisl. Praesent nibh purus, consectetur mattis enim sed, rutrum rhoncus arcu. Quisque semper urna ac enim vehicula feugiat. Duis posuere, sem a volutpat congue, nisi metus dignissim metus, in blandit purus urna et ligula. Proin vel tellus nibh.\n\nMauris ex nisi, sodales ut iaculis nec, gravida at purus. Proin ultrices ultricies erat et eleifend. Fusce vulputate congue dui, at convallis lacus efficitur non. Nulla ac iaculis augue. Fusce blandit nec sapien in mollis. Vivamus et justo ultrices nulla euismod mollis. Duis faucibus tincidunt ipsum et rhoncus. Etiam varius, mi eget rutrum congue, tortor nisi interdum ipsum, sit amet vulputate diam tortor ut est. Nunc sed odio a diam sagittis fermentum. Mauris varius arcu non eleifend tincidunt. Sed dictum, elit feugiat commodo fermentum, lorem ante aliquet dolor, vel bibendum erat ante eu neque. Curabitur lectus arcu, gravida nec arcu eu, sagittis mollis felis. Morbi auctor tempor nisi non interdum. Phasellus a libero sed justo vestibulum ullamcorper vel vel turpis.\n\nInteger blandit lectus rutrum nulla fringilla, non malesuada ex condimentum. Fusce malesuada quam ut bibendum dapibus. Fusce ullamcorper accumsan aliquet. Proin nec leo congue, laoreet tortor et, faucibus sapien. Donec rhoncus sit amet turpis eu hendrerit. Interdum et malesuada fames ac ante ipsum primis in faucibus. Sed vulputate magna eget fringilla maximus. Quisque fermentum lacus nec orci maximus, sed bibendum est commodo. In hac habitasse platea dictumst. Praesent et leo faucibus, laoreet justo a, aliquam leo. Integer libero diam, mattis nec elit vitae, varius ultrices turpis. Vestibulum iaculis leo ex, quis hendrerit ante placerat a. Proin vel nisi a leo eleifend porta quis sit amet libero. Sed id neque eu felis fringilla congue.", bodyFont, 60, 1820, 4000, 1);
    Texture2D bodyTexture = LoadTextureFromImage(bodyImage);

    int dataSize = 0;
    unsigned char *fileData = LoadFileData("font/DejaVuSans.ttf", &dataSize);
    int unicodeCodepoints[1] = { 9765 };
    FontWithKerning unicodeFont = LoadFontWithKerningFromMemory(fileData, 60, dataSize, unicodeCodepoints, 1);
    if (!unicodeFont.info) return 1;
    Image unicodeImage = KernText("☥", unicodeFont, 60);
    Texture2D unicodeTexture = LoadTextureFromImage(unicodeImage);

    SetTargetFPS(60);

    int yOffset = 0;
    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_DOWN)) {
            yOffset -= 10;
        }
        if (IsKeyDown(KEY_UP)) {
            yOffset += 10;
        }
        if (GetMouseWheelMove()) {
            yOffset += GetMouseWheelMove() * 30;
        }

        BeginDrawing();
            ClearBackground(BLACK);

            DrawTexture(bodyTexture, 0, yOffset, WHITE);
            DrawTexture(unicodeTexture, 150, yOffset, WHITE);
        EndDrawing();
    }

    /* UnloadFontWithKerning(headerFont); */
    UnloadFontWithKerning(bodyFont);
    UnloadFontWithKerning(unicodeFont);

    return 0;
}
