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
#include <Log.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

Entity::Entity()
 : m_destroyed(false)
{}

//public
void Entity::update(float dt)
{
    //remove deleted components
    void* dp = nullptr;
    m_components.erase(std::remove_if(m_components.begin(), m_components.end(),
        [&dp](const Component::Ptr& p)
    {
        if (p->destroyed())
        {
            dp = p.get();
            return true;
        }
        return false;
    }), m_components.end());
    if (dp)
    {
        m_drawables.erase(std::remove_if(m_drawables.begin(), m_drawables.end(),
            [dp](const sf::Drawable* p)
        {
            return p == dp;
        }), m_drawables.end());
    }

    //mark self as deleted if no components remain
    if (m_components.empty()) destroy();

    for (auto& c : m_components)
    {
        c->entityUpdate(*this, dt);
    }
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

//private
void Entity::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    states.transform *= getTransform();
    for (auto& d : m_drawables)
    {
        rt.draw(*d, states);
    }
}