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
#include <Log.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

GameState::GameState(StateStack& stateStack, Context context)
    : State(stateStack, context)
{
    context.renderWindow.setView(context.defaultView);

    Entity::Ptr e = std::make_unique<Entity>();
    CircleDrawable::Ptr cd = std::make_unique<CircleDrawable>(10.f, m_messageBus);
    e->addComponent<CircleDrawable>(cd);

    m_entities.push_back(std::move(e));
}

bool GameState::update(float dt)
{
    //TODO encapsulate entities into a scene
    m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(),
        [](const Entity::Ptr& p)
    {
        return p->destroyed();
    }), m_entities.end());

    for (auto& e : m_entities)
    {
        e->update(dt);
    }

    return true;
}

void GameState::draw()
{
    for (const auto& e : m_entities)
    {
        getContext().renderWindow.draw(*e);
    }
}

bool GameState::handleEvent(const sf::Event& evt)
{
    if (evt.type == sf::Event::KeyPressed && evt.key.code == sf::Keyboard::Return)
    {
        m_entities[0]->destroy();
    }
    return true;
}