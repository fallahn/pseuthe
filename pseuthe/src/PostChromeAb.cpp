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

#include <PostChromeAb.hpp>

#include <SFML/Graphics/RenderTexture.hpp>

namespace
{
    float accumulatedTime = 0.f;
    const float scanlineCount = 5500.f;
}

PostChromeAb::PostChromeAb()
{
    m_shader.loadFromMemory(Shader::FullPass::vertex, Shader::PostChromeAb::fragment);
}

//public
void PostChromeAb::apply(const sf::RenderTexture& src, sf::RenderTarget& dst)
{
    float windowRatio = static_cast<float>(dst.getSize().y) / static_cast<float>(src.getSize().y);

    //auto& shader = m_shaderResource.get(Shader::Type::PostChromeAb);
    m_shader.setUniform("u_sourceTexture", src.getTexture());
    m_shader.setUniform("u_time", accumulatedTime * (10.f * windowRatio));
    m_shader.setUniform("u_lineCount", windowRatio  * scanlineCount);

    applyShader(m_shader, dst);
}

void PostChromeAb::update(float dt)
{
    accumulatedTime += dt;
}
