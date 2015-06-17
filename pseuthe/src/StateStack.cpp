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

#include <StateStack.hpp>

#include <SFML/Graphics/RenderWindow.hpp>

#include <cassert>

StateStack::StateStack(State::Context context)
    : m_context     (context)
{
    updateView();
}

//public
void StateStack::update(float dt)
{
    for (auto i = m_stack.rbegin(); i != m_stack.rend(); ++i)
    {
        if (!(*i)->update(dt)) break;
    }
    applyPendingChanges();
}

void StateStack::draw()
{
    for (auto& s : m_stack) s->draw();
}

void StateStack::handleEvent(const sf::Event& evt)
{
    for (auto i = m_stack.rbegin(); i != m_stack.rend(); ++i)
    {
        if (!(*i)->handleEvent(evt)) break;
    }
    applyPendingChanges();
}

void StateStack::handleMessage(const Message& msg)
{
    for (auto& s : m_stack) s->handleMessage(msg);
}

void StateStack::pushState(States::ID id)
{
    m_pendingChanges.emplace_back(Action::Push, id);
}

void StateStack::popState()
{
    m_pendingChanges.emplace_back(Action::Pop);
}

void StateStack::clearStates()
{
    m_pendingChanges.emplace_back(Action::Clear);
}

bool StateStack::empty() const
{
    return m_stack.empty();
}

sf::View StateStack::updateView()
{
    //calculate the correct view size / ratio for window size
    m_context.defaultView.setSize(1920.f, 1080.f);
    m_context.defaultView.setCenter(960.f, 540.f);

    float ratioX = static_cast<float>(m_context.renderWindow.getSize().x) / 16.f;
    float ratioY = static_cast<float>(m_context.renderWindow.getSize().y) / ratioX;

    if (ratioY != 9)
    {
        auto winSize = static_cast<sf::Vector2f>(m_context.renderWindow.getSize());
        float windowRatio = winSize.x / winSize.y;
        float viewRatio = 16.f / 9.f;

        float sizeY = windowRatio / viewRatio;
        m_context.defaultView.setViewport({ { 0.f, (1.f - sizeY) / 2.f }, { 1.f, sizeY } });
    }

    return m_context.defaultView;
}

//private
State::Ptr StateStack::createState(States::ID id)
{
    auto result = m_factories.find(id);
    assert(result != m_factories.end());

    return result->second();
}

void StateStack::applyPendingChanges()
{
    for (auto& change : m_pendingChanges)
    {
        switch (change.action)
        {
        case Action::Push:
            m_stack.emplace_back(createState(change.id));
            break;
        case Action::Pop:
            m_stack.pop_back();
            break;
        case Action::Clear:
            m_stack.clear();
            break;
        default: break;
        }
    }
    m_pendingChanges.clear();
}

//---------------------------------------

StateStack::Pendingchange::Pendingchange(Action a, States::ID i)
    : action    (a),
    id          (i)
{

}