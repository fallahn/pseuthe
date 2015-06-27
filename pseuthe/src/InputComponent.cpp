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
#include <SFML/Window/Joystick.hpp>

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
    const float planktonHealth = 50.f;
    const float bonusHealth = 100.f;
    const sf::Color defaultColour(200u, 200u, 230u, 180u);

    const float minTrailRate = 1.f;
    const float maxTrailRate = 50.f;

    const float dragMultiplier = 2900.f;

    const float joyDeadZone = 25.f;

    Animation mouthAnim("", 0, 0, false);
}

InputComponent::InputComponent(MessageBus& mb)
    : Component         (mb),
    m_physicsComponent  (nullptr),
    m_headDrawable      (nullptr),
    m_mouthDrawable     (nullptr),
    m_trailParticles    (nullptr),
    m_sparkleParticles  (nullptr),
    m_echo              (nullptr),
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
        forceVec = getKeyboard();
        if (Util::Vector::lengthSquared(forceVec) > 0.1f)
        {
            Util::Vector::normalise(forceVec);
            forceVec *= force * m_invMass * dt;
        }
        else
        {
            forceVec = getController();
            forceVec *= dt;
        }
    }

    //forceVec = Util::Vector::rotate(forceVec, m_drawable->getRotation() + 90.f);

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

    m_headDrawable->setRotation(Util::Vector::rotation(m_physicsComponent->getVelocity()));
    m_mouthDrawable->setRotation(m_headDrawable->getRotation());

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
        m_headDrawable->setColour(colour);
        m_mouthDrawable->setColour(colour);
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
    else if (msg.type == Message::Type::Player)
    {
        switch (msg.player.action)
        {
        case Message::PlayerEvent::PartAdded:
            m_mass += msg.player.value;
            m_invMass = (1.f / m_mass) * dragMultiplier;
            break;
        case Message::PlayerEvent::PartRemoved:
            m_mass = std::max(0.f, m_mass - msg.player.value);
            m_invMass = (m_mass > 0.01f) ? (1.f / m_mass) * dragMultiplier : 1.f;
            break;
        default: break;
        }
    }
    else if (msg.type == Message::Type::Plankton)
    {
        if (msg.plankton.action == Message::PlanktonEvent::Died
            && m_physicsComponent->getContraintCount() == 0) //no body parts
        {
            Message newMessage;
            newMessage.type = Message::Type::Player;

            switch (msg.plankton.type)
            {
            case PlanktonController::Type::Good:
                m_health += planktonHealth;
                newMessage.player.action = Message::PlayerEvent::HealthAdded;
                m_sparkleParticles->start(4u, 0.f, 0.6f);
                break;
            case PlanktonController::Type::Bad:
                m_health -= planktonHealth;
                newMessage.player.action = Message::PlayerEvent::HealthLost;
                m_echo->start(1u, 0.f, 0.02f);
                break;
            case PlanktonController::Type::Bonus:
                m_health += bonusHealth;
                newMessage.player.action = Message::PlayerEvent::HealthAdded;
                //TODO prevent health counting down for short duration
                m_sparkleParticles->start(4u, 0.f, 0.6f);
                break;
            default:break;
            }

            //clamp health and send remainder
            const float remainder = m_health - maxHealth;
            m_health = std::min(m_health, maxHealth);
            newMessage.player.value = remainder;
            sendMessage(newMessage);
        }
    }
    else if (msg.type == Message::Type::Physics)
    {
        switch (msg.physics.event)
        {
        default: break;
        case Message::PhysicsEvent::Trigger:
            if (msg.physics.entityId[0] == getParentUID()
                || msg.physics.entityId[1] == getParentUID())
            {
                if (!m_mouthDrawable->playing())
                    m_mouthDrawable->play(mouthAnim);
                //else LOG("already playing" + std::to_string(Util::Random::value(0, 1000)), Logger::Type::Info);
            }
            break;
        }
    }
}

void InputComponent::onStart(Entity& entity)
{
    m_physicsComponent = entity.getComponent<PhysicsComponent>("control");
    assert(m_physicsComponent);

    m_headDrawable = entity.getComponent<AnimatedDrawable>("head");
    assert(m_headDrawable);
    m_headDrawable->setColour(defaultColour);

    m_mouthDrawable = entity.getComponent<AnimatedDrawable>("mouth");
    assert(m_mouthDrawable);
    m_mouthDrawable->setColour(defaultColour);
    if (!m_mouthDrawable->getAnimations().empty()) mouthAnim = m_mouthDrawable->getAnimations()[0];

    m_trailParticles = entity.getComponent<ParticleSystem>("trail");
    assert(m_trailParticles);

    m_sparkleParticles = entity.getComponent<ParticleSystem>("sparkle");
    assert(m_sparkleParticles);

    m_echo = entity.getComponent<ParticleSystem>("echo");
    assert(m_echo);
}

//private
sf::Vector2f InputComponent::getKeyboard()
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
    return forceVec;
}

sf::Vector2f InputComponent::getController()
{
    //TODO how do we maintain the analog of the controller? (normalising the vector breaks this)
    //try and remember dot product tomorrow... cos you're too tired now.
    sf::Vector2f forceVec;
    if (sf::Joystick::isConnected(0))
    {
        auto axisPosX = sf::Joystick::getAxisPosition(0, sf::Joystick::PovX);
        if (axisPosX < -joyDeadZone || axisPosX > joyDeadZone)
        {
            forceVec.x = force * (axisPosX / 100.f) * m_invMass;
        }
        auto axisPosY = -sf::Joystick::getAxisPosition(0, sf::Joystick::PovY);
        if (axisPosY < -joyDeadZone || axisPosY > joyDeadZone)
        {
            forceVec.y = force * (axisPosY / 100.f) * m_invMass;
        }

        axisPosX = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
        if (axisPosX < -joyDeadZone || axisPosX > joyDeadZone)
        {
            forceVec.x = force * (axisPosX / 100.f) * m_invMass;
        }

        axisPosY = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
        if (axisPosY < -joyDeadZone || axisPosY > joyDeadZone)
        {
            forceVec.y = force * (axisPosY / 100.f) * m_invMass;
        }
    }

    return forceVec;
}