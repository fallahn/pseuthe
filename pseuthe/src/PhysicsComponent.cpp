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
#include <Util.hpp>

namespace
{
    const float density = 10.f;
    const float PI = 3.142f;
}

PhysicsComponent::PhysicsComponent(float radius, MessageBus& m)
    : Component (m),
    m_radius(radius),
    m_mass(std::powf(radius, 3.f) * 4.f * PI * density / 3.f)
{
    
}

//public
Component::Type PhysicsComponent::type() const
{
    return Component::Type::Physics;
}

void PhysicsComponent::entityUpdate(Entity& e, float dt)
{
    //set the parent entity's position to that of phys body
}

void PhysicsComponent::physicsUpdate(float dt, sf::Vector3f manifold)
{

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

//private