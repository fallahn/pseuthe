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

#include <TitleState.hpp>
#include <App.hpp>
#include <Shaders.hpp>
#include <Util.hpp>

#include <SFML/Window/Event.hpp>

//Needed to access resources on OS X
#ifdef __APPLE__
#include <ResourcePath.hpp>
#endif

namespace
{
    const float fadeTime = 1.f;
    const float holdTime = 3.5f;

    float accumulatedTime = 0.f;
    const float scanlineCount = 6500.f;
}

TitleState::TitleState(StateStack& stack, Context context)
    : State         (stack, context),
    m_windowRatio   (static_cast<float>(context.renderWindow.getSize().y) / context.defaultView.getSize().y),
    m_mesageBus     (context.appInstance.getMessageBus()),
    m_fadeTime      (0.f),
    m_fade          (Fade::In)
{
    std::string resPath("");
    //if it's OS X, prepend the resourcePath
    #ifdef __APPLE__
    resPath = resourcePath();
    #endif
    
    m_sprite.setTexture(context.appInstance.getTexture("assets/images/startup.png"));   //resPath not needed here as it does it inside Resource.hpp
    m_rectangleShape.setSize({ 1920.f, 1080.f });

    m_noiseShader.loadFromMemory(Shader::PostChromeAb::fragment, sf::Shader::Fragment);
    m_noiseShader.setUniform("u_lineCount", m_windowRatio  * scanlineCount);
    m_noiseShader.setUniform("u_sourceTexture", *m_sprite.getTexture());

    m_lineShader.loadFromMemory(Shader::Scanline::fragment, sf::Shader::Fragment);
    m_lineShader.setUniform("u_sourceTexture", *m_sprite.getTexture());

    context.renderWindow.setView(context.defaultView);

    m_startupSound.loadFromFile(resPath + "assets/sound/startup.wav");
    m_soundPlayer.play(m_startupSound);
}

//public
bool TitleState::handleEvent(const sf::Event& evt)
{ 
    if (evt.type == sf::Event::KeyReleased
        || evt.type == sf::Event::JoystickButtonReleased)
    {
        requestStackPop();
        requestStackPush(States::ID::Main);
        requestStackPush(States::ID::Menu);
    }
    return false;
}
void TitleState::handleMessage(const Message&){}

bool TitleState::update(float dt)
{
    accumulatedTime += dt;
    m_noiseShader.setUniform("u_time", accumulatedTime * (10.f * m_windowRatio));

    sf::Color colour = sf::Color::Black;
    m_fadeTime += dt;
    switch (m_fade)
    {
    case Fade::In:
        colour.a = static_cast<sf::Uint8>(std::min(1.f - ((m_fadeTime / fadeTime) * 255.f), 0.f));
        if (m_fadeTime >= fadeTime)
        {
            m_fade = Fade::Hold;
            m_fadeTime = 0.f;
        }
        break;
    case Fade::Hold:
        colour.a = 0u;
        if (m_fadeTime >= holdTime)
        {
            m_fadeTime = 0.f;
            m_fade = Fade::Out;
        }
        break;
    case Fade::Out:
        colour.a = static_cast<sf::Uint8>(std::min((m_fadeTime / fadeTime) * 255.f, 255.f));
        if (m_fadeTime >= fadeTime)
        {
            requestStackPop();
            requestStackPush(States::ID::Main);
            requestStackPush(States::ID::Menu);
        }
        break;
    default: break;
    }
    m_rectangleShape.setFillColor(colour);

    return false;
}

void TitleState::draw()
{
    sf::RenderStates states;
    states.shader = &m_noiseShader;

    auto& rw = getContext().renderWindow;
    rw.draw(m_sprite, states);

    if (Util::Random::value(0, 10) == 0)
    {
        m_sprite.setPosition(9.f, -7.f);
        //states.blendMode = sf::BlendAdd;
        states.shader = &m_lineShader;
        rw.draw(m_sprite, states);
        m_sprite.setPosition(0.f, 0.f);
    }

    rw.draw(m_rectangleShape);
}

//private