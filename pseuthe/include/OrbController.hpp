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

//handles logic for the floating orbs

#ifndef ORB_CONTROLLER_HPP_
#define ORB_CONTROLLER_HPP_

#include <Component.hpp>

class ParticleSystem;
class AnimatedDrawable;
class PhysicsComponent;
class OrbController final : public Component
{
public:
    explicit OrbController(MessageBus&);
    ~OrbController() = default;

    Component::Type type() const override;
    void entityUpdate(Entity&, float) override;
    void handleMessage(const Message&) override;
    void onStart(Entity&) override;

private:

    ParticleSystem* m_particleSystem;
    AnimatedDrawable* m_drawable;
    PhysicsComponent* m_physComponent;
};

#endif //ORB_CONTROLLER_HPP_