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

namespace
{
    float musicVolume = 100.f;
    float fxVolume = 60.f;
    float fadeDelay = 2.f; //delay starting the fade while the initial state is loaded
    const std::string impactSoundPath = "assets/sound/chimes01/";
    const std::string fxSoundPath = "assets/sound/fx/";
}

AudioManager::AudioManager()
    : m_fadeInTime      (4.f),
    m_currentFadeTime   (0.f)
{   
    m_musicPlayer.setVolume(0.f);
    m_musicPlayer.play("assets/sound/background.ogg", true);

    m_soundPlayer.setVolume(0.f);
    auto files = FileSystem::listFiles(impactSoundPath);
    for (const auto& file : files)
    {
        if (FileSystem::getFileExtension(file) == ".wav")
        {
            m_impactSounds.emplace_back(sf::SoundBuffer());
            m_impactSounds.back().loadFromFile(impactSoundPath + file);
        }
    }

    files = FileSystem::listFiles(fxSoundPath);
    for (const auto& file : files)
    {
        if (FileSystem::getFileExtension(file) == ".wav")
        {
            m_fxSounds.emplace_back(sf::SoundBuffer());
            m_fxSounds.back().loadFromFile(fxSoundPath + file);
        }
    }

    m_switchFx.loadFromFile("assets/sound/switch.wav");
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
    if (msg.type == Message::Type::Physics)
    {
        { 
            m_soundPlayer.play(m_impactSounds[Util::Random::value(0, m_impactSounds.size() - 1)]);
        }
    }
    else if (msg.type == Message::Type::Entity)
    {
        if (msg.entity.maxCollisionsReached)
        {
            //play a swooshy sound :D
            m_soundPlayer.play(m_switchFx);
        }
    }
}