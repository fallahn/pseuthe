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

//drawable for wavy wiggly tails

#ifndef TAIL_DRAWABLE_HPP_
#define TAIL_DRAWABLE_HPP_

#include <Component.hpp>

#include <SFML/Graphics/Drawable.hpp>

#include <vector>

class TailDrawable final : public Component, public sf::Drawable
{
public:
    explicit TailDrawable(MessageBus&);
    ~TailDrawable() = default;

    Component::Type type() const override;
    void entityUpdate(Entity&, float) override;
    void handleMessage(const Message&) override;

private:

    struct Mass final
    {
        explicit Mass(float = 0.f);
        ~Mass() = default;

        float mass;
        sf::Vector2f velocity;
        sf::Vector2f force;
        sf::Vector2f position;

        void reset();
        void applyForce(const sf::Vector2f&);
        void simulate(float);
    };

    struct Constraint final
    {
        //TODO most of these values will be fine as consts
        Constraint(Mass&, Mass&, float stiffness, float length, float friction);
        ~Constraint() = default;

        Mass* massA;
        Mass* massB;
        float stiffness;
        float length;
        float friction;

        void solve();
    };

    class Simulation final
    {
    public:
        //TODO set fixed values for these rather than ctor params
        Simulation(int massCount, float mass, float stiffness, float constraintlength, float constraintFriction, float airFriction, const sf::Vector2f& start, const sf::Vector2f& end);
        ~Simulation() = default;
        Simulation(const Simulation&) = delete;
        Simulation& operator = (const Simulation&) = delete;

        const std::vector<Mass*>& getMasses() const;

        void reset();
        void solve();
        void simulate(float);
        void execute(float);

    private:
        std::vector<Mass*> m_masses;
        std::vector<Constraint*> m_constraints;
        float m_airFriction;
    };

    void draw(sf::RenderTarget&, sf::RenderStates) const override;
};

#endif //TAIL_DRAWABLE