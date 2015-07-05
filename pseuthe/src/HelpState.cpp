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

#include <HelpState.hpp>
#include <App.hpp>
#include <Util.hpp>

#include <SFML/Window/Event.hpp>

namespace
{
    const sf::Uint32 charSize = 48u;
    const float fadeTime = 0.25f;
}

HelpState::HelpState(StateStack& stack, Context context)
    : State     (stack, context),
    m_mode      (Mode::FadeIn),
    m_messageBus(context.appInstance.getMessageBus()),
    m_fadeTime  (0.f)
{
    m_menuSprite.setTexture(context.appInstance.getTexture("assets/images/help_menu.png"));

    auto& font = context.appInstance.getFont("assets/fonts//N_E_B.ttf");

    sf::Vector2f centrePos = context.defaultView.getCenter();

    m_texts.emplace_back("How To Play", font, 80u);
    auto& titleText = m_texts.back();
    Util::Position::centreOrigin(titleText);
    titleText.setPosition(centrePos);
    titleText.move(0.f, -100.f);

    m_texts.emplace_back("Absorb green plankton to prevent starvation", font, charSize);
    auto& text01 = m_texts.back();
    Util::Position::centreOrigin(text01);
    text01.setPosition(centrePos);
    //text01.move(0.f, -10.f);

    m_texts.emplace_back("Beware red plankton which will reduce your health", font, charSize);
    auto& text02 = m_texts.back();
    Util::Position::centreOrigin(text02);
    text02.setPosition(centrePos);
    text02.move(0.f, 70.f);

    m_texts.emplace_back("Look out for special plankton to eat", font, charSize);
    auto& text03 = m_texts.back();
    Util::Position::centreOrigin(text03);
    text03.setPosition(centrePos);
    text03.move(0.f, 140.f);

    m_texts.emplace_back("Avoid collisions and live as long as you can.", font, charSize);
    auto& text04 = m_texts.back();
    Util::Position::centreOrigin(text04);
    text04.setPosition(centrePos);
    text04.move(0.f, 240.f);
}

bool HelpState::update(float dt)
{
    sf::Color colour = sf::Color::White;
    m_fadeTime += dt;
    switch (m_mode)
    {
    case Mode::FadeIn:
        if (m_fadeTime < fadeTime)
        {
            colour.a = static_cast<sf::Uint8>((m_fadeTime / fadeTime) * 255.f);
        }
        else
        {
            m_mode = Mode::Static;
        }
        break;
    case Mode::FadeOut:
        if (m_fadeTime < fadeTime)
        {
            colour.a = static_cast<sf::Uint8>(1.f - ((m_fadeTime / fadeTime) * 255.f));
        }
        else
        {
            Message msg;
            msg.type = Message::Type::UI;
            msg.ui.type = Message::UIEvent::MenuClosed;
            msg.ui.stateId = States::ID::Help;
            m_messageBus.send(msg);
            requestStackPop();
        }
        break;
    case Mode::Static:
    default: return true;
    }

    m_menuSprite.setColor(colour);
    for (auto& t : m_texts)
        t.setColor(colour);

    return true;
}

bool HelpState::handleEvent(const sf::Event& evt)
{
    switch (evt.type)
    {
    case sf::Event::KeyReleased:
    case sf::Event::MouseButtonReleased:
    case sf::Event::JoystickButtonReleased:
        m_fadeTime = 0.f;
        m_mode = Mode::FadeOut;
        break;
    default: break;
    }

    return false;
}

void HelpState::handleMessage(const Message&)
{

}

void HelpState::draw()
{
    auto& rw = getContext().renderWindow;

    rw.draw(m_menuSprite);

    for (const auto& t : m_texts)
        rw.draw(t);
}