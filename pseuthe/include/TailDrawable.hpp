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
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include <vector>
#include <memory>
#include <utility>

class TailDrawable final : public Component, public sf::Drawable, public sf::Transformable
{
public:
    explicit TailDrawable(MessageBus&);
    ~TailDrawable() = default;

    Component::Type type() const override;
    void entityUpdate(Entity&, float) override;
    void handleMessage(const Message&) override;
    void onStart(Entity&) override;

    void setColour(const sf::Color&);

    void addTail(const sf::Vector2f&);

private:

    struct Mass final
    {
        using Ptr = std::unique_ptr<Mass>;
        explicit Mass(float);
        ~Mass() = default;

        float getMass() const;
        const sf::Vector2f& getVelocity() const;
        const sf::Vector2f& getForce() const;
        const sf::Vector2f& getPosition() const;

        void setVelocity(const sf::Vector2f&);
        void setPosition(const sf::Vector2f&);

        void reset();
        void applyForce(const sf::Vector2f&);
        void simulate(float);

    private:
        float m_mass;
        sf::Vector2f m_velocity;
        sf::Vector2f m_force;
        sf::Vector2f m_position;

    };

    struct Constraint final
    {
        using Ptr = std::unique_ptr<Constraint>;

        Constraint(Mass&, Mass&);
        ~Constraint() = default;

        void solve();

    private:
        Mass& m_massA;
        Mass& m_massB;
    };

    class Simulation final
    {
    public:
        using Ptr = std::unique_ptr<Simulation>;

        Simulation(const sf::Vector2f& start, const sf::Vector2f& end);
        ~Simulation() = default;
        Simulation(const Simulation&) = delete;
        Simulation& operator = (const Simulation&) = delete;

        const std::vector<Mass::Ptr>& getMasses() const;
        void setAnchor(const sf::Vector2f&);
        float getScale() const;
        void setPosition(const sf::Vector2f&);

        void reset();
        void solve();
        void simulate(float);
        void update(float);

    private:
        std::vector<Mass::Ptr> m_masses;
        std::vector<Constraint::Ptr> m_constraints;
        sf::Vector2f m_anchor;
        sf::Vector2f m_layoutEnd;

        void layoutMasses();
    };
    
    std::vector<std::pair<Simulation::Ptr, sf::Vector2f>> m_simulations;
    sf::Color m_colour;
    float m_fadeTime;

    std::vector<float> m_wavetable;
    std::size_t m_currentIndex;

    void draw(sf::RenderTarget&, sf::RenderStates) const override;
};

#endif //TAIL_DRAWABLE