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

#include <TailDrawable.hpp>
#include <Entity.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

namespace
{
    const float maxDt = 0.002f;
    const float thickness = 0.16f;
}

TailDrawable::TailDrawable(MessageBus& mb)
    :Component(mb)
{

}

//public
Component::Type TailDrawable::type() const
{
    return Component::Type::Drawable;
}

void TailDrawable::entityUpdate(Entity& entity, float dt)
{
    setPosition(entity.getWorldPosition());
    auto transform = getTransform();
    for (auto& sim : m_simulations)
    {
        sim.first->setAnchor(transform.transformPoint(sim.second));
    }
    
    int iterCount = static_cast<int>(dt / maxDt) + 1;
    if (iterCount) dt /= iterCount;

    for (int i = 0; i < iterCount; ++i)
    {
        for (auto& sim : m_simulations) sim.first->update(dt);
    }
    
}

void TailDrawable::handleMessage(const Message&)
{

}

void TailDrawable::onStart(Entity& entity)
{
    auto position = entity.getWorldPosition();
    for (auto& sim : m_simulations)
    {
        auto& masses = sim.first->getMasses();
        for (auto& m : masses)
        {
            m->setPosition(m->getPosition() + ((position + sim.second) / sim.first->getScale()));
        }
    }
}

void TailDrawable::setColour(const sf::Color& colour)
{
    m_colour = colour;
    m_colour.a /= 2;
}

void TailDrawable::addTail(const sf::Vector2f& relPosition)
{
    m_simulations.emplace_back(std::make_pair(std::make_unique<Simulation>(sf::Vector2f(), sf::Vector2f( -700.f, 0.f )), relPosition));
}

//private
void TailDrawable::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    std::vector<sf::Vertex> verts;

    for (const auto& sim : m_simulations)
    {
        const auto& masses = sim.first->getMasses();
        sf::Vector2f position = masses[0]->getPosition() * sim.first->getScale();

        verts.emplace_back(position, sf::Color::Transparent);
        verts.emplace_back(position, sf::Color::Transparent);

        auto size = masses.size() - 1;
        sf::Vector2f lastVec;
        
        for (auto i = 0u; i < size; ++i)
        {
            position = masses[i]->getPosition() * sim.first->getScale();
            sf::Vector2f vec = (masses[i + 1]->getPosition() - masses[i]->getPosition()) * sim.first->getScale();
            vec *= thickness;
            vec = sf::Vector2f(vec.y, -vec.x); //rotate 90 degrees

            if (i > 0)
            {
                vec += lastVec;
                vec /= 2.f;
            }

            verts.emplace_back(position + vec, m_colour);
            verts.emplace_back(position - vec, m_colour);
            lastVec = vec;
        }

        position = masses.back()->getPosition() * sim.first->getScale();
        verts.emplace_back(position + lastVec, m_colour);
        verts.emplace_back(position - lastVec, m_colour);

        verts.emplace_back(position + lastVec, sf::Color::Transparent);
        verts.emplace_back(position - lastVec, sf::Color::Transparent);
    }

    rt.draw(verts.data(), verts.size(), sf::TrianglesStrip, sf::BlendAdd);
}