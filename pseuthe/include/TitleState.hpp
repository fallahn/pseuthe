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

//displays splash screen at startup

#ifndef TITLE_STATE_HPP_
#define TITLE_STATE_HPP_

#include <State.hpp>
#include <SoundPlayer.hpp>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

class MessageBus;
class TitleState final : public State
{
public:
    TitleState(StateStack&, Context);
    ~TitleState() = default;

    bool handleEvent(const sf::Event&) override;
    void handleMessage(const Message&) override;
    bool update(float dt) override;
    void draw() override;

private:
    sf::Sprite m_sprite;
    sf::RectangleShape m_rectangleShape;
    sf::Shader m_noiseShader;
    sf::Shader m_lineShader;
    float m_windowRatio;

    MessageBus& m_mesageBus;

    float m_fadeTime;
    enum class Fade
    {
        In,
        Hold,
        Out
    }m_fade;

    SoundPlayer m_soundPlayer;
    sf::SoundBuffer m_startupSound;
};


#endif //TITLE_STATE_HPP_