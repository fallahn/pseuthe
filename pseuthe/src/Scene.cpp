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

#include <Scene.hpp>
#include <MessageBus.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

Scene::Scene()
{
    for (int i = 0; i < Layer::Count; ++i)
        m_layers.emplace_back(std::make_unique<Entity>());
}

//public
void Scene::update(float dt)
{
    for (auto& e : m_layers)
        e->update(dt);
}

void Scene::handleMessages(const Message& msg)
{
    for (auto& e : m_layers)
        e->handleMessage(msg);
}

void Scene::addEntity(Entity::Ptr& entity, Layer layer)
{
    m_layers[layer]->addChild(std::move(entity));
}

Entity& Scene::getLayer(Layer l)
{
    return *m_layers[l];
}

//private
void Scene::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    for (const auto& e : m_layers)
        rt.draw(*e, states);
}