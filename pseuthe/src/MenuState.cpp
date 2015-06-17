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

#include <MenuState.hpp>
#include <Util.hpp>
#include <App.hpp>

#include <UISlider.hpp>
#include <UICheckBox.hpp>
#include <UIComboBox.hpp>
#include <UIButton.hpp>

#include <SFML/Window/Event.hpp>

MenuState::MenuState(StateStack& stateStack, Context context)
    : State(stateStack, context)
{
    m_menuSprite.setTexture(context.appInstance.getTexture("assets/images/main_menu.png"));
    m_menuSprite.setPosition(context.defaultView.getCenter());
    Util::Position::centreOrigin(m_menuSprite);

    auto& font = context.appInstance.getFont("assets/fonts/N_E_B.ttf");

    m_texts.emplace_back("Press Space to Toggle Menu", font, 35u);
    auto& spaceText = m_texts.back();
    Util::Position::centreOrigin(spaceText);
    spaceText.setPosition(context.defaultView.getCenter());
    spaceText.move(0.f, 400.f);

    //m_texts.emplace_back("Options", font, 35u);
    //auto& optionsText = m_texts.back();
    //Util::Position::centreOrigin(optionsText);
    //optionsText.setPosition(context.defaultView.getCenter());
    //optionsText.move(0.f, -100.f);
    //optionsText.setStyle(sf::Text::Bold);

    //m_texts.emplace_back("Volume:", *spaceText.getFont(), 30u);
    //m_texts.back().setPosition(540.f, 430.f);

    m_texts.emplace_back("https://github.com/fallahn/pseuthe", context.appInstance.getFont("assets/fonts/VeraMono.ttf"), 18u);
    m_texts.back().setPosition(1520.f, 1050.f);

    buildMenu(font);

    context.renderWindow.setMouseCursorVisible(true);
}

MenuState::~MenuState()
{
    getContext().renderWindow.setMouseCursorVisible(false);
}

//public
bool MenuState::update(float dt)
{
    return true;
}

void MenuState::draw()
{
    auto& rw = getContext().renderWindow;
    rw.setView(getContext().defaultView);
    rw.draw(m_menuSprite);

    rw.draw(m_uiContainer);

    for (const auto& t : m_texts)
    {
        rw.draw(t);
    }
}

bool MenuState::handleEvent(const sf::Event& evt)
{
    if (evt.type == sf::Event::KeyReleased)
    {
        switch (evt.key.code)
        {
        case sf::Keyboard::Space:
            requestStackPop();
            return false;
        default: break;
        }
    }
    
    //pass remaining events to menu
    const auto& rw = getContext().renderWindow;
    auto mousePos = rw.mapPixelToCoords(sf::Mouse::getPosition(rw));

    m_uiContainer.handleEvent(evt, mousePos);

    return false; //consume events
}

//private
void MenuState::buildMenu(const sf::Font& font)
{
    auto soundSlider = std::make_shared<ui::Slider>(font, getContext().appInstance.getTexture("assets/images/ui/slider_handle.png"));
    soundSlider->setAlignment(ui::Alignment::TopLeft);
    soundSlider->setPosition(780.f, 550.f);
    soundSlider->setText("Volume");
    //TODO get settings for current volume
    soundSlider->setCallback([this](const ui::Slider* slider)
    {
        //TODO send volume setting command
    }, ui::Slider::Event::ValueChanged);
    m_uiContainer.addControl(soundSlider);

    auto muteCheckbox = std::make_shared<ui::CheckBox>(font, getContext().appInstance.getTexture("assets/images/ui/checkbox.png"));
    muteCheckbox->setPosition(1100.f, 550.f);
    muteCheckbox->setAlignment(ui::Alignment::Centre);
    muteCheckbox->setText("Mute");
    m_uiContainer.addControl(muteCheckbox);
    //TODO add 'check changed' callback

    auto resolutionBox = std::make_shared<ui::ComboBox>(font, getContext().appInstance.getTexture("assets/images/ui/combobox.png"));
    //resolution->setAlignment(ui::Alignment::Centre);
    resolutionBox->setPosition(780.f, 600.f);

    const auto& modes = getContext().appInstance.getVideoSettings().AvailableVideoModes;
    auto i = 0u;
    auto j = 0u;
    for (const auto& m : modes)
    {
        std::string name = std::to_string(m.width) + " x " + std::to_string(m.height);
        sf::Int32 val = (m.width << 16) | m.height;
        resolutionBox->addItem(name, val);
        //select currently active mode
        if (getContext().appInstance.getVideoSettings().VideoMode != m)
            i++;
        else
            j = i;
    }
    if (i < modes.size()) resolutionBox->setSelectedIndex(j);

    m_uiContainer.addControl(resolutionBox);


    auto applyButton = std::make_shared<ui::Button>(font, getContext().appInstance.getTexture("assets/images/ui/button.png"));
    applyButton->setText("Apply");
    applyButton->setPosition(1000.f, 595.f);
    applyButton->setCallback([](){}); //TODO handle click
    m_uiContainer.addControl(applyButton);
}