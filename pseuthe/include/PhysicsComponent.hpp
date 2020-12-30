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
#include <SFML/Graphics/Rect.hpp>

#include <memory>
#include <vector>

class PhysicsComponent final : public Component
{
public:
    using Ptr = std::unique_ptr<PhysicsComponent>;

    struct Manifold
    {
        sf::Vector2f normal;
        float penetration = 0.f;
        sf::Vector2f transferForce;
    };

    PhysicsComponent(float, MessageBus&);
    ~PhysicsComponent() = default;

    Component::Type type() const override;
    void entityUpdate(Entity&, float) override;
    void handleMessage(const Message&) override;
    void destroy() override;

    void physicsUpdate(float, const sf::FloatRect&);
    void resolveCollision(PhysicsComponent*, const Manifold&);

    void applyForce(const sf::Vector2f& force);
    void setPosition(const sf::Vector2f& position);
    void setVelocity(const sf::Vector2f& velocity);

    const sf::Vector2f& getPosition() const;
    const sf::Vector2f& getVelocity() const;
    float getMass() const;
    float getInverseMass() const;

    float getRadiusSquared() const;

    class Constraint final
    {
    public:
        using Ptr = std::unique_ptr<Constraint>;
        Constraint(PhysicsComponent*, PhysicsComponent*, float);
        ~Constraint() = default;

        void update(float);

        void destroy();
        bool destroyed() const;

    private:
        bool m_destroyed;
        PhysicsComponent* m_bodyA;
        PhysicsComponent* m_bodyB;
        float m_length;
    };

    void addConstraint(Constraint*);
    void removeConstraint(Constraint* constraint);
    std::size_t getContraintCount() const;

    void setTriggerOnly(bool);
    bool isTrigger() const;

private:

    sf::Vector2f m_position;
    sf::Vector2f m_velocity;
    float m_radius;
    float m_mass;
    float m_inverseMass;

    std::vector<Constraint*> m_constraints;

    bool m_triggerOnly;
};

#endif //PHYS_COMP_HPP_