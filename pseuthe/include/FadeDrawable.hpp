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

//full screen rectangle whih fades to nothing and destroys itself

#ifndef FADE_DRAWABLE_HPP_
#define FADE_DRAWABLE_HPP_

#include <Component.hpp>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

class FadeDrawable final : public Component, public sf::Drawable
{
public:
    explicit FadeDrawable(MessageBus&);
    ~FadeDrawable() = default;
    FadeDrawable(const FadeDrawable&) = delete;
    const FadeDrawable& operator = (const FadeDrawable&) = delete;

    Component::Type type() const override;
    void entityUpdate(Entity&, float) override;
    void handleMessage(const Message&) override;

private:

    sf::RectangleShape m_rectangleShape;
    float m_currentFadeTime;
    float m_delayTime;

    void draw(sf::RenderTarget&, sf::RenderStates) const override;
};


#endif //FADE_DRAWABLE_HPP_