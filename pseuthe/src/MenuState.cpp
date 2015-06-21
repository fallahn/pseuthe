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
#include <Log.hpp>

#include <UISlider.hpp>
#include <UICheckBox.hpp>
#include <UISelection.hpp>
#include <UIButton.hpp>

#include <SFML/Window/Event.hpp>

MenuState::MenuState(StateStack& stateStack, Context context)
    : State     (stateStack, context),
    m_messageBus(context.appInstance.getMessageBus())
{
    m_menuSprite.setTexture(context.appInstance.getTexture("assets/images/main_menu.png"));
    m_menuSprite.setPosition(context.defaultView.getCenter());
    Util::Position::centreOrigin(m_menuSprite);
    m_menuSprite.move(0.f, -40.f);

    m_cursorSprite.setTexture(context.appInstance.getTexture("assets/images/ui/cursor.png"));

    auto& font = context.appInstance.getFont("assets/fonts/N_E_B.ttf");

    m_texts.emplace_back("Press Space to Toggle Menu", font, 35u);
    auto& spaceText = m_texts.back();
    Util::Position::centreOrigin(spaceText);
    spaceText.setPosition(context.defaultView.getCenter());
    spaceText.move(0.f, 400.f);

    m_texts.emplace_back("https://github.com/fallahn/pseuthe", context.appInstance.getFont("assets/fonts/VeraMono.ttf"), 18u);
    m_texts.back().setPosition(1520.f, 1050.f);

    buildMenu(font);

    m_cursorSprite.setPosition(context.renderWindow.mapPixelToCoords(sf::Mouse::getPosition(context.renderWindow)));

    Message msg;
    msg.type = Message::Type::UI;
    msg.ui.type = Message::UIEvent::MenuOpened;
    msg.ui.value = 0.f;
    m_messageBus.send(msg);
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

    rw.draw(m_cursorSprite);
}

bool MenuState::handleEvent(const sf::Event& evt)
{
    if (evt.type == sf::Event::KeyReleased)
    {
        switch (evt.key.code)
        {
        case sf::Keyboard::Space:
        case sf::Keyboard::Escape:
            requestStackPop();

            Message msg;
            msg.type = Message::Type::UI;
            msg.ui.type = Message::UIEvent::MenuClosed;
            msg.ui.value = 0.f;
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

    return false; //consume events
}

void MenuState::handleMessage(const Message&)
{

}

//private
void MenuState::buildMenu(const sf::Font& font)
{
    //auto buttonClose = std::make_shared<ui::Button>(font, getContext().appInstance.getTexture("assets/images/ui/close.png"));
    //buttonClose->setPosition(1421.f, 332.f);
    //buttonClose->setCallback([this]()
    //{
    //    requestStackPop();
    //});
    //m_uiContainer.addControl(buttonClose);
    
    auto soundSlider = std::make_shared<ui::Slider>(font, getContext().appInstance.getTexture("assets/images/ui/slider_handle.png"), 375.f);
    soundSlider->setPosition(600.f, 510.f);
    soundSlider->setText("Volume");
    soundSlider->setMaxValue(1.f);
    soundSlider->setCallback([this](const ui::Slider* slider)
    {
        //send volume setting command
        Message msg;
        msg.type = Message::Type::UI;
        msg.ui.type = Message::UIEvent::RequestVolumeChange;
        msg.ui.value = slider->getValue();
        m_messageBus.send(msg);

        auto audioSettings = getContext().appInstance.getAudioSettings();
        audioSettings.volume = slider->getValue();
        getContext().appInstance.setAudioSettings(audioSettings);

    }, ui::Slider::Event::ValueChanged);
    soundSlider->setValue(getContext().appInstance.getAudioSettings().volume); //set this *after* callback is set
    m_uiContainer.addControl(soundSlider);

    auto muteCheckbox = std::make_shared<ui::CheckBox>(font, getContext().appInstance.getTexture("assets/images/ui/checkbox.png"));
    muteCheckbox->setPosition(1100.f, 470.f);
    muteCheckbox->setText("Mute");
    muteCheckbox->setCallback([this](const ui::CheckBox* checkBox)
    {
        Message msg;
        msg.type = Message::Type::UI;
        msg.ui.type = (checkBox->checked()) ? Message::UIEvent::RequestAudioMute : Message::UIEvent::RequestAudioUnmute;
        m_messageBus.send(msg);

        auto audioSettings = getContext().appInstance.getAudioSettings();
        audioSettings.muted = checkBox->checked();
        getContext().appInstance.setAudioSettings(audioSettings);
    }, ui::CheckBox::Event::CheckChanged);
    muteCheckbox->check(getContext().appInstance.getAudioSettings().muted);
    m_uiContainer.addControl(muteCheckbox);


    auto resolutionBox = std::make_shared<ui::Selection>(font, getContext().appInstance.getTexture("assets/images/ui/scroll_arrow.png"), 375.f);
    resolutionBox->setPosition(600.f, 600.f);

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

    auto fullscreenCheckbox = std::make_shared<ui::CheckBox>(font, getContext().appInstance.getTexture("assets/images/ui/checkbox.png"));
    fullscreenCheckbox->setPosition(1100.f, 600.f);
    fullscreenCheckbox->setText("Full Screen");
    fullscreenCheckbox->setCallback([this](const ui::CheckBox*)
    {

    }, ui::CheckBox::Event::CheckChanged);
    fullscreenCheckbox->check((getContext().appInstance.getVideoSettings().WindowStyle & sf::Style::Fullscreen) != 0);
    m_uiContainer.addControl(fullscreenCheckbox);

    auto applyButton = std::make_shared<ui::Button>(font, getContext().appInstance.getTexture("assets/images/ui/button.png"));
    applyButton->setText("Apply");
    applyButton->setAlignment(ui::Alignment::Centre);
    applyButton->setPosition(840.f, 755.f);
    applyButton->setCallback([fullscreenCheckbox, resolutionBox, this]()
    {
        auto res = resolutionBox->getSelectedValue();

        App::VideoSettings settings;
        settings.VideoMode.width = res >> 16;
        settings.VideoMode.height = res & 0xFFFF;
        settings.WindowStyle = (fullscreenCheckbox->checked()) ? sf::Style::Fullscreen : sf::Style::Close;
        getContext().appInstance.applyVideoSettings(settings);
    });
    m_uiContainer.addControl(applyButton);

    auto quitButton = std::make_shared<ui::Button>(font, getContext().appInstance.getTexture("assets/images/ui/button.png"));
    quitButton->setText("Quit");
    quitButton->setAlignment(ui::Alignment::Centre);
    quitButton->setPosition(1080.f, 755.f);
    quitButton->setCallback([this]()
    {
        getContext().renderWindow.close();
    });
    m_uiContainer.addControl(quitButton);
}