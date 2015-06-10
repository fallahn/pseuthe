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
    const float density = 0.5f;
    const float PI = 3.142f;

    const int maxVelocity = 400;
    const int maxPosition = 1080;

    const float epsilon = 0.2f;
}

PhysicsComponent::PhysicsComponent(float radius, MessageBus& m)
    : Component     (m),
    m_radius        (radius),
    m_mass          ((std::powf(radius, 2.f) * PI * density / 2.f)),
    m_inverseMass   (1.f/m_mass)
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
            && m_position.x < bounds.left + bounds.width)
        {
            //we're off the top or bottom, so bounce
            sf::Vector2f normal;
            normal.y = (m_position.y < bounds.top) ? 1.f : -1.f;
            m_velocity = Util::Vector::reflect(m_velocity, normal);

            m_position.y = Util::Math::clamp(m_position.y, bounds.top, bounds.top + bounds.height);
        }
        else if (m_position.y > bounds.top
            && m_position.y < bounds.top + bounds.height)
        {
            //off the edge so reposition somewhere
            (m_velocity.x > 0) ? m_position.x -= bounds.width : m_position.x += bounds.width;           
        }
        else
        {
            m_position.y = static_cast<float>(Util::Random::value(static_cast<int>(bounds.top), static_cast<int>(bounds.top + bounds.height)));
        }
    }
}

void PhysicsComponent::resolveCollision(PhysicsComponent* other, const Manifold& m)
{
    m_velocity += m.transferForce;
    m_position -= (other->m_mass / (other->m_mass + m_mass) * m.penetration) * m.normal;
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

const sf::Vector2f& PhysicsComponent::getPosition() const
{
    return m_position;
}

const sf::Vector2f& PhysicsComponent::getVelocity() const
{
    return m_velocity;
}

float PhysicsComponent::getMass() const
{
    return m_mass;
}

float PhysicsComponent::getInverseMass() const
{
    return m_inverseMass;
}

float PhysicsComponent::getRadiusSquared() const
{
    return m_radius * m_radius;
}

//private