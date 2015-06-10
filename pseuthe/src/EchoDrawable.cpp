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

#include <EchoDrawable.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

namespace
{
    const float lifetime = 1.5f; //seconds
}

EchoDrawable::EchoDrawable(float radius, MessageBus& mb)
    : Component     (mb),
    m_circleShape   (radius),
    m_colour        (sf::Color::White),
    m_lifetime      (lifetime)
{
    m_circleShape.setFillColor(sf::Color::Transparent);
    m_circleShape.setOutlineThickness(2.f);
    m_circleShape.setOrigin(radius, radius);
}

//public
Component::Type EchoDrawable::type() const
{
    return Component::Type::Drawable;
}

void EchoDrawable::entityUpdate(Entity&, float dt)
{
    auto currentScale = m_circleShape.getScale();
    currentScale.x += dt;
    currentScale.y += dt;
    m_circleShape.setScale(currentScale);

    //fade
    m_lifetime -= dt;
    const float ratio = m_lifetime / lifetime;
    m_colour.a = static_cast<sf::Uint8>(255.f * std::max(ratio, 0.f));
    m_circleShape.setOutlineColor(m_colour);

    if (m_colour.a == 0) destroy();
}

void EchoDrawable::handleMessage(const Message& msg)
{

}

void EchoDrawable::setColour(const sf::Color& colour)
{
    m_circleShape.setOutlineColor(colour);
    m_colour = colour;
}

//private
void EchoDrawable::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    rt.draw(m_circleShape, states);
}