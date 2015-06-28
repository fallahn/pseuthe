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
#include <ScoreState.hpp>
#include <Util.hpp>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <algorithm>
#include <fstream>
#include <cstring>

using namespace std::placeholders;

namespace
{
    const float timePerFrame = 1.f / 60.f;

    sf::Clock frameClock;
    float timeSinceLastUpdate = 0.f;

    const std::string windowTitle("pseuthe");
    const std::string settingsFile("settings.set");

    int lastScoreIndex = 0;
}

App::App()
    : m_videoSettings   (),
    m_renderWindow      (m_videoSettings.VideoMode, windowTitle, m_videoSettings.WindowStyle),
    m_stateStack        ({ m_renderWindow, *this }),
    m_difficulty        (Difficulty::Easy)
{
    registerStates();
    m_stateStack.pushState(States::ID::Main);
    m_stateStack.pushState(States::ID::Menu);

    loadSettings();
    m_scores.load();

    m_renderWindow.setVerticalSyncEnabled(m_videoSettings.VSync);

    //store available modes and remove unusable
    m_videoSettings.AvailableVideoModes = sf::VideoMode::getFullscreenModes();
    m_videoSettings.AvailableVideoModes.erase(std::remove_if(m_videoSettings.AvailableVideoModes.begin(), m_videoSettings.AvailableVideoModes.end(),
        [](const sf::VideoMode& vm)
    {
        return (!vm.isValid() || vm.bitsPerPixel != 32);
    }), m_videoSettings.AvailableVideoModes.end());
    std::reverse(m_videoSettings.AvailableVideoModes.begin(), m_videoSettings.AvailableVideoModes.end());

    update = std::bind(&App::updateApp, this, _1);
}

//public
void App::run()
{
    if (!sf::Shader::isAvailable())
    {
        Logger::Log("Shaders reported as unavailable.", Logger::Type::Error, Logger::Output::File);
        return;
    }

    m_renderWindow.setMouseCursorVisible(false);

    frameClock.restart();
    while (m_renderWindow.isOpen())
    {
        float elapsedTime = frameClock.restart().asSeconds();
        timeSinceLastUpdate += elapsedTime;

        while (timeSinceLastUpdate > timePerFrame)
        {
            timeSinceLastUpdate -= timePerFrame;

            handleEvents();
            handleMessages();
            update(timePerFrame);
        }
        draw();
    }

    saveSettings();
    m_scores.save();
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

const App::AudioSettings& App::getAudioSettings() const
{
    return m_audioSettings;
}

void App::setAudioSettings(AudioSettings as)
{
    m_audioSettings = as;
}

const App::VideoSettings& App::getVideoSettings() const
{
    return m_videoSettings;
}

void App::applyVideoSettings(const VideoSettings& settings)
{
    if (m_videoSettings == settings) return;

    auto availableModes = m_videoSettings.AvailableVideoModes;

    m_renderWindow.create(settings.VideoMode, windowTitle, settings.WindowStyle);
    m_renderWindow.setVerticalSyncEnabled(settings.VSync);
    m_renderWindow.setMouseCursorVisible(false);
    //TODO test validity and restore old settings if possible
    m_videoSettings = settings;
    m_videoSettings.AvailableVideoModes = availableModes;
    m_stateStack.updateView();
}

sf::Font& App::getFont(const std::string& path)
{
    return m_fontResource.get(path);
}

sf::Texture& App::getTexture(const std::string& path)
{
    return m_textureResource.get(path);
}

MessageBus& App::getMessageBus()
{
    return m_messageBus;
}

void App::addScore(const std::string& name, float value)
{
    lastScoreIndex = m_scores.add(name, value, m_difficulty);
}

int App::getLastScoreIndex() const
{
    return lastScoreIndex;
}

const std::vector<Scores::Item>& App::getScores() const
{
    return m_scores.getScores(m_difficulty);
}

void App::setDifficulty(Difficulty d)
{
    m_difficulty = d;
}

Difficulty App::getDifficulty() const
{
    return m_difficulty;
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

#ifdef _DEBUG_
        if (evt.type == sf::Event::KeyPressed
            && evt.key.code == sf::Keyboard::Escape)
        {
            m_renderWindow.close();
        }
#endif //_DEBUG_

        if (evt.type == sf::Event::KeyPressed
            && evt.key.code == sf::Keyboard::F5)
        {
            saveScreenshot();
        }

        m_stateStack.handleEvent(evt);
    }
}

