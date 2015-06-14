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
    const sf::Uint8 alpha = 100u;
}

CircleDrawable::CircleDrawable(float radius, MessageBus& m)
    : Component     (m),
    m_circleShape   (radius)
{
    m_circleShape.setOrigin(radius, radius);
    m_circleShape.setOutlineThickness(2.f);

    float colour = std::min(radius / 50.f, 1.f);
    colour *= 210.f;
    sf::Uint8 colourByte = static_cast<sf::Uint8>(colour);
    sf::Color finalColour(colourByte, colourByte, colourByte);
    m_circleShape.setOutlineColor(finalColour);
    finalColour.a = alpha;
    m_circleShape.setFillColor(finalColour);
}

//public
Component::Type CircleDrawable::type() const
{
    return Component::Type::Drawable;
}

void CircleDrawable::entityUpdate(Entity& parent, float dt)
{
    //modify colours and things based on entity properties

}

void CircleDrawable::handleMessage(const Message& msg)
{
    switch (msg.type)
    {
    case Message::Type::Physics:
        //if (msg.physics.entityId == getParentUID())
        //{

        //}
        //break;
    default:break;
    }
}

void CircleDrawable::setColour(sf::Color colour)
{
    m_circleShape.setOutlineColor(colour);
    colour.a = alpha;
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

const sf::Color& CircleDrawable::getColour() const
{
    return m_circleShape.getOutlineColor();
}

//private
void CircleDrawable::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    states.blendMode = sf::BlendAdd;
    rt.draw(m_circleShape, states);
}