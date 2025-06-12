#include <locale>
#include <codecvt>
#include "TextMesh.h"
#include "Debug.h"

Mesh TextMesh::Generate(const Font& font, const std::u32string& text, bool rtl)
{
    Mesh mesh;

    if (font.glyphs.empty())
    {
        Debug::LogError("Font has no baked glyphs. Font Name: " + font.name);
    }
    else
    {
        float xOffset = 0.0f;
        std::vector<Vector3> verts;
        std::vector<Vector3> normals;
        std::vector<Vector2> uvs;
        std::vector<int> indices;

        std::u32string displayText = text;
        
        if (rtl)
        {
            // Will need better solution.
            std::reverse(displayText.begin(), displayText.end());
        }

        for (char32_t ch : displayText)
        {
            const GlyphEntry* glyphEntry = nullptr;

            for (const auto& entry : font.glyphs)
            {
                if (entry.codepoint == ch)
                {
                    glyphEntry = &entry;
                    break;
                }
            }

            if (glyphEntry)
            {
                const BakedGlyph& glyph = glyphEntry->data;
                int baseIndex = static_cast<int>(verts.size());

                float minX = std::numeric_limits<float>::max();
                float maxX = std::numeric_limits<float>::lowest();
                float minY = std::numeric_limits<float>::max();
                float maxY = std::numeric_limits<float>::lowest();

                for (const Vector2& pos : glyph.positions)
                {
                    float x = pos.x + xOffset;
                    float y = pos.y;
                    verts.emplace_back(Vector3{ x, y, 0.0f });
                    normals.emplace_back(Vector3{ 0.0f, 0.0f, 1.0f });

                    if (pos.x < minX) minX = pos.x;
                    if (pos.x > maxX) maxX = pos.x;
                    if (pos.y < minY) minY = pos.y;
                    if (pos.y > maxY) maxY = pos.y;
                }

                float width = maxX - minX;
                float height = maxY - minY;

                for (const Vector2& pos : glyph.positions)
                {
                    float u = width > 0.0f ? (pos.x - minX) / width : 0.0f;
                    float v = height > 0.0f ? (pos.y - minY) / height : 0.0f;
                    uvs.emplace_back(Vector2{ u, v });
                }

                for (int idx : glyph.indices)
                {
                    indices.push_back(baseIndex + idx);
                }

                xOffset += glyphEntry->metrics.advance;
            }
            else if (ch == U' ')
            {
                xOffset += 0.2f; // Handle space
            }
        }

        CenterMeshPivot(verts);

        mesh.SetVertices(verts);
        mesh.SetNormals(normals);
        mesh.SetTexCoords(uvs);
        mesh.SetIndices(indices);
    }

    return mesh;
}

void TextMesh::CenterMeshPivot(std::vector<Vector3>& verts)
{
    if (verts.empty() == false)
    {
        Vector3 min = verts[0];
        Vector3 max = verts[0];

        for (const Vector3& v : verts)
        {
            if (v.x < min.x) min.x = v.x;
            if (v.y < min.y) min.y = v.y;
            if (v.z < min.z) min.z = v.z;

            if (v.x > max.x) max.x = v.x;
            if (v.y > max.y) max.y = v.y;
            if (v.z > max.z) max.z = v.z;
        }

        Vector3 center = {
            ( min.x + max.x ) * 0.5f,
            ( min.y + max.y ) * 0.5f,
            ( min.z + max.z ) * 0.5f
        };

        for (Vector3& v : verts)
        {
            v.x -= center.x;
            v.y -= center.y;
            v.z -= center.z;
        }
    }
}