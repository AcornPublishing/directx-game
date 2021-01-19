#pragma once

#include "resource.h"

const char cEffect[] = 
    "textureCUBE CubeMapTex;\n"

	"technique Cube\n"
    "{\n"
        "pass P0\n"
        "{\n"
            "Texture[0] = <CubeMapTex>;\n"
            
            "TexCoordIndex[0] = CameraSpaceReflectionVector;\n"
            "TextureTransformFlags[0] = Count3;\n"
        "}\n"
    "}\n";



