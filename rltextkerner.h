#ifndef RLTEXTKERNER
#define RLTEXTKERNER

#ifdef RLTEXTKERNER_IMPLEMENTATION
    #define STB_TRUETYPE_IMPLEMENTATION
#endif
#include "raylib.h"
#include "stb_truetype.h"
#include <assert.h>

#if defined(__cplusplus)
extern "C" {            // Prevents name mangling of functions
#endif

/**
 * Copyright (c) 2024 Michael Mackus
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

typedef struct GlyphWithKerning {
    int index; // glyph index
    int value; // Character value (Unicode)
    int advanceX; // Character advance position X
    int lsb; // Left side bearing for accurate font rendering
    int imageCount; // Number of bitmap images for different font sizes
    Image *images; // Character raw bitmap data for different font sizes
} GlyphWithKerning;

// Font, font texture and GlyphInfo array data
typedef struct FontWithKerning {
    int glyphCount;           // Number of glyph characters
    GlyphWithKerning *glyphs;  // Glyph data for faster bitmap generation
    stbtt_fontinfo *info;     // Font info from stb_truetype
} FontWithKerning;

// Load font from file - only supports TTF or OTF. NOTE: if the info property is NULL in the returned struct, there was
// an error during loading.
FontWithKerning LoadFontWithKerning(const char *fileName, int baseFontSize);

// Load font from file - only supports filetypes TTF or OTF. NOTE: if the info property is NULL in the returned struct,
// there was an error during loading.
FontWithKerning LoadFontWithKerningEx(const char *fileName, int baseFontSize, const int *codepoints, int codepointCount);

// Load font from memory buffer - only supports TTF or OTF. NOTE: if the info property is NULL in the returned struct,
// there was an error during loading.
//
// NOTE: The fileData pointer must remain valid for the life of the font. Calling UnloadFontWithKerning will free this data.
FontWithKerning LoadFontWithKerningFromMemory(const unsigned char *fileData, int baseFontSize, int dataSize, const int *codepoints, int codepointCount);

// Update font with bitmaps for the font size - this way KernText functions operate much faster at that size.
void UpdateFontWithKerningBitmaps(FontWithKerning *font, int fontSize);

// Free the font data
void UnloadFontWithKerning(FontWithKerning font);

Image KernText(const char *text, FontWithKerning font, int fontSize); // Kern text and produce greyscale image.
Image KernTextWrapped(const char *text, FontWithKerning font, int fontSize, int maxWidth); // Kern text word wrapped to a max width (maxWidth = max width in pixels).
Image KernTextEx(const char *text, FontWithKerning font, int fontSize, int maxWidth, int maxHeight, int wrap, int subpixel); // Kern text advanced within maxWidth & maxHeight.
Image KernCodepoints(const int *codepoints, int codepointsCount, FontWithKerning font, int fontSize, int maxWidth, int maxHeight, int wrap, int subpixel); // Kern UTF-8 codepoints (called via the above functions)

#ifdef RLTEXTKERNER_IMPLEMENTATION

Image CreateGlyphImageWithKerning(FontWithKerning font, int codepoint, float fontScale)
{
    Image image = { 0 };
    image.data = stbtt_GetGlyphBitmap(font.info,
                        fontScale,
                        fontScale,
                        stbtt_FindGlyphIndex(font.info, codepoint),
                        &image.width,
                        &image.height,
                        NULL,
                        NULL);
    image.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
    image.mipmaps = 1;

    return image;
}

FontWithKerning LoadFontWithKerning(const char *fileName, int baseFontSize)
{
    return LoadFontWithKerningEx(fileName, baseFontSize, NULL, 0);
}

FontWithKerning LoadFontWithKerningEx(const char *fileName, int baseFontSize, const int *codepoints, int codepointCount)
{
    FontWithKerning font = { 0 };

    if (!IsFileExtension(fileName, ".ttf") && !IsFileExtension(fileName, ".otf")) {
        TraceLog(LOG_WARNING, "FONT: Error loading font (%s) with kerning - file must be either TTF or OTF.", fileName);

        return font;
    }

    // Loading file to memory
    int dataSize = 0;
    unsigned char *fileData = LoadFileData(fileName, &dataSize);

    if (fileData == NULL) {
        TraceLog(LOG_WARNING, "FONT: Error loading file %s", fileName);

        return font;
    }

    return LoadFontWithKerningFromMemory(fileData, baseFontSize, dataSize, codepoints, codepointCount);
}

FontWithKerning LoadFontWithKerningFromMemory(const unsigned char *fileData, int baseFontSize, int dataSize, const int *codepoints, int codepointCount)
{
    FontWithKerning font = { 0 };

    font.info = RL_MALLOC(sizeof(*font.info));
    if (font.info != NULL && stbtt_InitFont(font.info, fileData, 0)) {
        TraceLog(LOG_INFO, "FONT: TTF font TTF info loaded successfully. Kerning enabled: %s", font.info->gpos || font.info->kern ? "true" : "false");
        // load default glyphs
        font.glyphCount = (codepointCount > 0) ? codepointCount : 95;
        font.glyphs = RL_MALLOC(font.glyphCount * sizeof(*font.glyphs));

        if (font.glyphs != NULL) {
            for (int i=0; i < font.glyphCount; i++) {
                int codepoint;
                if (codepoints == NULL) codepoint = i + 32;
                else codepoint = codepoints[i];
                float fontScale = stbtt_ScaleForPixelHeight(font.info, baseFontSize);
                GlyphWithKerning glyph = { 0 };
                glyph.value = codepoint;
                glyph.index = stbtt_FindGlyphIndex(font.info, codepoint);
                stbtt_GetGlyphHMetrics(font.info, glyph.index, &glyph.advanceX, &glyph.lsb);
                glyph.imageCount = 1;
                glyph.images = RL_MALLOC(sizeof(*glyph.images));
                glyph.images[0] = CreateGlyphImageWithKerning(font, codepoint, fontScale);
                font.glyphs[i] = glyph;
            }
            TraceLog(LOG_INFO, "FONT: TTF font glyphs loaded successfully (%i glyphs)", font.glyphCount);
        } else {
            TraceLog(LOG_WARNING, "FONT: Error allocating memory for font glyphs");
        }
    } else {
        TraceLog(LOG_WARNING, "FONT: Error loading TTF font info! Font unusable with kerning.");
        if (font.info) free(font.info);
        font.info = NULL;
    }

    return font;
}

void UpdateFontWithKerningBitmaps(FontWithKerning *font, int fontSize)
{
    float fontScale = stbtt_ScaleForPixelHeight(font->info, fontSize);
    for (int i=0; i<font->glyphCount; i++) {
        GlyphWithKerning *glyph = &font->glyphs[i];
        ++glyph->imageCount;
        Image *images = RL_REALLOC(glyph->images, glyph->imageCount * sizeof(*glyph->images));
        if (images) {
            glyph->images = images;
            glyph->images[glyph->imageCount - 1] = CreateGlyphImageWithKerning(*font, glyph->value, fontScale);
        } else {
            TraceLog(LOG_WARNING, "FONT: Error updating font glyph memory!");
        }
    }
}

void UnloadFontWithKerning(FontWithKerning font)
{
    if (font.glyphs) {
        for (int i=0; i<font.glyphCount; i++) {
            for (int j=0; j<font.glyphs[i].imageCount; j++) {
                UnloadImage(font.glyphs[i].images[j]);
            }
            free(font.glyphs[i].images);
        }
        free(font.glyphs);
    }
    if (font.info->data) free(font.info->data);
    if (font.info) free(font.info);
}

Image KernTextWrapped(const char *text, FontWithKerning font, int fontSize, int maxWidth)
{
    return KernTextEx(text, font, fontSize, maxWidth, GetScreenHeight(), 1, 1);
}

Image KernText(const char *text, FontWithKerning font, int fontSize)
{
    return KernTextEx(text, font, fontSize, GetScreenWidth(), GetScreenHeight(), 0, 1);
}

Image KernTextEx(const char *text, FontWithKerning font, int fontSize, int maxWidth, int maxHeight, int wrap, int subpixel)
{
    int codepointsCount;
    int *codepoints = LoadCodepoints(text, &codepointsCount);
    Image result = KernCodepoints(codepoints, codepointsCount, font, fontSize, maxWidth, maxHeight, wrap, subpixel);
    free(codepoints);

    return result;
}

int GetGlyphIndexWithKerning(FontWithKerning font, int codepoint)
{
    for (int i = 0; i < font.glyphCount; i++) {
        if (font.glyphs[i].value == codepoint) {
            return font.glyphs[i].index;
        }
    }

    return stbtt_FindGlyphIndex(font.info, codepoint);
}

// get glyph from font - index will be 0 if invalid
GlyphWithKerning GetGlyphWithKerning(FontWithKerning font, int codepoint)
{
    for (int i = 0; i < font.glyphCount; i++) {
        if (font.glyphs[i].value == codepoint) {
            return font.glyphs[i];
        }
    }

    return (GlyphWithKerning){ 0 };
}

Image KernCodepoints(const int *codepoints, int codepointsCount, FontWithKerning font, int fontSize, int maxWidth, int maxHeight, int wrap, int subpixel)
{
    assert(font.info);
    assert(maxWidth > 0);
    assert(maxHeight > 0);

    // bitmap for writing the resulting image data
    char *bitmap = RL_CALLOC(maxWidth * maxHeight, sizeof(unsigned char));

    // obtain font metrics
    float fontScale = stbtt_ScaleForPixelHeight(font.info, fontSize);
    int ascent, descent, lineGap, yInc;
    stbtt_GetFontVMetrics(font.info, &ascent, &descent, &lineGap);
    ascent = roundf(ascent * fontScale);
    descent = roundf(descent * fontScale);
    lineGap = roundf(lineGap * fontScale);
    yInc = ascent - descent + lineGap;

    float x = 0;
    int y = 0;
    int i = 0;
    int lastSpaceX = 0;
    int lastSpaceIndex = 0;
    int imageWidth = 0;
    while (i < codepointsCount && y + yInc < maxHeight)
    {
        int codepoint = codepoints[i];

        // lookup glyph indices, fallback to looking up the glyph within the font info
        GlyphWithKerning glyph = GetGlyphWithKerning(font, codepoint);
        if (glyph.index == 0) {
            TraceLog(LOG_WARNING, "FONT: Unable to find glyph for codepoint %d", codepoint);
            GlyphWithKerning glyph = { 0 };
            glyph.value = codepoint;
            glyph.index = stbtt_FindGlyphIndex(font.info, codepoint);
            stbtt_GetGlyphHMetrics(font.info, glyph.index, &glyph.advanceX, &glyph.lsb);
        }

        // handle newline characters
        if (codepoint == '\n') {
            // new line - first reset x to first column and advance y
            lastSpaceX = 0;
            lastSpaceIndex = 0;
            x = 0;
            y += yInc;
        // mark word seen in current x pos
        } else if (codepoint == ' ' || codepoint == '\t') {
            lastSpaceX = x;
            lastSpaceIndex = i;
            if (x < maxWidth) x += glyph.advanceX * fontScale; // conditional to prevent overflow
        // populate glyph with kerning value
        } else {
            // add kerning & advance x
            int kern = 0;
            if (i < codepointsCount - 1) {
                // lookup kerning if two characters side by side
                int glyphNextIndex = GetGlyphIndexWithKerning(font, codepoints[i + 1]);
                kern = stbtt_GetGlyphKernAdvance(font.info, glyph.index, glyphNextIndex);
            }
            float xInc = kern * fontScale + glyph.advanceX * fontScale;

            // compute the glyph dimensions & subpixel shift for the glyph
            int cX1, cY1, cX2, cY2;
            float subpixelShift = x - (float) floor(x);
            if (subpixel) stbtt_GetGlyphBitmapBoxSubpixel(font.info, glyph.index, fontScale, fontScale, subpixelShift, 0, &cX1, &cY1, &cX2, &cY2);
            else stbtt_GetGlyphBitmapBox(font.info, glyph.index, fontScale, fontScale, &cX1, &cY1, &cX2, &cY2);
            int glyphWidth = cX2 - cX1;
            int glyphHeight = cY2 - cY1;

            // calculate offset index in our destination bitmap
            int bitmapOffset = floor(x) + roundf(glyph.lsb * fontScale) + ((y + ascent + cY1) * maxWidth);
            if (ceil(x + xInc) < maxWidth) {
                x = x + xInc;
                if (ceil(x) > imageWidth) imageWidth = ceil(x);

                // find image for corresponding font size in glyph
                unsigned char *glyphBitmap = NULL;
                for (int i=0; i < glyph.imageCount; i++) {
                    if (glyph.images[i].width == glyphWidth && glyph.images[i].height == glyphHeight) {
                        glyphBitmap = glyph.images[i].data;
                        break;
                    }
                }

                // if image wasn't found, generate a new one
                int bitmapAllocated = 0;
                if (glyphBitmap == NULL) {
                    bitmapAllocated = 1;
                    if (subpixel)
                        glyphBitmap = stbtt_GetGlyphBitmapSubpixel(font.info,
                                fontScale,
                                fontScale,
                                subpixelShift,
                                0,
                                glyph.index, NULL, NULL, NULL, NULL);
                    else
                        glyphBitmap = stbtt_GetGlyphBitmap(font.info,
                                fontScale,
                                fontScale,
                                glyph.index, NULL, NULL, NULL, NULL);
                }

                // draw the glyph onto the destination bitmap
                if (glyphBitmap) {
                    int glyphOffset = 0;
                    for (int y = 0; y < glyphHeight; y++) {
                        for (int x = 0; x < glyphWidth; x++) {
                            if (glyphBitmap[glyphOffset + x] != 0) {
                                bitmap[bitmapOffset + x] = glyphBitmap[glyphOffset + x];
                            }
                        }
                        bitmapOffset += maxWidth;
                        glyphOffset += glyphWidth;
                    }
                } else {
                    TraceLog(LOG_WARNING, "FONT: Error generating char bitmap for codepoint: %i", glyph.value);
                }

                if (bitmapAllocated == 1) free(glyphBitmap);
            } else if (wrap != 0) {
                if (lastSpaceX > 0) {
                    // erase last part of bitmap where the last part of the broken word is
                    int bitmapOffset = y*maxWidth;
                    for (int y = 0; y < yInc; y++) {
                        for (int x = lastSpaceX; x < maxWidth; x++) {
                            bitmap[bitmapOffset + x] = 0;
                        }
                        bitmapOffset += maxWidth;
                    }
                    // reset character pointer back to start of last word
                    i = lastSpaceIndex + 1;
                    lastSpaceX = 0;
                    lastSpaceIndex = 0;
                }
                // new line - first reset x to first column and advance y
                x = 0;
                y += yInc;
                continue; // don't advance character index
            }
        }

        ++i;
    }

    Image image = { .data = bitmap,
                    .mipmaps = 1,
                    .format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE,
                    .width = maxWidth,
                    .height = maxHeight };


    // ensure image data is cropped to height & width
    int imageHeight = y + yInc >= maxHeight ? maxHeight : y + yInc;
    ImageCrop(&image, (Rectangle){ 0, 0, imageWidth, image.height = imageHeight });

    return image;
}

#endif

#if defined(__cplusplus)
}
#endif

#endif
