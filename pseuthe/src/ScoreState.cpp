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

#include <ScoreState.hpp>
#include <App.hpp>
#include <Util.hpp>
#include <MessageBus.hpp>

#include <UIButton.hpp>

#include <SFML/Window/Event.hpp>

namespace
{
    const float scorePadding = 18.f;
}

ScoreState::ScoreState(StateStack& stateStack, Context context)
    : State(stateStack, context),
    m_messageBus(context.appInstance.getMessageBus())
{
    m_menuSprite.setTexture(context.appInstance.getTexture("assets/images/main_menu.png"));
    m_menuSprite.setPosition(context.defaultView.getCenter());
    Util::Position::centreOrigin(m_menuSprite);
    m_menuSprite.move(0.f, -40.f);

    m_cursorSprite.setTexture(context.appInstance.getTexture("assets/images/ui/cursor.png"));

    auto& menuFont = context.appInstance.getFont("assets/fonts/N_E_B.ttf");
    auto& scoreFont = context.appInstance.getFont("assets/fonts/Ardeco.ttf");

    m_texts.emplace_back("", menuFont, 35u);
    auto& titleText = m_texts.back();
    switch (context.appInstance.getDifficulty())
    {
    default:
    case Difficulty::Easy:
        titleText.setString("Scores: Easy");
        break;
    case Difficulty::Medium:
        titleText.setString("Scores: Medium");
        break;
    case Difficulty::Hard:
        titleText.setString("Scores: Hard");
        break;
    }
    Util::Position::centreOrigin(titleText);
    titleText.setPosition(context.defaultView.getCenter());
    titleText.move(0.f, -206.f);



    //scores
    const auto& scores = context.appInstance.getScores();
    int index = context.appInstance.getLastScoreIndex();
    int start = std::max(0, index - 2);
    int end = std::min(index + 3, static_cast<int>(scores.size()));

    for (int i = start; i < end; ++i)
    {
        m_texts.emplace_back(std::to_string(i + 1) + ".    " + std::string(scores[i].name) + " - " + std::to_string(scores[i].score), scoreFont, 42u);
        auto& text = m_texts.back();
        if (i == index) text.setStyle(sf::Text::Bold);
        Util::Position::centreOrigin(text);
        text.setPosition(context.defaultView.getCenter());
        int offset = i - index;
        text.move(0.f, (text.getLocalBounds().height + scorePadding) * offset);
        sf::Uint8 alpha = 255u - std::abs(offset) * 100u;
        sf::Color colour(255u, 255u, 255u, alpha);
        text.setColor(colour);
    }

    buildMenu(menuFont);

    m_cursorSprite.setPosition(context.renderWindow.mapPixelToCoords(sf::Mouse::getPosition(context.renderWindow)));

    Message msg;
    msg.type = Message::Type::UI;
    msg.ui.type = Message::UIEvent::MenuOpened;
    msg.ui.value = 0.f;
    msg.ui.stateId = States::ID::Score;
    m_messageBus.send(msg);
}

//public
bool ScoreState::update(float dt)
{
    return true;
}

void ScoreState::draw()
{
    auto& rw = getContext().renderWindow;
    rw.setView(getContext().defaultView);
    rw.draw(m_menuSprite);

    rw.draw(m_uiContainer);

    for (const auto& t : m_texts)
    {
        rw.draw(t);
    }

    rw.draw(m_cursorSprite);
}

bool ScoreState::handleEvent(const sf::Event& evt)
{
    if (evt.type == sf::Event::KeyReleased)
    {
        switch (evt.key.code)
        {
        case sf::Keyboard::Space:
        case sf::Keyboard::Escape:
        case sf::Keyboard::Return:
            requestStackPop();
            requestStackPush(States::ID::Menu);

            Message msg;
            msg.type = Message::Type::UI;
            msg.ui.type = Message::UIEvent::MenuClosed;
            msg.ui.value = 0.f;
            msg.ui.stateId = States::ID::Score;
            m_messageBus.send(msg);
            return false;
        default: break;
        }
    }
    else if (evt.type == sf::Event::JoystickButtonReleased)
    {
        switch (evt.joystickButton.button)
        {
        case 7:
            requestStackPop();
            requestStackPush(States::ID::Menu);

            Message msg;
            msg.type = Message::Type::UI;
            msg.ui.type = Message::UIEvent::MenuClosed;
            msg.ui.value = 0.f;
            msg.ui.stateId = States::ID::Score;
            m_messageBus.send(msg);
            return false;
        default: break;
        }
    }


    //pass remaining events to menu
    const auto& rw = getContext().renderWindow;
    auto mousePos = rw.mapPixelToCoords(sf::Mouse::getPosition(rw));

    m_uiContainer.handleEvent(evt, mousePos);
    m_cursorSprite.setPosition(mousePos);
    return false;
}

void ScoreState::handleMessage(const Message&)
{

}

//private
void ScoreState::buildMenu(const sf::Font& font)
{
    auto applyButton = std::make_shared<ui::Button>(font, getContext().appInstance.getTexture("assets/images/ui/button.png"));
    applyButton->setText("OK");
    applyButton->setAlignment(ui::Alignment::Centre);
    applyButton->setPosition(960.f, 875.f);
    applyButton->setCallback([this]()
    {
        requestStackPop();
        requestStackPush(States::ID::Menu);

        Message msg;
        msg.type = Message::Type::UI;
        msg.ui.type = Message::UIEvent::MenuClosed;
        msg.ui.value = 0.f;
        msg.ui.stateId = States::ID::Score;
        m_messageBus.send(msg);
    });
    m_uiContainer.addControl(applyButton);

}