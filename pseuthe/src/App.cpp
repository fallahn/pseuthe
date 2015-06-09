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

#include <App.hpp>
#include <MenuState.hpp>
#include <GameState.hpp>
#include <PauseState.hpp>

#include <SFML/Window/Event.hpp>

#include <algorithm>

using namespace std::placeholders;

namespace
{
    const float timePerFrame = 1.f / 60.f;

    sf::Clock frameClock;
    float timeSinceLastUpdate = 0.f;

    const std::string windowTitle("pseuthe");
}

App::App()
    : m_videoSettings   (),
    m_renderWindow      (m_videoSettings.VideoMode, windowTitle, m_videoSettings.WindowStyle),
    m_stateStack        ({ m_renderWindow, *this })
{
    registerStates();
    m_stateStack.pushState(States::ID::Main);
    m_stateStack.pushState(States::ID::Menu);

    m_renderWindow.setVerticalSyncEnabled(m_videoSettings.VSync);

    //store available modes and remove unusable
    m_videoSettings.AvailableVideoModes = sf::VideoMode::getFullscreenModes();
    m_videoSettings.AvailableVideoModes.erase(std::remove_if(m_videoSettings.AvailableVideoModes.begin(), m_videoSettings.AvailableVideoModes.end(),
        [](const sf::VideoMode& vm)
    {
        return (!vm.isValid() || vm.bitsPerPixel != 32);
    }), m_videoSettings.AvailableVideoModes.end());


    update = std::bind(&App::updateApp, this, _1);
}

//public
void App::run()
{
    frameClock.restart();
    while (m_renderWindow.isOpen())
    {
        float elapsedTime = frameClock.restart().asSeconds();
        timeSinceLastUpdate += elapsedTime;

        while (timeSinceLastUpdate > timePerFrame)
        {
            timeSinceLastUpdate -= timePerFrame;

            handleEvents();
            update(timePerFrame);
        }
        draw();
    }
}

void App::pause()
{
    update = std::bind(&App::pauseApp, this, _1);
}

void App::resume()
{
    update = std::bind(&App::updateApp, this, _1);
    frameClock.restart();
    timeSinceLastUpdate = 0.f;
}

const App::VideoSettings& App::getVideoSettings() const
{
    return m_videoSettings;
}

sf::Font& App::getFont(const std::string& path)
{
    return m_fontResource.get(path);
}

sf::Texture& App::getTexture(const std::string& path)
{
    return m_textureResource.get(path);
}

//private
void App::handleEvents()
{
    sf::Event evt;
    while (m_renderWindow.pollEvent(evt))
    {
        switch (evt.type)
        {
        case sf::Event::LostFocus:
            pause();
            continue;
        case sf::Event::GainedFocus:
            resume();
            continue;
        case sf::Event::Closed:
            m_renderWindow.close();
            return;
        default: break;
        }

        //-----TODO make only debug-----
        if (evt.type == sf::Event::KeyPressed
            && evt.key.code == sf::Keyboard::Escape)
        {
            m_renderWindow.close();
        }
        //------------------------------

        m_stateStack.handleEvent(evt);
    }
}

void App::updateApp(float dt)
{
    m_stateStack.update(dt);
}

void App::pauseApp(float dt)
{
    //do nothing, we're paused!
}

void App::draw()
{
    m_renderWindow.clear(sf::Color::Black);
    m_stateStack.draw();
    m_renderWindow.display();
}

void App::registerStates()
{
    m_stateStack.registerState<MenuState>(States::ID::Menu); 
    m_stateStack.registerState<GameState>(States::ID::Main);
    m_stateStack.registerState<PauseState>(States::ID::Pause);
}