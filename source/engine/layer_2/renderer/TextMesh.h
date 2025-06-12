#pragma once

#include "Types.h"
#include "Mesh.h"
#include "Font.h"

class TextMesh
{
    public:
    static Mesh Generate(const Font& font, const std::u32string& text, bool rtl = false);

    private:
    static void CenterMeshPivot(std::vector<Vector3>& verts);
};
