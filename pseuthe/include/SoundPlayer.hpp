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

//plays a given sound effect

#ifndef SOUND_PLAYER_HPP_
#define SOUND_PLAYER_HPP_

#include <SFML/System/Vector2.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>

#include <map>
#include <list>

class SoundPlayer final
{
public:
    enum class AudioId
    {
        
    };

    SoundPlayer();
    ~SoundPlayer() = default;
    SoundPlayer(const SoundPlayer&) = delete;
    const SoundPlayer& operator = (const SoundPlayer&) = delete;

    void update();
    void play(AudioId id);
    void play(AudioId id, const sf::Vector2f& position, bool loop = false);
    void play(AudioId, const sf::Vector3f& position);
    
    void setListenerPosition(const sf::Vector2f& position);
    sf::Vector2f getListenerPosition() const;
    void cacheSound(AudioId, const std::string&);

    static void setVolume(float volume);
    static float getVolume();

private:

    std::map<AudioId, sf::SoundBuffer> m_buffers;
    std::list<sf::Sound> m_sounds;

    void flushSounds();
};

#endif //SOUND_PLAYER_HPP_