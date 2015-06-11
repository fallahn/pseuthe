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

#include <GradientDrawable.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

namespace
{
    static std::vector<sf::Color> colours =
    { 
        sf::Color(52u, 32u, 102u),
        sf::Color(32u, 37u, 102u),
        sf::Color(32u, 93u, 102u),
        sf::Color(32u, 102u, 74u)
    };

    const float TAU = 3.14159f * 2.f;
    std::vector<sf::Vector2f> createPoints(const sf::Vector2f& centre, int pointCount, float radius)
    {
        std::vector<sf::Vector2f> retVal;
        float increment = TAU / static_cast<float>(pointCount);

        for (float angle = 0.f; angle <= TAU; angle += increment)
            retVal.emplace_back(centre.x + radius * std::cosf(angle), centre.y + radius * std::sinf(angle));

        return retVal;
    }

    const float cycleTime = 50.f; //seconds between each colour
    sf::Color lerp(const sf::Color& a, sf::Color& b, float time)
    {
        sf::Color retVal;
        retVal.r = static_cast<sf::Uint8>(static_cast<float>(b.r - a.r) * time) + a.r;
        retVal.g = static_cast<sf::Uint8>(static_cast<float>(b.g - a.g) * time) + a.g;
        retVal.b = static_cast<sf::Uint8>(static_cast<float>(b.b - a.b) * time) + a.b;
        return retVal;
    }
}

GradientDrawable::GradientDrawable(MessageBus& mb)
    :Component      (mb),
    m_vertexArray   (sf::PrimitiveType::TrianglesFan),
    m_colour        (colours[0]),
    m_currentTime   (0.f),
    m_colourIndexA  (0),
    m_colourIndexB  (1)
{
    sf::Vector2f centre(960.f, 540.f);
    auto points = createPoints(centre, 8, 1920.f);

    m_vertexArray.append({ centre, sf::Color::Black });

    for (const auto& p : points)
        m_vertexArray.append({ p, m_colour });
}

Component::Type GradientDrawable::type() const
{
    return Component::Type::Drawable;
}

void GradientDrawable::entityUpdate(Entity&, float dt)
{
    //cycle colours over time
    m_currentTime += dt;
    if (m_currentTime > cycleTime)
    {
        m_currentTime -= cycleTime;
        m_colourIndexA = (m_colourIndexA + 1) % colours.size();
        m_colourIndexB = (m_colourIndexA + 1) % colours.size();
    }

    const float ratio = m_currentTime / cycleTime;
    m_colour = lerp(colours[m_colourIndexA], colours[m_colourIndexB], ratio);

    for (int i = 1; i < m_vertexArray.getVertexCount(); ++i)
        m_vertexArray[i].color = m_colour;
}

void GradientDrawable::handleMessage(const Message&)
{

}

//private
void GradientDrawable::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    rt.draw(m_vertexArray, states);
}