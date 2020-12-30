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

#include <MakeUnique.hpp>
#include <TailDrawable.hpp>
#include <Entity.hpp>
#include <MessageBus.hpp>
#include <Util.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

namespace
{
    const float maxDt = 0.002f;
    const float thickness = 0.16f; //percentage of segment length
    const float largeDistance = 330.f;//prevent drawing stretched tails

    const float fadeTime = 0.5f;

    const float wiggleAmplitude = 3.5f;
}

TailDrawable::TailDrawable(MessageBus& mb)
    : Component     (mb),
    m_fadeTime      (0.f),
    m_currentIndex  (0)
{
    //60 is our fixed step time
    float stepCount = 60.f;// / static_cast<float>(Util::Random::value(2, 3));
    float step = TAU / stepCount;
    for (float i = 0.f; i < stepCount; ++i)
    {
        m_wavetable.push_back(std::sin(step * i) * wiggleAmplitude);
    }
}

//public
Component::Type TailDrawable::type() const
{
    return Component::Type::Drawable;
}

void TailDrawable::entityUpdate(Entity& entity, float dt)
{
    auto position = entity.getWorldPosition();

    setPosition(position);
    auto transform = getTransform();
    auto wiggleOffset = m_wavetable.size() / m_simulations.size();
    for (auto i = 0u; i < m_simulations.size(); ++i)
    {
        auto point = m_simulations[i].second + sf::Vector2f(0.f, m_wavetable[(m_currentIndex + (i * wiggleOffset)) % m_wavetable.size()]);
        m_simulations[i].first->setAnchor(transform.transformPoint(point));
    }
    m_currentIndex = (m_currentIndex + 1) % m_wavetable.size();
    
    int iterCount = static_cast<int>(dt / maxDt) + 1;
    if (iterCount) dt /= iterCount;

    for (int i = 0; i < iterCount; ++i)
    {
        for (auto& sim : m_simulations) sim.first->update(dt);
    }

    m_fadeTime += dt;
}

void TailDrawable::handleMessage(const Message& msg)
{
    if (msg.type == Message::Type::Physics)
    {

    }
}

void TailDrawable::onStart(Entity& entity)
{
    auto position = entity.getWorldPosition();
    for (auto& sim : m_simulations)
    {
        sim.first->setPosition(position + sim.second);
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

        sf::Color colour = m_colour;
        colour.a = static_cast<sf::Uint8>(std::min(m_fadeTime / fadeTime, 1.f) * m_colour.a);

        auto size = masses.size() - 1;
        sf::Vector2f lastVec;
        
        for (auto i = 0u; i < size; ++i)
        {
            position = masses[i]->getPosition() * sim.first->getScale();
            sf::Vector2f vec = (masses[i + 1]->getPosition() - masses[i]->getPosition()) * sim.first->getScale();
            if (Util::Vector::lengthSquared(vec) > largeDistance) return;
            vec *= thickness;
            vec = sf::Vector2f(vec.y, -vec.x); //rotate 90 degrees

            if (i > 0)
            {
                vec += lastVec;
                vec /= 2.f;
            }

            verts.emplace_back(position + vec, colour);
            verts.emplace_back(position - vec, colour);
            lastVec = vec;
        }

        position = masses.back()->getPosition() * sim.first->getScale();
        verts.emplace_back(position + lastVec, colour);
        verts.emplace_back(position - lastVec, colour);

        verts.emplace_back(position + lastVec, sf::Color::Transparent);
        verts.emplace_back(position - lastVec, sf::Color::Transparent);
    }

    rt.draw(verts.data(), verts.size(), sf::TrianglesStrip, sf::BlendAdd);
}
