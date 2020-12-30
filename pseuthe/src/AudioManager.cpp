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

#include <AudioManager.hpp>
#include <MessageBus.hpp>
#include <Util.hpp>
#include <Log.hpp>
#include <FileSystem.hpp>

#include <SFML/Audio/Listener.hpp>

//Needed to access resources on OS X
#ifdef __APPLE__
#include <ResourcePath.hpp>
#endif

namespace
{
    const float maxMusicVolume = 100.f;
    float musicVolume = 100.f;
    const float maxFxVolume = 85.f;
    float fxVolume = 60.f;

    float fadeDelay = 2.f; //delay starting the fade while the initial state is loaded
    const std::string impactSoundPath = "assets/sound/chimes01/";
}

AudioManager::AudioManager()
    : m_fadeInTime      (4.f),
    m_currentFadeTime   (0.f),
    m_fxSounds          (SoundIds::Size),
    m_muted             (false)
{
    std::string resPath("");
    //if it's OS X, prepend the resourcePath
    #ifdef __APPLE__
    resPath = resourcePath();
    #endif
    
    m_musicPlayer.setVolume(0.f);
    m_musicPlayer.play(resPath + "assets/sound/background.ogg", true);

    m_soundPlayer.setVolume(0.f);
    auto files = FileSystem::listFiles(resPath + impactSoundPath);
    for (const auto& file : files)
    {
        if (FileSystem::getFileExtension(file) == ".wav")
        {
            m_impactSounds.emplace_back(sf::SoundBuffer());
            m_impactSounds.back().loadFromFile(resPath + impactSoundPath + file);
        }
    }
    
    m_fxSounds[SwitchFx].loadFromFile(resPath + "assets/sound/switch.wav");
    m_fxSounds[HealthLost].loadFromFile(resPath + "assets/sound/healthlost.wav");
    m_fxSounds[HealthGained].loadFromFile(resPath + "assets/sound/healthgained.wav");
    m_fxSounds[Eating].loadFromFile(resPath + "assets/sound/nomnom.wav");
    m_fxSounds[AteJelly].loadFromFile(resPath + "assets/sound/jelly_eat.wav");
    m_fxSounds[Spawned].loadFromFile(resPath + "assets/sound/player_spawn.wav");
}


//public
void AudioManager::update(float dt)
{
    if (fadeDelay > 0)
    {
        fadeDelay -= dt;
    }
    else if (m_currentFadeTime < m_fadeInTime)
    {
        float ratio = std::min(m_currentFadeTime / m_fadeInTime, 1.f);
        m_musicPlayer.setVolume(musicVolume * ratio);
        m_soundPlayer.setVolume(fxVolume * ratio);
        m_currentFadeTime += dt;
    }

    m_soundPlayer.update();
}

void AudioManager::handleMessage(const Message& msg)
{
    //You has no seagulls.
    switch (msg.type)
    {
    case Message::Type::Physics:
        switch (msg.physics.event)
        {
        case Message::PhysicsEvent::Collision:
            if (m_muted) break;
                m_soundPlayer.play(m_impactSounds[Util::Random::value(0, static_cast<std::int32_t>(m_impactSounds.size() - 1))]);
                break;
        default:break;
        }
        break;
    case Message::Type::Entity:
        if (!m_muted && msg.entity.maxCollisionsReached)
        {
            //play a swooshy sound :D
            m_soundPlayer.play(m_fxSounds[SwitchFx]);
        }
        break;
    case Message::Type::UI:
        switch (msg.ui.type)
        {
        case Message::UIEvent::RequestVolumeChange:
            musicVolume = std::min(maxMusicVolume * msg.ui.value, maxMusicVolume);
            fxVolume = std::min(maxFxVolume * msg.ui.value, maxFxVolume);

            if (fadeDelay <= 0)
            {
                m_musicPlayer.setVolume(musicVolume);
                m_soundPlayer.setVolume(fxVolume);
            }

            break;
        case Message::UIEvent::RequestAudioMute:
            m_muted = true;
            m_musicPlayer.setPaused(true);
            break;
        case Message::UIEvent::RequestAudioUnmute:
            if (m_muted)
            {
                m_muted = false;
                m_musicPlayer.setPaused(false);
            }
            break;
        default: break;
        }
        break;
    case Message::Type::Player:
        switch (msg.player.action)
        {
        case Message::PlayerEvent::HealthAdded:
            (msg.player.value > 100) ? m_soundPlayer.play(m_fxSounds[AteJelly]) : m_soundPlayer.play(m_fxSounds[HealthGained]);
            break;
        case Message::PlayerEvent::HealthLost:
            m_soundPlayer.play(m_fxSounds[HealthLost]);
            break;
        case Message::PlayerEvent::BeganEating:
            m_soundPlayer.play(m_fxSounds[Eating]);
            break;
        case Message::PlayerEvent::Spawned:
            m_soundPlayer.play(m_fxSounds[Spawned]);
            break;
        default:break;
        }
        break;
    default: break;
    }
}

void AudioManager::mute(bool m)
{
    m_muted = m;
    m_musicPlayer.setPaused(m);
}