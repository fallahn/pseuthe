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

#include <MakeUnique.hpp>
#include <TailDrawable.hpp>
#include <Util.hpp>
#include <Log.hpp>

#include <cassert>

namespace
{
    const float pixelScale = 100.f; //scale pixels to metres

    const int jointCount = 12;
    const float jointMass = 0.15f;
    const float stiffness = 5000.f;
    const float constraintLength = 0.12f;
    const float friction = 1.2f;
    const float airFriction = 0.32f;
}

//----mass----//
TailDrawable::Mass::Mass(float m)
    : m_mass(m)
{
    assert(m_mass > 0);
}

float TailDrawable::Mass::getMass() const
{
    return m_mass;
}

const sf::Vector2f& TailDrawable::Mass::getVelocity() const
{
    return m_velocity;
}

const sf::Vector2f& TailDrawable::Mass::getForce() const
{
    return m_force;
}

const sf::Vector2f& TailDrawable::Mass::getPosition() const
{
    return m_position;
}

void TailDrawable::Mass::setVelocity(const sf::Vector2f& v)
{
    m_velocity = v;
}

void TailDrawable::Mass::setPosition(const sf::Vector2f& position)
{
    m_position = position;
}

void TailDrawable::Mass::reset()
{
    m_force = sf::Vector2f();
}

void TailDrawable::Mass::applyForce(const sf::Vector2f& amount)
{
    m_force += amount;
}

void TailDrawable::Mass::simulate(float dt)
{
    m_velocity += (m_force / m_mass) * dt;
    m_position += m_velocity * dt;
}

//----constraint----//
TailDrawable::Constraint::Constraint(TailDrawable::Mass& ma, TailDrawable::Mass& mb)
    : m_massA     (ma),
    m_massB       (mb)
{}

void TailDrawable::Constraint::solve()
{
    auto constraintVec = m_massA.getPosition() - m_massB.getPosition();
    auto vecLength = Util::Vector::length(constraintVec);

    sf::Vector2f force(0.f ,0.f);
    if (vecLength != 0)
        force += -(constraintVec / vecLength) * (vecLength - constraintLength) * stiffness;

    force += -(m_massA.getVelocity() - m_massB.getVelocity()) * friction;
    m_massA.applyForce(force);
    m_massB.applyForce(-force);
}

//----simulation----//
TailDrawable::Simulation::Simulation(const sf::Vector2f& start, const sf::Vector2f& end)
    : m_anchor  (start / pixelScale),
    m_layoutEnd (end / pixelScale)
{
    for (auto i = 0; i < jointCount; ++i)
        m_masses.emplace_back(std::make_unique<Mass>(jointMass));

    layoutMasses();

    for (auto i = 0; i < jointCount - 1; ++i)
    {
        m_constraints.emplace_back(std::make_unique<Constraint>(*m_masses[i], *m_masses[i + 1]));
    }
}

const std::vector<TailDrawable::Mass::Ptr>& TailDrawable::Simulation::getMasses() const
{
    return m_masses;
}

void TailDrawable::Simulation::setAnchor(const sf::Vector2f& position)
{
    m_anchor = position / pixelScale;
    m_layoutEnd = m_anchor;
    m_layoutEnd.x -= constraintLength;
}

float TailDrawable::Simulation::getScale() const
{
    return pixelScale;
}

void TailDrawable::Simulation::setPosition(const sf::Vector2f& position)
{
    setAnchor(position);
    for (auto& m : m_masses)
    {
        m->setPosition(m->getPosition() + (position / pixelScale));
    }
}

void TailDrawable::Simulation::reset()
{
    for (auto& m : m_masses)
    {
        m->reset();
    }
}

void TailDrawable::Simulation::solve()
{
    for (auto& c : m_constraints)
    {
        c->solve();
    }

    for (auto& m : m_masses)
    {
        m->applyForce(-m->getVelocity() * airFriction);
    }
}

void TailDrawable::Simulation::simulate(float dt)
{
    for (auto& m : m_masses)
    {
        m->simulate(dt);
    }

    //set first point tied to anchor
    m_masses[0]->setVelocity({});
    m_masses[0]->setPosition(m_anchor);
}

void TailDrawable::Simulation::update(float dt)
{
    reset();
    solve();
    simulate(dt);
}

//private
void TailDrawable::Simulation::layoutMasses()
{   
    auto direction = Util::Vector::normalise(m_layoutEnd - m_anchor) * constraintLength;

    for (auto i = 0; i < jointCount; ++i)
    {
        m_masses[i]->setPosition(m_anchor + (direction * static_cast<float>(i)));
    }
}
