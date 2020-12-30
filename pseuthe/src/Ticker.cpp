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

#include <Ticker.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

#include <cassert>

Ticker::Ticker(sf::Font& font)
    : m_font    (font),
    m_speed     (100.f),
    m_totalWidth(0.f)
{

}

//public
void Ticker::update(float dt)
{
    //move all messages
    for (auto i = m_messages.begin(); i != m_messages.end(); ++i)
    {
        if (i == m_messages.begin())
        {
            i->move(-m_speed * dt, 0.f);
        }
        else
        {
            auto j = std::prev(i);
            if (!j->getGlobalBounds().intersects(i->getGlobalBounds()))
            {
                i->move(-m_speed * dt, 0.f);
            }
        }

        auto lb = i->getLocalBounds();
        if (i->getPosition().x + lb.width < 0)
        {
            i->move(std::max(m_size.width, m_totalWidth) + lb.width, 0.f);
        }
    }

}

void Ticker::addItem(const std::string& text)
{
    m_messages.emplace_back(text, m_font, static_cast<sf::Uint32>(m_size.height * 0.8f));
    m_messages.back().setPosition(m_size.width, m_size.height * 0.1f);
    m_totalWidth += m_messages.back().getLocalBounds().width;
}

void Ticker::setSpeed(float speed)
{
    assert(speed > 0);
    m_speed = speed;
}

void Ticker::setSize(sf::FloatRect size)
{
    m_size = size;
}

std::size_t Ticker::getMessageCount() const
{
    return m_messages.size();
}

void Ticker::setColour(const sf::Color& colour)
{
    for (auto& t : m_messages)
    {
        t.setFillColor(colour);
    }
}

//private
void Ticker::draw(sf::RenderTarget& rt, sf::RenderStates states)const
{
    states.transform *= getTransform();
    for (const auto& t : m_messages)
        rt.draw(t, states);
}