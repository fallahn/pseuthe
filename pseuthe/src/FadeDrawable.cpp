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

#include <FadeDrawable.hpp>
#include <Log.hpp>
#include <MessageBus.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

namespace
{
    const float fadeTime = 1.f;
    const float delayTime = 2.f;
}

FadeDrawable::FadeDrawable(MessageBus& mb)
    : Component         (mb),
    m_currentFadeTime   (fadeTime),
    m_delayTime         (delayTime)
{
    m_rectangleShape.setFillColor(sf::Color::Black);
    m_rectangleShape.setSize({ 1920.f, 1080.f });
}

//public
Component::Type FadeDrawable::type() const
{
    return Component::Type::Drawable;
}

void FadeDrawable::entityUpdate(Entity&, float dt)
{
    if (m_delayTime > 0)
    {
        m_delayTime -= dt;
    }
    else
    {
        float ratio = m_currentFadeTime / fadeTime;

        auto colour = m_rectangleShape.getFillColor();
        colour.a = static_cast<sf::Uint8>(255.f * std::max(ratio, 0.f));
        m_rectangleShape.setFillColor(colour);

        m_currentFadeTime -= dt;
        if (m_currentFadeTime < 0)
        {
            destroy();

            /*Message msg;
            msg.type = Message::Type::UI;
            msg.ui.type = Message::UIEvent::RequestState;
            msg.ui.stateId = States::ID::Menu;
            sendMessage(msg);*/
        }

        //LOG(std::to_string(ratio), Logger::Type::Info);
    }
}

void FadeDrawable::handleMessage(const Message&)
{

}

//private
void FadeDrawable::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    //states.blendMode = sf::BlendMultiply;
    rt.draw(m_rectangleShape, states);
}