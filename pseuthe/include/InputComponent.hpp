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

//handles real time input for player

#ifndef INPUT_COMPONENT_HPP_
#define INPUT_COMPONENT_HPP_

#include <Component.hpp>
#include <StateIds.hpp>

#include <SFML/System/Vector2.hpp>

#include <functional>

class PhysicsComponent;
class AnimatedDrawable;
class ParticleSystem;
class TailDrawable;
class InputComponent final : public Component
{
public:

    explicit InputComponent(MessageBus&);
    ~InputComponent() = default;

    Component::Type type() const override;
    void entityUpdate(Entity&, float) override;
    void handleMessage(const Message&) override;
    void onStart(Entity&) override;

    void setControlType(ControlType);
    void setControllerEnabled(bool);

private:
    using ControlFunction = std::function<sf::Vector2f(float)>;

    PhysicsComponent* m_physicsComponent;
    AnimatedDrawable* m_headDrawable;
    AnimatedDrawable* m_mouthDrawable;
    AnimatedDrawable* m_wigglerA;
    AnimatedDrawable* m_wigglerB;
    ParticleSystem* m_trailParticles;
    ParticleSystem* m_sparkleParticles;
    ParticleSystem* m_echo;
    TailDrawable* m_tail;

    float m_health;
    bool m_parseInput;
    bool m_controllerEnabled;
    bool m_mouseClickPending;
    sf::Vector2f m_mouseClick;

    float m_mass;
    float m_invMass;

    ControlFunction getKeyboard;
    ControlFunction getController;

    sf::Vector2f getKeyboardClassic(float);
    sf::Vector2f getControllerClassic(float);

    sf::Vector2f getKeyboardArcade(float);
    sf::Vector2f getControllerArcade(float);

    sf::Vector2f getMouse(float, const sf::Vector2f&);
};

#endif //INPUT_COMPONENT_HPP_