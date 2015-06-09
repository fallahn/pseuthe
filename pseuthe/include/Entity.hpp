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

//an entity created defiend by its collection of components

#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <Log.hpp>

#include <Component.hpp>

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>

#include <memory>
#include <vector>

//TODO this class could form the basis of nodes in a scene graph
//decide if we want to add parenting if it seems necessary

//class Component;
class Entity final : public sf::Transformable, public sf::Drawable
{
public:
    using Ptr = std::unique_ptr<Entity>;

    Entity();
    ~Entity() = default;
    Entity(const Entity&) = delete;
    Entity& operator = (const Entity&) = delete;

    void update(float dt);

    template <typename T>
    void addComponent(std::unique_ptr<T>& component)
    {
        Component::Ptr c(static_cast<Component*>(component.release()));
        if (c->type() == Component::Type::Drawable)
        {
            //store a reference to drawables so they can be drawn
            m_drawables.push_back(dynamic_cast<sf::Drawable*>(c.get()));
        }

        m_components.push_back(std::move(c));
    }

    void destroy();
    bool destroyed() const;

private:

    bool m_destroyed;

    std::vector<std::unique_ptr<Component>> m_components;
    std::vector<sf::Drawable*> m_drawables;

    void draw(sf::RenderTarget& rt, sf::RenderStates state) const override;
};

#endif //ENTITY_HPP_