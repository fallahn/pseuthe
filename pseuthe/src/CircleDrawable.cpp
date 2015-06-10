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

#include <CircleDrawable.hpp>
#include <Entity.hpp>
#include <MessageBus.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

namespace
{
    const float minEchoTime = 0.3f;
}

CircleDrawable::CircleDrawable(float radius, MessageBus& m)
    : Component     (m),
    m_circleShape   (radius),
    m_echo          (nullptr),
    m_timeSinceEcho (minEchoTime)
{
    m_circleShape.setOrigin(radius, radius);
}

//public
Component::Type CircleDrawable::type() const
{
    return Component::Type::Drawable;
}

void CircleDrawable::entityUpdate(Entity& parent, float dt)
{
    //modify colours and things based on entity properties

    if (m_echo)
    {
        parent.addComponent<EchoDrawable>(m_echo);
    }

    m_timeSinceEcho -= dt;
}

void CircleDrawable::handleMessage(const Message& msg)
{
    switch (msg.type)
    {
    case Message::Type::Physics:
        if (m_timeSinceEcho > 0) break;
        if (msg.physics.entityId == getParentUID())
        {
            m_echo = std::make_unique<EchoDrawable>(m_circleShape.getRadius(), getMessageBus());
            m_echo->setColour(m_circleShape.getOutlineColor());
            m_timeSinceEcho = minEchoTime;
        }
        break;
    default:break;
    }
}

void CircleDrawable::setOuterColour(const sf::Color& colour)
{
    m_circleShape.setOutlineColor(colour);
}

void CircleDrawable::setInnerColour(const sf::Color& colour)
{
    m_circleShape.setFillColor(colour);
}

void CircleDrawable::setRadius(float radius)
{
    m_circleShape.setRadius(radius);
    m_circleShape.setOrigin(radius / 2.f, radius / 2.f);
}

void CircleDrawable::setOutlineThickness(float thickness)
{
    m_circleShape.setOutlineThickness(thickness);
}

//private
void CircleDrawable::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    rt.draw(m_circleShape, states);
}