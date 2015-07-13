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
    const float decayRate = 4.f; //reduction per second
    const float minHealth = 2.f; //don't want non-tail parts completely dying
    const float hitPoint = 0.5f; //lose this if hit by a ball

    const float planktonHealth = 50.f;
    const float bonusHealth = 100.f;
    const float uberHealth = 1000.f;

    const sf::Color defaultColour(200u, 200u, 230u, 180u);
}

BodypartController::BodypartController(MessageBus& mb)
    : Component     (mb),
    m_physComponent (nullptr),
    m_drawable      (nullptr),
    m_sparkles      (nullptr),
    m_echo          (nullptr),
    m_health        (maxHealth),
    m_decayRate     (decayRate),
    m_paused        (false)
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
        m_physComponent->setPosition({ minBounds + (minBounds - currentPosition.x), currentPosition.y });
    }
    else if (currentPosition.x > maxBounds)
    {
        sf::Vector2f normal(-1.f, 0.f);
        m_physComponent->setVelocity(Util::Vector::reflect(m_physComponent->getVelocity(), normal) * impactReduction);
        m_physComponent->setPosition({ maxBounds - (currentPosition.x - maxBounds), currentPosition.y });
    }

    //reduce health if this is the tail
    if (m_physComponent->getContraintCount() < 2 && !m_paused)
    {
        m_health -= m_decayRate * dt;
        if (m_health <= 0 && m_echo->getParticleCount() == 0)
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

        if ((msg.physics.entityId[0] == getParentUID() || msg.physics.entityId[1] == getParentUID())
            && msg.physics.event == Message::PhysicsEvent::Collision)
        {
            if (m_health > minHealth && !m_paused)
            {
                m_health -= hitPoint;
                m_echo->start(1u, 0.f, 0.02f);
            }
        }
        break;
    case Message::Type::Plankton:
        if (msg.plankton.action == Message::PlanktonEvent::Died
            && msg.plankton.touchingPlayer
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
                m_health -= planktonHealth * 0.7f;
                newMessage.player.action = Message::PlayerEvent::HealthLost;
                m_echo->start(1u, 0.f, 0.02f);
                break;
            case PlanktonController::Type::Bonus:
                m_health += bonusHealth;
                newMessage.player.action = Message::PlayerEvent::HealthAdded;
                m_sparkles->start(4u, 0.f, 0.6f);
                break;
            case PlanktonController::Type::UberLife:
                m_health += uberHealth;
                newMessage.player.action = Message::PlayerEvent::HealthAdded;
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
    case Message::Type::UI:
        switch (msg.ui.type)
        {
        case Message::UIEvent::MenuClosed:
            if (msg.ui.stateId == States::ID::Menu)
                m_paused = false;
            break;
        case Message::UIEvent::MenuOpened:
            if (msg.ui.stateId == States::ID::Menu)
                m_paused = true;
            break;
        default:break;
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

    m_echo = entity.getComponent<ParticleSystem>("echo");
    assert(m_echo);
}

void BodypartController::setHealth(float health)
{
    assert(health >= 0 && health <= maxHealth);
    m_health = health;
}

void BodypartController::setDecayRate(float rate)
{
    assert(rate > 0 && rate < maxHealth);
    m_decayRate = rate;
}