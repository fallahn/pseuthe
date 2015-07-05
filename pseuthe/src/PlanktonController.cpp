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

#include <PlanktonController.hpp>
#include <Entity.hpp>
#include <MessageBus.hpp>
#include <PhysicsComponent.hpp>
#include <AnimatedDrawable.hpp>
#include <ParticleSystem.hpp>
#include <TailDrawable.hpp>
#include <Util.hpp>

#include <SFML/Graphics/Color.hpp>

#include <cassert>

namespace
{
    const sf::Color goodColour(200u, 230u, 200u, 180u);
    const sf::Color badColour(230u, 200u, 200u, 180u);
    const sf::Color bonusColour(170u, 170u, 200u, 180u);

    const float maxHealth = 100.f;
    const float healthReduction = 45.f; //reduction per second
    const float uberTypeHealthReduction = 1;// 8.f;
    const float badTypeHealthReduction = 2.f;

    const float rotationSpeed = 6.f;
    const float rotationTolerance = 0.1f;

    const float wallDamage = 8.f; //reduction in health when rebounding
}

PlanktonController::PlanktonController(MessageBus& mb)
    : Component         (mb),
    m_flags             (0u),
    m_type              (Type::Good),
    m_physComponent     (nullptr),
    m_drawable          (nullptr),
    m_trail             (nullptr),
    m_ident             (nullptr),
    m_tail              (nullptr),
    m_health            (maxHealth),
    m_enemyId           (0u),
    m_targetRotation    (0.f)
{

}

//public
Component::Type PlanktonController::type() const
{
    return Component::Type::Script;
}

void PlanktonController::entityUpdate(Entity& entity, float dt)
{
    //check flags
    if (m_flags & Flags::HealthHit)
    {
        m_health -= healthReduction * dt;
        m_flags &= ~Flags::HealthHit;
    }

    if ((m_flags & Flags::RequestRotation) && m_type != Type::Bonus)
    {           
        float currentRotation = m_drawable->getRotation();
        m_drawable->rotate(Util::Math::shortestRotation(currentRotation, m_targetRotation) * rotationSpeed * dt);

        if (m_type == Type::UberLife) m_tail->setRotation(m_drawable->getRotation());

        const float diff = currentRotation - m_targetRotation;
        if (diff < rotationTolerance && diff > -rotationTolerance)
        {
            m_flags &= ~Flags::RequestRotation;
        }
    }

    if (m_flags & Flags::Suicide)
    {
        m_health = 0.f;
    }

    //check health
    if (m_health <= 0)
    {
        m_trail->stop();
        m_ident->stop();
        if (m_trail->getParticleCount() == 0)
        {
            entity.destroy();
        
            //send message
            Message msg;
            msg.type = Message::Type::Plankton;
            msg.plankton.action = Message::PlanktonEvent::Died;
            msg.plankton.type = m_type;
            msg.plankton.touchingPlayer = ((m_flags & Flags::TouchingPlayer) != 0);
            sendMessage(msg);
        }
    }
    

    //set colour
    sf::Color colour;
    switch (m_type)
    {
    case Type::Good:
    default:
        colour = goodColour;
        break;
    case Type::Bad:
        colour = badColour;
        m_health -= badTypeHealthReduction * dt;
        break;
    case Type::Bonus:
        colour = bonusColour;
        m_drawable->rotate(Util::Vector::dot({ 1.f, 0.f }, m_physComponent->getVelocity()) * dt);
        break;
    case Type::UberLife:
        colour = bonusColour;
        m_health -= uberTypeHealthReduction * dt;
        break;
    }
    colour.a = static_cast<sf::Uint8>(std::max((m_health / maxHealth) * static_cast<float>(colour.a), 0.f));
    m_drawable->setColour(colour);
    if (m_type == Type::UberLife) m_tail->setColour(colour);
}

void PlanktonController::handleMessage(const Message& msg)
{
    if (msg.type == Message::Type::Physics)
    {
        switch (msg.physics.event)
        {
        case Message::PhysicsEvent::Trigger:
            assert(m_enemyId > 0);
            if ((msg.physics.entityId[0] == m_enemyId || msg.physics.entityId[1] == m_enemyId)
                && (msg.physics.entityId[0] == getParentUID() || msg.physics.entityId[1] == getParentUID()))
            {
                m_flags |= Flags::HealthHit | Flags::TouchingPlayer;
            }
            break;
        case Message::PhysicsEvent::Collision:
            //request rotation change
            if (msg.physics.entityId[0] == getParentUID())
            {
                m_targetRotation = Util::Vector::rotation(m_physComponent->getVelocity());
                m_flags |= Flags::RequestRotation;
                m_flags &= ~Flags::TouchingPlayer;
                m_health -= wallDamage;
            }
            break;
        default: break;
        }
    }
    else if (msg.type == Message::Type::Player)
    {
        switch (msg.player.action)
        {
        case Message::PlayerEvent::Died:
            m_flags |= Flags::Suicide; //goodbye cruel world!
            break;
        default: break;
        }
    }
}

void PlanktonController::onStart(Entity& entity)
{
    m_physComponent = entity.getComponent<PhysicsComponent>("control");
    assert(m_physComponent);

    m_targetRotation = Util::Vector::rotation(m_physComponent->getVelocity());
    if (m_targetRotation < 0) m_targetRotation += 360.f;
    m_flags |= Flags::RequestRotation;

    m_drawable = entity.getComponent<AnimatedDrawable>("drawable");
    assert(m_drawable);

    m_trail = entity.getComponent<ParticleSystem>("trail");
    assert(m_trail);

    m_ident = entity.getComponent<ParticleSystem>("ident");
    assert(m_ident);

    if (m_type == Type::UberLife)
    {
        m_tail = entity.getComponent<TailDrawable>("tail");
        assert(m_tail);
        m_tail->setColour(bonusColour);
    }
}

void PlanktonController::setType(Type t)
{
    m_type = t;
}

void PlanktonController::setEnemyId(sf::Uint64 id)
{
    m_enemyId = id;
}

//private