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

#include <PhysicsComponent.hpp>
#include <Entity.hpp>
#include <Util.hpp>

namespace
{
    const float density = 0.0001f;
    const float PI = 3.142f;

    const int maxVelocity = 100;
    const int maxPosition = 1080;

    const float epsilon = 0.2f;
}

PhysicsComponent::PhysicsComponent(float radius, MessageBus& m)
    : Component (m),
    m_radius    (radius),
    m_mass      (std::powf(radius, 3.f) * 4.f * PI * density / 3.f)
{
    m_velocity.x = static_cast<float>(Util::Random::value(-maxVelocity, maxVelocity));
    m_velocity.y = static_cast<float>(Util::Random::value(-maxVelocity, maxVelocity));

    m_position.x = static_cast<float>(Util::Random::value(0, maxPosition));
    m_position.y = static_cast<float>(Util::Random::value(0, maxPosition));
}

//public
Component::Type PhysicsComponent::type() const
{
    return Component::Type::Physics;
}

void PhysicsComponent::entityUpdate(Entity& e, float dt)
{
    //set the parent entity's position to that of phys body
    e.setPosition(m_position);
}

void PhysicsComponent::physicsUpdate(float dt, const sf::FloatRect& bounds)
{
    //bounce off top / bottom bounds, wrap left / right
    m_position += m_velocity * dt;

    if (!bounds.contains(m_position))
    {
        if (m_position.x > bounds.left 
            && m_position.y < bounds.left + bounds.width)
        {
            //we're off the top or bottom, so bounce
            sf::Vector2f normal;
            normal.y = (m_position.y < bounds.top) ? 1.f : -1.f;
            m_velocity = Util::Vector::reflect(m_velocity, normal);
        }
        else
        {
            //off the edge so reposition somewhere
            (m_velocity.x > 0) ? m_position.x -= bounds.width : m_position.x += bounds.width;

            m_position.y = static_cast<float>(Util::Random::value(static_cast<int>(bounds.top), static_cast<int>(bounds.top + bounds.height)));
        }
    }
}

void PhysicsComponent::resolveCollision()
{
    m_position += m_lastCollision.normal * (m_lastCollision.penetration + epsilon);
    m_velocity = Util::Vector::reflect(m_velocity, m_lastCollision.normal);
    m_velocity += m_lastCollision.transferForce;
}

void PhysicsComponent::applyForce(const sf::Vector2f& force)
{
    m_velocity += force;
}

void PhysicsComponent::setPosition(const sf::Vector2f& position)
{
    m_position = position;
}

void PhysicsComponent::setVelocity(const sf::Vector2f& velocity)
{
    m_velocity = velocity;
}

void PhysicsComponent::setManifold(const Manifold& manifold)
{
    m_lastCollision = manifold;
}

const sf::Vector2f& PhysicsComponent::getPosition() const
{
    return m_position;
}

float PhysicsComponent::getRadiusSquared() const
{
    return m_radius * m_radius;
}

sf::Vector2f PhysicsComponent::getTransferForce() const
{
    return Util::Vector::normalise(m_velocity) * m_mass;
}

//private