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

//circular physics body

#ifndef PHYS_COMP_HPP_
#define PHYS_COMP_HPP_

#include <Component.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

class PhysicsComponent final : public Component
{
public:
    PhysicsComponent(float, MessageBus&);
    ~PhysicsComponent() = default;

    Component::Type type() const override;
    void entityUpdate(Entity&, float) override;

    void physicsUpdate(float, sf::Vector3f);

    void applyForce(const sf::Vector2f& force);
    void setPosition(const sf::Vector2f& position);
    void setVelocity(const sf::Vector2f& velocity);

private:

    sf::Vector2f m_position;
    sf::Vector2f m_velocity;
    float m_mass;
    float m_radius;
};

#endif //PHYS_COMP_HPP_