void App::handleMessages()
{
    while (!m_messageBus.empty())
    {
        m_stateStack.handleMessage(m_messageBus.poll());
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
    m_stateStack.registerState<ScoreState>(States::ID::Score);
}

void App::loadSettings()
{
    std::fstream file(settingsFile, std::ios::binary | std::ios::in);
    if (!file.good() || !file.is_open() || file.fail())
    {
        Logger::Log("failed to open settings file for reading", Logger::Type::Error, Logger::Output::All);
        file.close();
        return;
    }

    //check file size
    file.seekg(0, std::ios::end);
    int fileSize = static_cast<int>(file.tellg());
    file.seekg(0, std::ios::beg);

    int expectedSize = sizeof(m_videoSettings.VideoMode) + sizeof(m_videoSettings.WindowStyle) + sizeof(AudioSettings) + sizeof(Difficulty);

    if (fileSize != expectedSize)
    {
        Logger::Log("settings file not expected file size", Logger::Type::Error, Logger::Output::All);
        file.close();
        return;
    }

    std::vector<char> data(expectedSize);
    file.read(&data[0], expectedSize);
    file.close();

    VideoSettings  newVideoSettings;

    char* dataPtr = &data[0];
    std::memcpy(&newVideoSettings.VideoMode, dataPtr, sizeof(m_videoSettings.VideoMode));
    dataPtr += sizeof(m_videoSettings.VideoMode);

    std::memcpy(&newVideoSettings.WindowStyle, dataPtr, sizeof(m_videoSettings.WindowStyle));
    dataPtr += sizeof(m_videoSettings.WindowStyle);

    std::memcpy(&m_audioSettings, dataPtr, sizeof(m_audioSettings));
    m_audioSettings.volume = Util::Math::clamp(m_audioSettings.volume, 0.f, 1.f);
    dataPtr += sizeof(AudioSettings);

    std::memcpy(&m_difficulty, dataPtr, sizeof(Difficulty));

    applyVideoSettings(newVideoSettings);
}

void App::saveSettings()
{
    std::fstream file(settingsFile, std::ios::binary | std::ios::out);
    if (!file.good() || !file.is_open() || file.fail())
    {
        Logger::Log("failed to open settings file for writing", Logger::Type::Error, Logger::Output::All);
        file.close();
        return;
    }

    std::vector<char> data(sizeof(m_videoSettings.VideoMode) + sizeof(m_videoSettings.WindowStyle) + sizeof(AudioSettings) + sizeof(Difficulty));
    char* dataPtr = &data[0];
    std::memcpy(dataPtr, &m_videoSettings.VideoMode, sizeof(m_videoSettings.VideoMode));
    dataPtr += sizeof(m_videoSettings.VideoMode);

    std::memcpy(dataPtr, &m_videoSettings.WindowStyle, sizeof(m_videoSettings.WindowStyle));
    dataPtr += sizeof(m_videoSettings.WindowStyle);

    std::memcpy(dataPtr, &m_audioSettings, sizeof(m_audioSettings));
    dataPtr += sizeof(m_audioSettings);

    std::memcpy(dataPtr, &m_difficulty, sizeof(Difficulty));

    file.write(&data[0], data.size());
    file.close();
}

void App::saveScreenshot()
{
    std::time_t time = std::time(NULL);
    struct tm* timeInfo;

    timeInfo = std::localtime(&time);

    char buffer[40];
    std::string fileName;

    strftime(buffer, 40, "screenshot%d_%m_%y_%H_%M_%S.png", timeInfo);

    fileName.assign(buffer);

    sf::Image screenCap = m_renderWindow.capture();
    if (!screenCap.saveToFile(fileName)) Logger::Log("failed to save " + fileName, Logger::Type::Error, Logger::Output::File);
}