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
#include <AnimatedDrawable.hpp>
#include <ParticleSystem.hpp>
#include <MessageBus.hpp>
#include <Util.hpp>

#include <cassert>

namespace
{
    const float stiffness = 0.93f;
    const float maxBounds = 1920.f;
    const float minBounds = 0.f;
    const float impactReduction = 0.6f; //reduction of velocity when hitting edges
    const float maxHealth = 100.f;
    const float healthReduction = 4.f; //reduction per second
    const float minHealth = 2.f; //don't want non-tail parts completely dying
    const float hitPoint = 0.5f; //lose this if hit by a ball

    const float planktonHealth = 50.f;
    const float bonusHealth = 100.f;

    const sf::Color defaultColour(200u, 200u, 230u, 180u);
}

BodypartController::BodypartController(MessageBus& mb)
    : Component     (mb),
    m_physComponent (nullptr),
    m_drawable      (nullptr),
    m_sparkles      (nullptr),
    m_health        (maxHealth)
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

    //reduce health if this is the tail
    if (m_physComponent->getContraintCount() < 2)
    {
        m_health -= healthReduction * dt;
        if (m_health <= 0)
        {
            entity.destroy();
            Message msg;
            msg.type = Message::Type::Player;
            msg.player.action = Message::PlayerEvent::PartRemoved;
            msg.player.value = m_physComponent->getMass();
            sendMessage(msg);
        }

        auto colour = defaultColour;
        colour.a = static_cast<sf::Uint8>(std::max((m_health / maxHealth) * static_cast<float>(defaultColour.a), 0.f));
        m_drawable->setColour(colour);
    }
}

void BodypartController::handleMessage(const Message& msg)
{
    switch (msg.type)
    {
    case Message::Type::Physics:

        if ((msg.physics.entityId[0] == getParentUID() || msg.physics.entityId[1] == getParentUID()))
        {
            if (m_health > minHealth)
                m_health -= hitPoint;

            //TODO fire a particle effect
        }
        break;
    case Message::Type::Plankton:
        if (msg.plankton.action == Message::PlanktonEvent::Died
            && m_physComponent->getContraintCount() < 2) //we're on the end
        {
            Message newMessage;
            newMessage.type = Message::Type::Player;

            switch (msg.plankton.type)
            {
            case PlanktonController::Type::Good:
                m_health += planktonHealth;
                newMessage.player.action = Message::PlayerEvent::HealthAdded;
                m_sparkles->start(4u, 0.f, 0.6f);
                break;
            case PlanktonController::Type::Bad:
                m_health -= planktonHealth;
                newMessage.player.action = Message::PlayerEvent::HealthLost;
                //TODO same particle effect as hit from physics?
                break;
            case PlanktonController::Type::Bonus:
                m_health += bonusHealth;
                newMessage.player.action = Message::PlayerEvent::HealthAdded;
                //TODO prevent health counting down for short duration

                m_sparkles->start(4u, 0.f, 0.6f);
                break;
            default:break;
            }

            //clamp health and send remainder
            const float remainder = m_health - maxHealth;
            m_health = std::min(m_health, maxHealth);           
            newMessage.player.value = remainder;
            sendMessage(newMessage);
        }
        break;
    default: break;
    }
}

void BodypartController::onStart(Entity& entity)
{
    m_physComponent = entity.getComponent<PhysicsComponent>("control");
    assert(m_physComponent);

    m_drawable = entity.getComponent<AnimatedDrawable>("drawable");
    assert(m_drawable);
    m_drawable->setColour(defaultColour);

    m_sparkles = entity.getComponent<ParticleSystem>("sparkle");
    assert(m_sparkles);
}

void BodypartController::setHealth(float health)
{
    assert(health >= 0 && health <= maxHealth);
    m_health = health;
}