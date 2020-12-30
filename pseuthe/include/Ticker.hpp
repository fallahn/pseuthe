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

#ifndef TICKER_HPP_
#define TICKER_HPP_

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include <vector>

class Ticker final : private sf::NonCopyable, public sf::Drawable, public sf::Transformable
{
public:
    explicit Ticker(sf::Font& font);
    ~Ticker() = default;

    void update(float dt);
    void addItem(const std::string& text);
    void setSpeed(float speed);
    void setSize(sf::FloatRect size);
    std::size_t getMessageCount() const;
    void setColour(const sf::Color&);

private:
    sf::Font& m_font;
    std::vector<sf::Text> m_messages;
    sf::FloatRect m_size;
    float m_speed;
    float m_totalWidth;

    void draw(sf::RenderTarget& rt, sf::RenderStates states)const override;
};


#endif //TICKER_H_