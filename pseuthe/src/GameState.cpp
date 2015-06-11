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

#include <GameState.hpp>
#include <CircleDrawable.hpp>
#include <EchoDrawable.hpp>
#include <GradientDrawable.hpp>
#include <Log.hpp>
#include <Util.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

namespace
{
    Entity::Ptr createEntity(MessageBus& messageBus, PhysicsWorld& physicsWorld, const sf::Color& colour)
    {
        float size = static_cast<float>(Util::Random::value(10, 50));
        
        Entity::Ptr e = std::make_unique<Entity>();
        CircleDrawable::Ptr cd = std::make_unique<CircleDrawable>(size, messageBus);
        cd->setColour(colour);
        e->addComponent<CircleDrawable>(cd);

        PhysicsComponent::Ptr pc = physicsWorld.addBody(size, messageBus);
        e->addComponent<PhysicsComponent>(pc);

        return std::move(e);
    }

    const int nubbinCount = 18;
}

GameState::GameState(StateStack& stateStack, Context context)
    : State(stateStack, context)
{
    context.renderWindow.setView(context.defaultView);
    
    for (int i = 0; i < 9; ++i)
        m_scene.addEntity(createEntity(m_messageBus, m_physWorld, sf::Color::White), Scene::Layer::FrontFront);

    for (int i = 0; i < 6; ++i)
        m_scene.addEntity(createEntity(m_messageBus, m_physWorld, sf::Color(170u, 170u, 170u)), Scene::Layer::FrontMiddle);

    for (int i = 0; i < 3; ++i)
        m_scene.addEntity(createEntity(m_messageBus, m_physWorld, sf::Color(85u, 85u, 85u)), Scene::Layer::FrontRear);

    m_scene.getLayer(Scene::Layer::BackRear).addComponent<GradientDrawable>(std::make_unique<GradientDrawable>(m_messageBus));
}

bool GameState::update(float dt)
{    
    while (!m_messageBus.empty())
    {
        Message msg = m_messageBus.poll();
        m_physWorld.handleMessages(msg);
        m_scene.handleMessages(msg);
    }

    m_physWorld.update(dt);
    m_scene.update(dt);

    return true;
}

void GameState::draw()
{
    getContext().renderWindow.draw(m_scene);
}

bool GameState::handleEvent(const sf::Event& evt)
{

    return true;
}