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

namespace
{
    const int maxCollisions = 160;
}

Scene::Scene(MessageBus& mb)
    : m_collisionCount  (0),
    m_messageBus        (mb)
{
    for (int i = 0; i < Layer::Count; ++i)
        m_layers.emplace_back(std::make_unique<Entity>(mb));

    m_sceneBufferA.create(1920u, 1080u);
    m_sceneBufferB.create(1920u, 1080u);
}

//public
void Scene::update(float dt)
{
    for (auto& e : m_layers)
        e->update(dt);

    if (m_collisionCount > maxCollisions)
    {
        //send a message to change velocities
        m_collisionCount = 0;

        Message msg;
        msg.type = Message::Type::Entity;
        msg.entity.maxCollisionsReached = true;
        m_messageBus.send(msg);
    }

    m_chromeAbEffect.update(dt);
}

void Scene::handleMessage(const Message& msg)
{
    for (auto& e : m_layers)
        e->handleMessage(msg);

    if (msg.type == Message::Type::Physics)
    {
        if (msg.physics.event == Message::PhysicsEvent::Collided)
        {
            m_collisionCount++;
        }
    }
}

void Scene::addEntity(Entity::Ptr& entity, Layer layer)
{
    m_layers[layer]->addChild(std::move(entity));
}

Entity& Scene::getLayer(Layer l)
{
    return *m_layers[l];
}

void Scene::setView(const sf::View& v)
{
    m_sceneBufferA.setView(v);
}

//private
void Scene::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    m_sceneBufferA.clear();
    for (const auto& e : m_layers)
        m_sceneBufferA.draw(*e, states);
    m_sceneBufferA.display();

    m_bloomEffect.apply(m_sceneBufferA, m_sceneBufferB);
    m_chromeAbEffect.apply(m_sceneBufferB, rt);

}