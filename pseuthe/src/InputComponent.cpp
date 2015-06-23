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
#include <AnimatedDrawable.hpp>
#include <ParticleSystem.hpp>
#include <Entity.hpp>
#include <Util.hpp>
#include <MessageBus.hpp>
#include <Log.hpp>

#include <SFML/Window/Keyboard.hpp>

#include <cassert>

namespace
{
    const float force = 550.f;
    const float speed = 450.f;
    const float maxSpeed = speed * speed;

    const float maxBounds = 1920.f;
    const float minBounds = 0.f;
    const float impactReduction = 0.7f; //reduction of velocity when hitting edges

    const float maxHealth = 100.f;
    const float healthReduction = 4.f; //reduction per second
    const sf::Color defaultColour(190u, 190u, 255u);

    const float minTrailRate = 1.f;
    const float maxTrailRate = 50.f;

    const float dragMultiplier = 2900.f;
}

InputComponent::InputComponent(MessageBus& mb)
    : Component         (mb),
    m_physicsComponent  (nullptr),
    m_drawable          (nullptr),
    m_trailParticles    (nullptr),
    m_health            (maxHealth),
    m_parseInput        (true),
    m_mass              (0.f),
    m_invMass           (1.f)
{

}

Component::Type InputComponent::type() const
{
    return Component::Type::Script;
}

void InputComponent::entityUpdate(Entity& entity, float dt)
{
    sf::Vector2f forceVec;    
    if (m_parseInput)
    {
        //TODO parse controller input
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
        forceVec *= force * m_invMass * dt;
    }

    //limit speed
    auto currentSpeed = Util::Vector::lengthSquared(m_physicsComponent->getVelocity() + forceVec);
    if (currentSpeed > maxSpeed)
        m_physicsComponent->applyForce(-m_physicsComponent->getVelocity() * 0.01f);
    else
        m_physicsComponent->applyForce(forceVec);

    //set particle emit rate based on speed
    const float emitRate = (currentSpeed / maxSpeed) * maxTrailRate;
    m_trailParticles->setEmitRate(emitRate + minTrailRate);

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

    m_drawable->setRotation(Util::Vector::rotation(m_physicsComponent->getVelocity()));

    //update health if we have no tail
    if (m_physicsComponent->getContraintCount() < 1 && m_parseInput)
    {
        m_health -= healthReduction * dt;
        if (m_health <= 0)
        {
            m_trailParticles->stop();
            
            if (m_trailParticles->getParticleCount() == 0)
            {
                entity.destroy();
                Message msg;
                msg.type = Message::Type::Player;
                msg.player.action = Message::PlayerEvent::Died;
                sendMessage(msg);
            }
        }

        auto colour = defaultColour;
        colour.a = static_cast<sf::Uint8>(std::max((m_health / maxHealth) * static_cast<float>(defaultColour.a), 0.f));
        m_drawable->setColour(colour);
    }
}

void InputComponent::handleMessage(const Message& msg)
{
    if (msg.type == Message::Type::UI)
    {
        switch (msg.ui.type)
        {
        case Message::UIEvent::MenuClosed:
            m_parseInput = true;
            break;
        case Message::UIEvent::MenuOpened:
            m_parseInput = false;
            break;
        default:break;
        }
    }
    if (msg.type == Message::Type::Player)
    {
        switch (msg.player.action)
        {
        case Message::PlayerEvent::PartAdded:
            m_mass += msg.player.mass;
            m_invMass = (1.f / m_mass) * dragMultiplier;
            LOG("Current Inverse Mass: " + std::to_string(m_invMass), Logger::Type::Info);
            break;
        case Message::PlayerEvent::PartRemoved:
            m_mass = std::max(0.f, m_mass - msg.player.mass);
            m_invMass = (m_mass > 0.01f) ? (1.f / m_mass) * dragMultiplier : 1.f;
            LOG("Current Mass: " + std::to_string(m_mass), Logger::Type::Info);
            LOG("Current Inverse Mass: " + std::to_string(m_invMass), Logger::Type::Info);
            break;
        default: break;
        }
    }
}

void InputComponent::onStart(Entity& entity)
{
    m_physicsComponent = entity.getComponent<PhysicsComponent>("control");
    assert(m_physicsComponent);

    m_drawable = entity.getComponent<AnimatedDrawable>("drawable");
    assert(m_drawable);
    m_drawable->setColour(defaultColour);

    m_trailParticles = entity.getComponent<ParticleSystem>("trail");
    assert(m_trailParticles);
}