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
#include <MessageBus.hpp>

#include <cassert>

PhysicsComponent::Constraint::Constraint(PhysicsComponent* a, PhysicsComponent* b, float length)
    : m_destroyed   (false),
    m_bodyA         (a),
    m_bodyB         (b),
    m_length        (length)
{
    assert(a);
    assert(b);
    assert((length * length) > a->getRadiusSquared() + b->getRadiusSquared());

    a->addConstraint(this);
    b->addConstraint(this);
}

void PhysicsComponent::Constraint::update(float dt)
{
    if (m_destroyed) return;

    auto constraintVec = m_bodyB->m_position - m_bodyA->m_position;
    float constraintLength = Util::Vector::length(constraintVec);
    auto constraintUnit = constraintVec * (1.f / constraintLength);
    float relativeDistance = constraintLength - m_length;    
    float relativeVelocity = Util::Vector::dot((m_bodyB->m_velocity - m_bodyA->m_velocity), constraintUnit);
    float excess = (relativeVelocity + relativeDistance);
    sf::Vector2f force(constraintUnit * excess);

    m_bodyA->applyForce(force);
    //m_bodyB->applyForce(-force);
}

void PhysicsComponent::Constraint::destroy()
{
    //m_bodyA->removeConstraint(this);
    //m_bodyB->removeConstraint(this);
    Message msg;
    msg.type = Message::Type::Physics;
    msg.physics.event = Message::PhysicsEvent::ConstraintDestroyed;
    msg.physics.constraint = this;
    msg.physics.entityId[0] = m_bodyA->getParentUID();
    msg.physics.entityId[1] = m_bodyB->getParentUID();
    m_bodyA->sendMessage(msg);

    m_destroyed = true;
}

bool PhysicsComponent::Constraint::destroyed() const
{
    return m_destroyed;
}