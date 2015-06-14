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

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

ParticleField::ParticleField(const sf::FloatRect& bounds, MessageBus& mb)
    : Component (mb),
    m_blendMode (sf::BlendAdd)
{
    //TODO create a bunc of random positions and add particles there
}

//public
Component::Type ParticleField::type() const
{
    return Component::Type::Drawable;
}

void ParticleField::entityUpdate(Entity&, float dt)
{
    //TODO update all the particle positions

    //TODO update all the rotations / scales

    //TODO rebuild vertex array
}

void ParticleField::handleMessage(const Message& msg)
{

}

void ParticleField::setBlendMode(sf::BlendMode mode)
{
    m_blendMode = mode;
}

//private
void ParticleField::addParticle(const sf::Vector2f& position)
{
    //TODO randomly scale size and relatively velocity

    //TODO set random roation in degs per sec
}

void ParticleField::addVertex(const sf::Vector2f& position, float u, float v, const sf::Color& colour)
{

}

void ParticleField::updateVertices()
{

}

void ParticleField::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{

}