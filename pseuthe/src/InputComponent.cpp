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

#include <InputComponent.hpp>
#include <PhysicsComponent.hpp>
#include <Entity.hpp>
#include <Util.hpp>

#include <SFML/Window/Keyboard.hpp>

#include <cassert>

namespace
{
    const float force = 600.f;
    const float speed = 500.f;
    const float maxSpeed = speed * speed;

    const float maxBounds = 1920.f;
    const float minBounds = 0.f;
    const float impactReduction = 0.6f; //reduction of velocity when hitting edges
}

InputComponent::InputComponent(MessageBus& mb)
    : Component         (mb),
    m_physicsComponent  (nullptr)
{

}

Component::Type InputComponent::type() const
{
    return Component::Type::Script;
}

void InputComponent::entityUpdate(Entity& entity, float dt)
{
    sf::Vector2f forceVec;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        forceVec.y -= 1.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        forceVec.y += 1.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        forceVec.x -= 1.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        forceVec.x += 1.f;
    }
    Util::Vector::normalise(forceVec);
    forceVec *= force * dt;

    //limit speed
    if (Util::Vector::lengthSquared(m_physicsComponent->getVelocity() + forceVec) > maxSpeed)
        m_physicsComponent->applyForce(-m_physicsComponent->getVelocity() * 0.01f);
    else
        m_physicsComponent->applyForce(forceVec);

    //check player bounds
    auto currentPosition = m_physicsComponent->getPosition();
    if (currentPosition.x < minBounds)
    {
        sf::Vector2f normal(1.f, 0.f);
        m_physicsComponent->setVelocity(Util::Vector::reflect(m_physicsComponent->getVelocity(), normal) * impactReduction);
        m_physicsComponent->setPosition({ minBounds, currentPosition.y });
    }
    else if (currentPosition.x > maxBounds)
    {
        sf::Vector2f normal(-1.f, 0.f);
        m_physicsComponent->setVelocity(Util::Vector::reflect(m_physicsComponent->getVelocity(), normal) * impactReduction);
        m_physicsComponent->setPosition({ maxBounds, currentPosition.y });
    }
}

void InputComponent::handleMessage(const Message& msg)
{

}

void InputComponent::onStart(Entity& entity)
{
    m_physicsComponent = entity.getComponent<PhysicsComponent>("control");
    assert(m_physicsComponent);
}