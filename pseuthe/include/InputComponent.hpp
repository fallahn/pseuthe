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

#include <SFML/System/Vector2.hpp>

class PhysicsComponent;
class AnimatedDrawable;
class ParticleSystem;
class InputComponent final : public Component
{
public:
    explicit InputComponent(MessageBus&);
    ~InputComponent() = default;

    Component::Type type() const override;
    void entityUpdate(Entity&, float) override;
    void handleMessage(const Message&) override;
    void onStart(Entity&) override;

private:

    PhysicsComponent* m_physicsComponent;
    AnimatedDrawable* m_drawable;
    ParticleSystem* m_trailParticles;

    float m_health;
    bool m_parseInput;

    float m_mass;
    float m_invMass;

    sf::Vector2f getKeyboard();
    sf::Vector2f getController();
};

#endif //INPUT_COMPONENT_HPP_