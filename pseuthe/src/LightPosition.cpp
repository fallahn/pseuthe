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

#include <LightPosition.hpp>
#include <Entity.hpp>

namespace
{
    //TODO these consts are used in several places (caustic drawable)
    //need to collate somewhere.
    const float min = -30.f;
    const float max = 1980.f;
    const float moveSpeed = 10.f;
}

LightPosition::LightPosition(MessageBus& mb)
    : Component(mb)
{

}

//public
Component::Type LightPosition::type() const
{
    return Component::Type::Script;
}

void LightPosition::entityUpdate(Entity& entity, float dt)
{
    entity.move(moveSpeed * dt, 0.f);

    const float xPos = entity.getPosition().x;
    sf::Vector2f position(xPos - (max - min), entity.getPosition().y);
    if (xPos > max) entity.setPosition(position);
    entity.setRotation(((entity.getPosition().x / max) * 90.f) - 45.f);
}

void LightPosition::handleMessage(const Message&)
{

}