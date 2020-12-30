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
#include <SFML/Graphics/Shader.hpp>

namespace
{
    const float alpha = 100u;
    const float baseColour = 185.f;
}

CircleDrawable::CircleDrawable(float radius, MessageBus& m)
    : Component     (m),
    m_circleShape   (radius),
    m_shader        (nullptr),
    m_normalMap     (nullptr)
{
    m_circleShape.setOrigin(radius, radius);
    m_circleShape.setOutlineThickness(1.4f);

    float colour = std::min(radius / 46.f, 1.f);
    sf::Uint8 colourByte = static_cast<sf::Uint8>(colour * baseColour);
    sf::Color finalColour(colourByte, colourByte, colourByte, static_cast<sf::Uint8>(colour * 240.f));
    m_circleShape.setOutlineColor(finalColour);
    finalColour.a = static_cast<sf::Uint8>(colour * alpha);
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
    colour.a = static_cast<sf::Uint8>(alpha);
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

void CircleDrawable::setShader(sf::Shader& shader)
{
    m_shader = &shader;
}

void CircleDrawable::setTexture(const sf::Texture& t)
{
    m_circleShape.setTexture(&t, true);
}

void CircleDrawable::setNormalMap(const sf::Texture& t)
{
    m_normalMap = &t;
}

//private
void CircleDrawable::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    //states.transform *= m_circleShape.getTransform();
    states.blendMode = sf::BlendAdd;
    states.shader = m_shader;

    if (m_shader)
    {
        m_shader->setUniform("u_inverseWorldViewMatrix", sf::Glsl::Mat4(states.transform.getInverse()));
        m_shader->setUniform("u_diffuseMap", sf::Shader::CurrentTexture);
        m_shader->setUniform("u_normalMap", *m_normalMap);
    }

    rt.draw(m_circleShape, states);
}