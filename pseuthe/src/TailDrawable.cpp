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
}

TailDrawable::TailDrawable(MessageBus& mb)
    :Component(mb),
    m_simulation({0.f, 0.f}, {-790.f, 0.f})
{

}

//public
Component::Type TailDrawable::type() const
{
    return Component::Type::Drawable;
}

void TailDrawable::entityUpdate(Entity& entity, float dt)
{
    //set simluation's anchor based on ent position
    m_simulation.setAnchor(entity.getWorldPosition());

    int iterCount = static_cast<int>(dt / maxDt) + 1;
    if (iterCount) dt /= iterCount;

    for (int i = 0; i < iterCount; ++ i)
        m_simulation.update(dt);
}

void TailDrawable::handleMessage(const Message&)
{

}

void TailDrawable::onStart(Entity& entity)
{
    auto position = entity.getWorldPosition();
    auto& masses = m_simulation.getMasses();
    for (auto& m : masses)
    {
        m->setPosition(m->getPosition() + (position / m_simulation.getScale()));
    }
}

//private
void TailDrawable::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    std::vector<sf::Vertex> verts;
    const auto& masses = m_simulation.getMasses();
    for (const auto& m : masses)
    {
        verts.emplace_back(m->getPosition() * m_simulation.getScale());
    }

    rt.draw(verts.data(), verts.size(), sf::LinesStrip);
}