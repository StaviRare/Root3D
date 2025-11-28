#pragma once

#include <vector>
#include <unordered_map>
#include "Vector2.h"

struct BakedGlyph
{
    std::vector<int> indices;
    std::vector<Vector2> positions;
};

struct GlyphMetrics
{
    float advance = 0.0f;
};

struct GlyphEntry
{
    char32_t codepoint;
    BakedGlyph data;
    GlyphMetrics metrics;
};

struct Font
{
    std::string name;
    std::vector<GlyphEntry> glyphs;
};
