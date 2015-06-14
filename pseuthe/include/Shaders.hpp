/*********************************************************************
Matt Marchant 2015
http://trederia.blogspot.com

pseuthe Zlib license.

This software is provided 'as-is', without any express or
implied warranty. In no event will the authors be held
liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute
it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented;
you must not claim that you wrote the original software.
If you use this software in a product, an acknowledgment
in the product documentation would be appreciated but
is not required.

2. Altered source versions must be plainly marked as such,
and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any
source distribution.
*********************************************************************/

//source for shaders

#ifndef SHADERS_HPP_
#define SHADERS_HPP_

#include <string>

namespace Shader
{
    enum class Type
    {
        PostBrightnessExtract,
        PostDownSample,
        PostGaussianBlur,
        PostAdditiveBlend
    };

    namespace FullPass
    {
        static const std::string vertex =
            "#version 120\n" \
            "void main()\n" \
            "{\n" \
            "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n" \
            "    gl_TexCoord[0] = gl_MultiTexCoord0;\n" \
            "}";
    }

    namespace PostBrightness
    {
        static const std::string fragment =
            "#version 120\n" \
            "uniform sampler2D u_sourceTexture;\n" \
            "const float threshold = 0.35;\n" \
            "const float factor = 4.0;\n" \

            "void main()\n" \
            "{\n" \
            "    vec4 sourceColour = texture2D(u_sourceTexture, gl_TexCoord[0].xy);\n" \
            "    float luminance = sourceColour.r * 0.2126 + sourceColour.g * 0.7152 + sourceColour.b * 0.0722;\n" \
            "    sourceColour *= clamp(luminance - threshold, 0.0 , 1.0) * factor;\n" \
            "    gl_FragColor = sourceColour;"
            "}\n";
    }

    //buns
    namespace PostDownSample
    {
        static const std::string fragment =
            "#version 120\n" \
            "uniform sampler2D u_sourceTexture;\n" \
            "uniform vec2 u_sourceSize;\n" \

            "void main()\n" \
            "{\n" \
            "    vec2 pixelSize = 1.0 / u_sourceSize;\n" \
            "    vec2 texCoords = gl_TexCoord[0].xy;\n" \
            "    vec4 colour = texture2D(u_sourceTexture, texCoords);\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + vec2(1.0, 0.0) * pixelSize);\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + vec2(-1.0, 0.0) * pixelSize);\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + vec2(0.0, 1.0) * pixelSize);\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + vec2(0.0, -1.0) * pixelSize);\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + vec2(1.0, 1.0) * pixelSize);\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + vec2(-1.0, -1.0) * pixelSize);\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + vec2(1.0, -1.0) * pixelSize);\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + vec2(-1.0, 1.0) * pixelSize);\n" \
            "    gl_FragColor = colour / 9.0;\n" \
            "}";
    }

    namespace PostGaussianBlur
    {
        static const std::string fragment =
            "#version 120\n" \
            "uniform sampler2D u_sourceTexture;\n" \
            "uniform vec2 u_offset;\n" \

            "void main()\n" \
            "{\n " \
            "    vec2 texCoords = gl_TexCoord[0].xy;\n" \
            "    vec4 colour = vec4(0.0);\n" \
            "    colour += texture2D(u_sourceTexture, texCoords - 4.0 * u_offset) * 0.0162162162;\n" \
            "    colour += texture2D(u_sourceTexture, texCoords - 3.0 * u_offset) * 0.0540540541;\n" \
            "    colour += texture2D(u_sourceTexture, texCoords - 2.0 * u_offset) * 0.1216216216;\n" \
            "    colour += texture2D(u_sourceTexture, texCoords - u_offset) * 0.1945945946;\n" \
            "    colour += texture2D(u_sourceTexture, texCoords) * 0.2270270270;\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + u_offset) * 0.1945945946;\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + 2.0 * u_offset) * 0.1216216216;\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + 3.0 * u_offset) * 0.0540540541;\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + 4.0 * u_offset) * 0.0162162162;\n" \
            "    gl_FragColor = colour;\n" \
            "}";
    }

    namespace PostAdditiveBlend
    {
        static const std::string fragment =
            "#version 120\n" \
            "uniform sampler2D u_sourceTexture;\n" \
            "uniform sampler2D u_bloomTexture;\n" \

            "void main()\n" \
            "{\n" \
            "    gl_FragColor = texture2D(u_sourceTexture, gl_TexCoord[0].xy) + texture2D(u_bloomTexture, gl_TexCoord[0].xy);\n" \
            "}";
    }
}

#endif //SHADERS_HPP_