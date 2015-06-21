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

#include <BodypartController.hpp>
#include <Entity.hpp>
#include <PhysicsComponent.hpp>
#include <Util.hpp>

#include <cassert>

namespace
{
    const float stiffness = 0.93f;
    const float maxBounds = 1920.f;
    const float minBounds = 0.f;
    const float impactReduction = 0.6f; //reduction of velocity when hitting edges
}

BodypartController::BodypartController(MessageBus& mb)
    : Component(mb),
    m_physComponent(nullptr)
{

}


//public
Component::Type BodypartController::type() const
{
    return Component::Type::Script;
}

void BodypartController::entityUpdate(Entity& entity, float dt)
{
    auto velocity = m_physComponent->getVelocity();
    m_physComponent->setVelocity(velocity * stiffness);

    entity.setRotation(Util::Vector::rotation(velocity));

    //check bounds
    auto currentPosition = m_physComponent->getPosition();
    if (currentPosition.x < minBounds)
    {
        sf::Vector2f normal(1.f, 0.f);
        m_physComponent->setVelocity(Util::Vector::reflect(m_physComponent->getVelocity(), normal) * impactReduction);
        m_physComponent->setPosition({ minBounds, currentPosition.y });
    }
    else if (currentPosition.x > maxBounds)
    {
        sf::Vector2f normal(-1.f, 0.f);
        m_physComponent->setVelocity(Util::Vector::reflect(m_physComponent->getVelocity(), normal) * impactReduction);
        m_physComponent->setPosition({ maxBounds, currentPosition.y });
    }
}

void BodypartController::handleMessage(const Message&)
{

}

void BodypartController::onStart(Entity& entity)
{
    m_physComponent = entity.getComponent<PhysicsComponent>("control");
    assert(m_physComponent);
}