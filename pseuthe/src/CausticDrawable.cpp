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

#include <CausticDrawable.hpp>
#include <Util.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace
{
    const int rayCount = 7;
}

CausticDrawable::CausticDrawable(MessageBus& mb)
    : Component     (mb),
    m_vertexArray   (sf::PrimitiveType::Quads)
{
    float rotation = -42.f;
    int maxAngle = 100 / rayCount;
    for (auto i = 0; i < rayCount; ++i)
    {
        rotation += static_cast<float>(Util::Random::value(4, maxAngle));
        m_rays.emplace_back();
        m_rays.back().rotate(rotation);
    }
}

//public
Component::Type CausticDrawable::type() const
{
    return Component::Type::Drawable;
}

void CausticDrawable::entityUpdate(Entity&, float dt)
{
    for (auto& ray : m_rays)
        ray.update(dt);

    updateVertexArray();
}

void CausticDrawable::handleMessage(const Message&)
{}


//private
void CausticDrawable::updateVertexArray()
{
    m_vertexArray.clear();

    for (const auto& ray : m_rays)
    {
        auto transform = ray.getTransform();
        for (const auto& vertex : ray.vertices)
        {
            auto newVert = vertex;
            newVert.position = transform.transformPoint(newVert.position);
            m_vertexArray.append(newVert);
        }
    }
}

void CausticDrawable::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    states.blendMode = sf::BlendAdd;
    rt.draw(m_vertexArray, states);
}


////////////ray class////////////
namespace
{
    const float falloffSize = 100.f;
    const float rayLength = 3000.f;

    const sf::Color rayColour(255u, 250u, 190u, 18u);
}

CausticDrawable::Ray::Ray()
    : m_currentIndex(0)
{
    //create wavetables with random amp / freq to animate vertices
    //60 is our fixed step time
    float stepCount = 60.f / static_cast<float>(Util::Random::value(2, 3));
    float step = TAU / stepCount;
    for (float i = 0.f; i < stepCount; ++i)
    {
        m_wavetable.push_back(std::sinf(step * i));
    }

    m_currentIndex = Util::Random::value(0, static_cast<int>(stepCount) - 1);
    m_amplitude = static_cast<float>(Util::Random::value(3, 10));
    
    //TODO tidy up some of these magic numbers
    vertices.emplace_back(sf::Vector2f(-falloffSize, 0.f), sf::Color::Transparent);
    vertices.emplace_back(sf::Vector2f(-1.f, 0.f), rayColour);
    vertices.emplace_back(sf::Vector2f(-1.f, rayLength), rayColour);
    vertices.emplace_back(sf::Vector2f(-falloffSize, rayLength), rayColour);

    vertices.emplace_back(sf::Vector2f(-1.f, 0.f), rayColour);
    vertices.emplace_back(sf::Vector2f(1.f, 0.f), rayColour);
    vertices.emplace_back(sf::Vector2f(1.f, rayLength), rayColour);
    vertices.emplace_back(sf::Vector2f(-1.f, rayLength), rayColour);

    vertices.emplace_back(sf::Vector2f(1.f, rayLength), rayColour);
    vertices.emplace_back(sf::Vector2f(1.f, 0.f), rayColour);
    vertices.emplace_back(sf::Vector2f(falloffSize, 0.f), sf::Color::Transparent);
    vertices.emplace_back(sf::Vector2f(falloffSize, rayLength), rayColour);
}

void CausticDrawable::Ray::update(float dt)
{
    //animate vertices 0 and 3 based on wave tables, 10 and 11
    const float value = m_wavetable[m_currentIndex] * m_amplitude;
    vertices[0].position.x = -falloffSize + value;
    vertices[3].position.x = vertices[0].position.x;

    vertices[10].position.x = falloffSize - value;
    vertices[11].position.x = vertices[10].position.x;

    m_currentIndex = (m_currentIndex + 1) % m_wavetable.size();
}