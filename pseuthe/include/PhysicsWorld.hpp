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

//simulates the physics and applies the results to physics components

#ifndef PHYS_WORLD_HPP_
#define PHYS_WORLD_HPP_

#include <PhysicsComponent.hpp>

#include <vector>
#include <set>

class PhysicsWorld final
{
public:
    PhysicsWorld();
    ~PhysicsWorld() = default;
    PhysicsWorld(const PhysicsWorld&) = delete;
    const PhysicsWorld& operator = (const PhysicsWorld&) = delete;

    PhysicsComponent::Ptr addBody(float, MessageBus&);
    void handleMessages(const Message&);
    void update(float);

private:
    using Collision = std::pair<PhysicsComponent*, PhysicsComponent*>;

    std::vector<PhysicsComponent*> m_bodies;
    std::set<Collision> m_collisions;
};

#endif //PHYS_WORLD_HPP_