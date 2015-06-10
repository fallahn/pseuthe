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

#include <Entity.hpp>
#include <Component.hpp>
#include <MessageBus.hpp>
#include <EchoDrawable.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

namespace
{
    sf::Uint64 uid = 0u;
}

Entity::Entity()
 : m_destroyed  (false),
 m_uid          (uid++)
{}

//public
void Entity::update(float dt)
{
    //remove destroyed components
    m_components.erase(std::remove_if(m_components.begin(), m_components.end(),
        [&](const Component::Ptr& p)
    {
        if (p->destroyed())
        {
            if (p->type() == Component::Type::Drawable)
            {
                auto r = std::find(m_drawables.begin(), m_drawables.end(),
                    dynamic_cast<sf::Drawable*>(p.get()));

                if (r != m_drawables.end()) m_drawables.erase(r);
            }
            return true;
        }
        return false;
    }), m_components.end());

    //mark self as deleted if no components remain
    if (m_components.empty()) destroy();

    //allow entity / components to update each other
    for (auto& c : m_components)
    {
        c->entityUpdate(*this, dt);
    }

    //copy any new components we may have aquired via update
    for (auto& c : m_pendingComponents)
    {
        m_components.push_back(std::move(c));
    }
    m_pendingComponents.clear();
}

void Entity::destroy()
{
    for (auto& c : m_components)
        c->destroy();
    m_destroyed = true;
}

bool Entity::destroyed() const
{
    return m_destroyed;
}

sf::Uint64 Entity::getUID() const
{
    return m_uid;
}

void Entity::handleMessage(const Message& msg)
{
    for (auto& c : m_components)
        c->handleMessage(msg);
    
    switch (msg.type)
    {
    case Message::Type::Physics:
        break;
    default: break;
    }
}

//private
void Entity::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    states.transform *= getTransform();
    for (auto& d : m_drawables)
    {
        rt.draw(*d, states);
    }
}