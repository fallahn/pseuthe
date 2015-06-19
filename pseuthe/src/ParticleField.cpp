/*********************************************************************
Matt Marchant 2014 - 2015
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

#include <ParticleField.hpp>
#include <Util.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

namespace
{
    const sf::Uint16 maxParticles = 100;
    const int maxVelLength = 40;
    const float defaultSize = 16.f;
    const float alpha = 150.f;
    const float colour = 190.f;
}

ParticleField::ParticleField(const sf::FloatRect& bounds, MessageBus& mb)
    : Component (mb),
    m_blendMode (sf::BlendAdd),
    m_bounds    (bounds),
    m_vertices  (sf::Quads),
    m_texture   (nullptr)
{
    sf::Vector2f defaultVelocity;
    defaultVelocity.x = static_cast<float>(Util::Random::value(-maxVelLength, maxVelLength));
    defaultVelocity.y = static_cast<float>(Util::Random::value(-maxVelLength, maxVelLength));

    //create particles
    for (auto i = 0u; i < maxParticles; ++i)
    {
        const float scale = static_cast<float>(Util::Random::value(2, 10)) / 10.f;
        const sf::Uint8 colourByte = static_cast<sf::Uint8>(scale * colour);
        
        Particle p;
        p.colour = { colourByte, colourByte, colourByte };
        p.colour.a = static_cast<sf::Uint8>(scale * alpha);
        p.rotation = static_cast<float>(Util::Random::value(-120, 120));
        p.velocity = scale * scale * defaultVelocity;
        p.setScale(scale, scale);
        //ew. casts. but random floats don't appear as well distributed
        p.setPosition(static_cast<float>(Util::Random::value((int)bounds.left, (int)bounds.left + (int)bounds.width)),
                    static_cast<float>(Util::Random::value((int)bounds.top, (int)bounds.top + (int)bounds.height)));
        p.setRotation(static_cast<float>(Util::Random::value(0, 360)));
        m_particles.push_back(p);
    }
}

//public
Component::Type ParticleField::type() const
{
    return Component::Type::Drawable;
}

void ParticleField::entityUpdate(Entity&, float dt)
{
    //update particles
    for (auto& p : m_particles)
    {
        p.move(p.velocity * dt);
        p.rotate(p.rotation * dt);

        auto position = p.getPosition();
        if (!m_bounds.contains(position))
        {
            float moveX = 0.f;
            float moveY = 0.f;

            if (position.x > (m_bounds.left + m_bounds.width))
                moveX = -m_bounds.width;
            else if (position.x < m_bounds.left)
                moveX = m_bounds.width;

            if (position.y >(m_bounds.top + m_bounds.height))
                moveY = -m_bounds.height;
            else if (position.y < m_bounds.top)
                moveY = m_bounds.height;

            p.move(moveX, moveY);
        }

        p.velocity = Util::Vector::rotate(p.velocity, -2.f * dt);
    }

    //rebuild vertex array
    updateVertices();
}

void ParticleField::handleMessage(const Message& msg)
{
    //TODO update the velocity or somesuch?
}

void ParticleField::setBlendMode(sf::BlendMode mode)
{
    m_blendMode = mode;
}

void ParticleField::setTexture(sf::Texture& t)
{
    //currently we expect a texture with fixed
    //size subrects of 16px (because sfml texcoords works in pixels *sigh*)
    for (auto& p : m_particles)
    {
        p.textureRect.left = Util::Random::value(0, 3) * defaultSize;
        p.textureRect.top = Util::Random::value(0, 3) * defaultSize;
        p.textureRect.width = defaultSize;
        p.textureRect.height = defaultSize;
    }
    m_texture = &t;
}

//private
void ParticleField::addVertex(const sf::Vector2f& position, float u, float v, const sf::Color& colour)
{
    sf::Vertex vert;
    vert.position = position;
    vert.texCoords = { u, v };
    vert.color = colour;

    m_vertices.append(vert);
}

void ParticleField::updateVertices()
{
    sf::Vector2f halfSize(defaultSize / 2.f, defaultSize / 2.f);

    m_vertices.clear();
    for (auto& p : m_particles)
    {
        auto t = p.getTransform();
        addVertex(t.transformPoint(-halfSize.x, -halfSize.y), p.textureRect.left, p.textureRect.top, p.colour);
        addVertex(t.transformPoint(halfSize.x, -halfSize.y), p.textureRect.left + p.textureRect.width, p.textureRect.top, p.colour);
        addVertex(t.transformPoint(halfSize), p.textureRect.left + p.textureRect.width, p.textureRect.top + p.textureRect.height, p.colour);
        addVertex(t.transformPoint(-halfSize.x, halfSize.y), p.textureRect.left, p.textureRect.top + p.textureRect.height, p.colour);
    }
}

void ParticleField::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    states.texture = m_texture;
    rt.draw(m_vertices, states);
}