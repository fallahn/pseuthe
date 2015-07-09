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

//a circle shape drawable component

#ifndef CIRCLE_DRAWABLE_HPP_
#define CIRCLE_DRAWABLE_HPP_

#include <Component.hpp>

#include <SFML/Graphics/CircleShape.hpp>

namespace sf
{
    class Shader;
}

class CircleDrawable final : public Component, public sf::Drawable
{
public:
    using Ptr = std::unique_ptr<CircleDrawable>;

    CircleDrawable(float radius, MessageBus&);
    ~CircleDrawable() = default;

    Component::Type type() const override;
    void entityUpdate(Entity&, float) override;
    void handleMessage(const Message&) override;

    void setColour(sf::Color);
    void setRadius(float);
    void setOutlineThickness(float);

    const sf::Color& getColour() const;

    void setShader(sf::Shader&);
    void setTexture(const sf::Texture&);
    void setNormalMap(const sf::Texture&);

private:

    sf::CircleShape m_circleShape;
    sf::Shader* m_shader;
    const sf::Texture* m_normalMap;

    void draw(sf::RenderTarget& rt, sf::RenderStates states) const override;
};

#endif //CIRCLE_DRAWABLE_HPP_