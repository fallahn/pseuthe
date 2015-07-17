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

#include <iostream>

namespace
{
    sf::Uint64 uid = 1u; //use 0 for no entity
}

Entity::Entity(MessageBus& mb)
 : m_destroyed  (false),
 m_uid          (uid++),
 m_messageBus   (mb),
 m_parent       (nullptr),
 m_scene        (nullptr)
{}

//public
void Entity::addChild(Entity::Ptr& child)
{
    child->m_parent = this;
    child->setScene(m_scene);
    m_children.push_back(std::move(child));
}

Entity::Ptr Entity::removeChild(Entity& child)
{
    auto result = std::find_if(m_children.begin(), m_children.end(),
        [&child](const Ptr& p)
    {
        return (p.get() == &child);
    });

    if (result != m_children.end())
    {
        Ptr found = std::move(*result);
        found->m_parent = nullptr;
        found->m_scene = nullptr;
        m_children.erase(result);
        return found;
    }
    return nullptr;
}

const std::vector<Entity::Ptr>& Entity::getChildren() const
{
    return m_children;
}

sf::Vector2f Entity::getWorldPosition() const
{
    return getWorldTransform() * sf::Vector2f();
}

sf::Transform Entity::getWorldTransform() const
{
    auto t = sf::Transform::Identity;
    for (const auto* ent = this; ent != nullptr; ent = ent->m_parent)
        t = ent->getTransform() * t;

    return t;
}

void Entity::setWorldPosition(sf::Vector2f position)
{
    if (m_parent) position -= m_parent->getWorldPosition();
    setPosition(position);
}

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


    //mark self as deleted if no components remain
    if (m_components.empty())
    {
        destroy();
    }

    //update all children
    std::vector<Entity*> deadChildren;
    for (auto& c : m_children)
    {
        c->update(dt);
        if (c->destroyed())
            deadChildren.push_back(&(*c));
    }

    //remove any dead children (there's a line I never thought I'd write...)
    for (const auto& dc : deadChildren)
    {
        removeChild(*dc);
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
    //case Message::Type::Physics:
    //    
    //    if ((msg.physics.entityId[0] == m_uid || msg.physics.entityId[1] == m_uid))
    //    {


    //    }
    //    break;
    case Message::Type::ComponentSystem:
        if (msg.component.entityId == m_uid)
        {
            switch (msg.component.action)
            {
            case Message::ComponentEvent::Deleted:
                m_components.erase(std::remove_if(m_components.begin(), m_components.end(),
                    [&msg](const Component::Ptr& p)
                {
                    return msg.component.ptr == p.get();
                }), m_components.end());

                m_drawables.erase(std::remove_if(m_drawables.begin(), m_drawables.end(),
                    [&msg](const sf::Drawable* p)
                {
                    return msg.component.ptr == (Component*)p;
                }), m_drawables.end());
                break;
            default: break;
            }
        }
        break;
    default: break;
    }

    //pass down to children
    for (auto& c : m_children)
        c->handleMessage(msg);
}

void Entity::setScene(Scene* scene)
{
    m_scene = scene;
    for (auto& c : m_children) c->setScene(scene);
}

//private
void Entity::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    states.transform *= getTransform();    

    for (const auto& c : m_children)
        rt.draw(*c, states);

    drawSelf(rt, states);
}

void Entity::drawSelf(sf::RenderTarget& rt, sf::RenderStates states) const
{
    for (const auto& d : m_drawables)
    {
        rt.draw(*d, states);
    }
}