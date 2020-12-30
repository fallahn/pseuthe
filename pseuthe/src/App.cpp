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
#include <HelpState.hpp>
#include <TitleState.hpp>
#include <Util.hpp>
#include <Icon.hpp>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <algorithm>
#include <fstream>
#include <cstring>

//Needed to access resources on OS X
#ifdef __APPLE__
#include <ResourcePath.hpp>
#endif

using namespace std::placeholders;

namespace
{
    const float timePerFrame = 1.f / 60.f;

    sf::Clock frameClock;
    float timeSinceLastUpdate = 0.f;

    const std::string windowTitle("pseuthe");
    const std::string settingsFile("settings.set");

    int lastScoreIndex = 0;

    int settingsIdent = 0xfc41414b;
    int settingsVersion = 6;
}

App::App()
    : m_videoSettings   (),
    m_renderWindow      (m_videoSettings.VideoMode, windowTitle, m_videoSettings.WindowStyle),
    m_stateStack        ({ m_renderWindow, *this }),
    m_pendingDifficulty (Difficulty::Easy)
{
    registerStates();
#ifndef P_DEBUG_
    m_stateStack.pushState(States::ID::Title);
#else
    m_stateStack.pushState(States::ID::Main);
    m_stateStack.pushState(States::ID::Menu);
#endif //P_DEBUG_

    //loadSettings();
    m_scores.load();

    m_renderWindow.setVerticalSyncEnabled(m_videoSettings.VSync);
    m_renderWindow.setIcon(icon_width, icon_height, icon_arr);

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

    m_stateStack.clearStates();
    m_stateStack.applyPendingChanges();
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

const App::VideoSettings& App::getVideoSettings() const
{
    return m_videoSettings;
}

const App::GameSettings& App::getGameSettings() const
{
    return m_gameSettings;
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

    m_renderWindow.setIcon(icon_width, icon_height, icon_arr);
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
    lastScoreIndex = m_scores.add(name, value, m_gameSettings.difficulty);
}

int App::getLastScoreIndex() const
{
    return lastScoreIndex;
}

const std::vector<Scores::Item>& App::getScores() const
{
    return m_scores.getScores(m_gameSettings.difficulty);
}

void App::setPlayerInitials(const std::string& initials)
{
    std::string str(initials);
    while (str.size() < 3) str.push_back('-');
    if (str.size() > 3) str = str.substr(0, 3);

    std::strcpy(&m_gameSettings.playerInitials[0], str.c_str());
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

#ifdef P_DEBUG_
        if (evt.type == sf::Event::KeyPressed
            && evt.key.code == sf::Keyboard::Escape)
        {
            m_renderWindow.close();
        }
#endif //P_DEBUG_

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
        auto msg = m_messageBus.poll();

        switch (msg.type)
        {
        case Message::Type::Player:
            if (msg.player.action == Message::PlayerEvent::Spawned)
            {
                m_gameSettings.difficulty = m_pendingDifficulty;
            }
            break;
        case Message::Type::UI:
            switch (msg.ui.type)
            {
            case Message::UIEvent::RequestDifficultyChange:
                m_pendingDifficulty = msg.ui.difficulty;
                break;
            case Message::UIEvent::RequestAudioMute:
                m_audioSettings.muted = true;
                break;
            case Message::UIEvent::RequestAudioUnmute:
                m_audioSettings.muted = false;
                break;
            case Message::UIEvent::RequestVolumeChange:
                m_audioSettings.volume = msg.ui.value;
                break;
            case Message::UIEvent::RequestControlsArcade:
                m_gameSettings.controlType = ControlType::Arcade;
                break;
            case Message::UIEvent::RequestControlsClassic:
                m_gameSettings.controlType = ControlType::Classic;
                break;
            case Message::UIEvent::RequestControllerEnable:
                m_gameSettings.controllerEnabled = true;
                break;
            case Message::UIEvent::RequestControllerDisable:
                m_gameSettings.controllerEnabled = false;
                break;
            case Message::UIEvent::RequestColourblindEnable:
                m_gameSettings.colourblindMode = true;
                break;
            case Message::UIEvent::RequestColourblindDisable:
                m_gameSettings.colourblindMode = false;
                break;
            default: break;
            }
            break;
        default: break;
        }

        m_stateStack.handleMessage(msg);
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
    m_stateStack.registerState<HelpState>(States::ID::Help);
    m_stateStack.registerState<TitleState>(States::ID::Title);
}

void App::loadSettings()
{
    //OS X Resource Path
    std::string resPath("");
    #ifdef __APPLE__
    resPath = resourcePath();
    #endif
    
    std::fstream file(resPath + settingsFile, std::ios::binary | std::ios::in);
    if (!file.good() || !file.is_open() || file.fail())
    {
        Logger::Log("failed to open settings file for reading", Logger::Type::Warning, Logger::Output::All);
        Logger::Log("file probably missing - new file will be created", Logger::Type::Warning, Logger::Output::All);
        file.close();
        return;
    }

    //check file size
    file.seekg(0, std::ios::end);
    int fileSize = static_cast<int>(file.tellg());
    file.seekg(0, std::ios::beg);

    if (fileSize < static_cast<int>(sizeof(SettingsFile)))
    {
        Logger::Log("settings file not expected file size", Logger::Type::Error, Logger::Output::All);
        file.close();
        return;
    }

    SettingsFile settings;
    file.read((char*)&settings, sizeof(SettingsFile));
    file.close();
    
    if (settings.ident != settingsIdent || settings.version != settingsVersion)
    {
        Logger::Log("settings file invalid or wrong version", Logger::Type::Error, Logger::Output::All);
        return;
    }

    VideoSettings  newVideoSettings;
    newVideoSettings.VideoMode = settings.videoMode;
    newVideoSettings.WindowStyle = settings.windowStyle;

    m_audioSettings = settings.audioSettings;
    m_audioSettings.volume = Util::Math::clamp(m_audioSettings.volume, 0.f, 1.f);

    m_gameSettings = settings.gameSettings;

    applyVideoSettings(newVideoSettings);
}

void App::saveSettings()
{
    //OS X Resource Path
    std::string resPath("");
    #ifdef __APPLE__
    resPath = resourcePath();
    #endif
    
    std::fstream file(resPath + settingsFile, std::ios::binary | std::ios::out);
    if (!file.good() || !file.is_open() || file.fail())
    {
        Logger::Log("failed to open settings file for writing", Logger::Type::Error, Logger::Output::All);
        file.close();
        return;
    }

    SettingsFile settings;
    settings.ident = settingsIdent;
    settings.version = settingsVersion;
    settings.videoMode = m_videoSettings.VideoMode;
    settings.windowStyle = m_videoSettings.WindowStyle;

    settings.audioSettings = m_audioSettings;
    settings.gameSettings = m_gameSettings;

    file.write((char*)&settings, sizeof(SettingsFile));
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
    
    //get the "Pictures" folder if we are on OS X
    //as we can't save to working directory (which would be "Applications")
    std::string picturesPath("");
    #ifdef __APPLE__
    std::string user = getenv("USER");
    picturesPath = "/Users/" + user + "/Pictures/";
    #endif

    sf::Texture t;
    t.create(m_renderWindow.getSize().x, m_renderWindow.getSize().y);
    t.update(m_renderWindow);

    sf::Image screenCap = t.copyToImage();
    if (!screenCap.saveToFile(fileName)) Logger::Log("failed to save " + fileName, Logger::Type::Error, Logger::Output::File);
}